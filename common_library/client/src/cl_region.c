/*
 * @copyright Copyright (c) 2016-2019 TOYOTA MOTOR CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file cl_region.c
 * @brief region manage
 *
 */

#include <stdio.h>
#include <sys/mman.h>
#include <native_service/cl_region.h>
#include "cl_error.h"

#define cl_align_ptr(p, a) \
    (uint8_t *) (((long)(p) + ((long)a - 1)) & ~((long)a - 1))

static void *cl_region_anonmmap(size_t size);
static void *cl_region_expand(cl_region_t *region, size_t size, size_t align_size);
static void *cl_region_alloc_large(cl_region_t *region, size_t size);

cl_region_t *
CL_RegionCreate(size_t size) {
  cl_region_t *r;
  long pagesize;

  pagesize = sysconf(_SC_PAGE_SIZE);
  size = ((size + (size_t)pagesize - 1) / (size_t)pagesize) * (size_t)pagesize;

  r = cl_region_anonmmap(size);
  if (r == NULL) {
    return NULL;
  }

  r->d.last = (uint8_t *) r + sizeof(cl_region_t);
  r->d.end = (uint8_t *) r + size;
  r->d.next = NULL;
  r->d.failed = 0;

  size = size - sizeof(cl_region_t);
  r->max = (size < (pagesize - 1)) ? size : (size_t)(pagesize - 1);

  r->current = r;
  r->large = NULL;
  r->cleanup = NULL;

  return r;
}


void
CL_RegionDestroy(cl_region_t *region) {
  cl_region_t *r, *n;
  cl_region_large_t *l;
  cl_region_cleanup_t *c;

  for (c = region->cleanup; c; c = c->next) {
    if (c->handler) {
      CL_DBG_PRINT("run cleanup: %p\n", c->handler);
      c->handler(c->data);
    }
  }

  for (l = region->large; l; l = l->next) {

    CL_DBG_PRINT("free: %p\n", l->alloc);

    if (l->alloc) { // LCOV_EXCL_BR_LINE 6: double check, mmap in cl_monitor.c
      if (munmap(l->alloc, l->size) < 0) { // LCOV_EXCL_BR_LINE 5: fail safe for libc munmap
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        CL_PERROR("munmap"); // LCOV_EXCL_LINE 5: fail safe for libc munmap
      }
    }
  }

  for (r = region, n = region->d.next; /* void */; r = n, n = n->d.next) {
    if (munmap(r, (size_t)(r->d.end - (uint8_t *)r)) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for libc munmap
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      CL_PERROR("munmap"); // LCOV_EXCL_LINE 5: fail safe for libc munmap
    }

    if (n == NULL) {
      break;
    }
  }
}


void *
cl_region_alloc(cl_region_t *region, size_t size, size_t align_size) {
  uint8_t *m;
  volatile uint8_t *old_last;
  cl_region_t *r;

  if (size <= region->max) {

    r = region->current;

    do {
    retry:
      old_last = r->d.last;
      m = cl_align_ptr(old_last, align_size);
      if ((size_t)(r->d.end - m) >= size) {
        if (false == __sync_bool_compare_and_swap(&r->d.last, old_last, m + size)) {
          goto retry;
        }
        return m;
      }

      r = r->d.next;

    } while (r);

    return cl_region_expand(region, size, align_size);
  }

  return cl_region_alloc_large(region, size);
}


static void *
cl_region_anonmmap(size_t size) {
  uint8_t *p;

  p = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (p == MAP_FAILED) {  /* pgr0203 */
    CL_PERROR("mmap");
    return NULL;
  }

  return p;
}


static void *
cl_region_expand(cl_region_t *region, size_t size, size_t align_size) {
  uint8_t *m;
  size_t psize;
  cl_region_t *r, *new, *current;

  psize = (size_t)(region->d.end - (uint8_t *)region);

  m = cl_region_anonmmap(psize);
  if (m == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc mmap
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;// LCOV_EXCL_LINE 5: fail safe for libc mmap
  }

  new = (cl_region_t *)m;

  new->d.end = m + psize;
  new->d.next = NULL;
  new->d.failed = 0;

  m += sizeof(cl_region_data_t);
  m = cl_align_ptr(m, align_size);
  new->d.last = m + size;

  current = region->current;

  for (r = current; r->d.next; r = r->d.next) { /* pgr0689 */
    if (r->d.failed++ > 4) {
      current = r->d.next;
    }
  }

  while (false == __sync_bool_compare_and_swap(&r->d.next, NULL, new)) {
    for (r = r->d.next; r->d.next; r = r->d.next) {}  /* pgr0689 */
  }

  region->current = current ? current : new;

  return m;
}


static void *
cl_region_alloc_large(cl_region_t *region, size_t size) {
  void *p;
  int n;
  long pagesize;
  cl_region_large_t *large;

  pagesize = sysconf(_SC_PAGE_SIZE);
  size = ((size + (size_t)pagesize - 1) / (size_t)pagesize) * (size_t)pagesize;

  p = cl_region_anonmmap(size);

  if (p == NULL) {  // LCOV_EXCL_BR_LINE 5: fail safe for libc mmap
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;// LCOV_EXCL_LINE 5: fail safe for libc mmap
  }

  n = 0;

  for (large = region->large; large; large = large->next) {
    if (large->alloc == NULL) {
      if (false == __sync_bool_compare_and_swap(&large->alloc, NULL, p)) {
        break;
      }
      large->size = size;
      return p;
    }

    if (n++ > 3) {
      break;
    }
  }

  large = CL_RegionAlloc(region, cl_region_large_t, 1);
  if (large == NULL) {  // LCOV_EXCL_START 5: fail safe for libc mmap
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    if (munmap(p, size) < 0) {
      CL_PERROR("munmap");
    }
    return NULL;
  }  // LCOV_EXCL_STOP

  large->alloc = p;
  large->size = size;
  large->next = region->large;
  while (false == __sync_bool_compare_and_swap(&region->large, large->next, large)) {  // LCOV_EXCL_BR_LINE 100: race condition  // NOLINT (readability/nolint)
    // LCOV_EXCL_START 100: race condition
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    large->next = region->large;
  } // LCOV_EXCL_STOP

  return p;
}


bool
CL_RegionFree(cl_region_t *region, void *p) {
  cl_region_large_t  *l;

  for (l = region->large; l; l = l->next) {
    if (p == l->alloc) {
      CL_DBG_PRINT("free: %p\n", l->alloc);
      if (munmap(l->alloc, l->size) < 0) {// LCOV_EXCL_BR_LINE 5: fail safe for libc munmap
        AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
        CL_PERROR("munmap");// LCOV_EXCL_LINE 5: fail safe for libc munmap
      }
      l->alloc = NULL;
      return true;
    }
  }

  return false;
}


cl_region_cleanup_t *
cl_region_cleanup_add(cl_region_t *region, size_t size, size_t align_size) {
  cl_region_cleanup_t  *c;

  c = CL_RegionAlloc(region, cl_region_cleanup_t, 1);
  if (c == NULL) {  // LCOV_EXCL_START 5: fail safe for libc mmap
    AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
    return NULL;
  }  // LCOV_EXCL_STOP

  if (size) {
    c->data = cl_region_alloc(region, size, align_size);
    if (c->data == NULL) {  // LCOV_EXCL_START 5: fail safe for libc mmap
      AGL_ASSERT_NOT_TESTED();  // LCOV_EXCL_LINE 200: test assert
      return NULL;
    }  // LCOV_EXCL_STOP
  } else {
    c->data = NULL;
  }

  c->handler = NULL;
  c->next = region->cleanup;

  region->cleanup = c;

  CL_DBG_PRINT("add cleanup: %p\n", c);

  return c;
}

/* vim:set ts=8 sw=2 sts=2: */
