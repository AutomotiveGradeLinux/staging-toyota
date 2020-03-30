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
 * @file cl_region.h
 * @brief region manage
 *
 */

#ifndef _CL_REGION_H_  // NOLINT(build/header_guard)
#define _CL_REGION_H_  // NOLINT(build/header_guard)

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define CL_REGION_DEFAULT_SIZE    (4 * 1024)

#define CL_ALIGNOF(type) __alignof(type)

typedef void (*cl_region_cleanup_pt)(void *data);

typedef struct cl_region_cleanup_s  cl_region_cleanup_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        region cleanup struct
/////////////////////////////////////////////////////////////////////////////////////
struct cl_region_cleanup_s {  // NOLINT(readability/nolint)
  cl_region_cleanup_pt handler;
  void                 *data;
  cl_region_cleanup_t  *next;
};


typedef struct cl_region_large_s  cl_region_large_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        region large struct
/////////////////////////////////////////////////////////////////////////////////////
struct cl_region_large_s {  // NOLINT(readability/nolint)
  cl_region_large_t *next;
  void              *alloc;
  size_t            size;
};


typedef struct cl_region_s cl_region_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        region data struct
/////////////////////////////////////////////////////////////////////////////////////
typedef struct {
  uint8_t     *last;
  uint8_t     *end;
  cl_region_t *next;
  int         failed;
} cl_region_data_t;

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Monitor
/// \~english @par Brief
///        region struct
/////////////////////////////////////////////////////////////////////////////////////
struct cl_region_s {  // NOLINT(readability/nolint)
  cl_region_data_t    d;
  size_t              max;
  cl_region_t         *current;
  cl_region_large_t   *large;
  cl_region_cleanup_t *cleanup;
};


#ifdef __cplusplus
extern "C" {
#endif
/**
 * @file cl_region.h
 */

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup common_library
 *  @ingroup native_service
 *  @{
 */

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Region
/// \~english @par Brief
///     Region create
/// \~english @param [in] size
///     size_t - the size to create
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
cl_region_t *CL_RegionCreate(size_t size);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Region
/// \~english @par Brief
///     Region destroy
/// \~english @param [in] region
///     cl_region_t * - the region to destroy
/// \~english @retval 0  Success
/// \~english @retval -1 Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
void CL_RegionDestroy(cl_region_t *region);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Region
/// \~english @par Brief
///     Region allocation
/// \~english @param [in] region
///     cl_region_t * - the region to allocate
/// \~english @param [in] size
///     size_t * - the size to allocate
/// \~english @param [in] align_size
///     size_t * - the size to align
/// \~english @retval the pointer to region allocated
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
#define CL_RegionAlloc(region, type, length) \
  cl_region_alloc(region, sizeof(type) * length, CL_ALIGNOF(type))
void *cl_region_alloc(cl_region_t *region, size_t size, size_t align_size);  // NOLINT(readability/nolint)

/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Region
/// \~english @par Brief
///     Region free
/// \~english @param [in] region
///     cl_region_t * - the region to free
/// \~english @param [in] p
///     void * - the pointer to region
/// \~english @retval true  Success
/// \~english @retval false Error
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
bool CL_RegionFree(cl_region_t *region, void *p);  // NOLINT(readability/nolint)


/////////////////////////////////////////////////////////////////////////////////////
/// \ingroup Region
/// \~english @par Brief
///     add region cleanup
/// \~english @param [in] region
///     cl_region_t * - the region to allocate
/// \~english @param [in] size
///     size_t * - the size to add
/// \~english @param [in] align_size
///     size_t * - the size to align
/// \~english @retval the pointer to region cleanup added
/// \~english @par Prerequisite
///     None
/// \~english @par Change of internal state
///     None
/// \~english @par Conditions of processing failure
///     None
/// \~english @par Classification
///     Public
/// \~english @par Type
///     Sync
/// \~english @par Detail
///     None
/// \~english @see None
////////////////////////////////////////////////////////////////////////////////////
#define CL_RegionCleanupAdd(region, type, length) \
  cl_region_cleanup_add(region, sizeof(type) * length, CL_ALIGNOF(type))
cl_region_cleanup_t *cl_region_cleanup_add(cl_region_t *region, size_t size,  // NOLINT(readability/nolint)
                                           size_t align_size);

/** @}*/  // end of common_library
/** @}*/  // end of NativeService
/** @}*/  // end of BaseSystem
#ifdef __cplusplus
}
#endif


#endif /* _CL_REGION_H_ */  // NOLINT(build/header_guard)

/* vim:set ts=8 sw=2 sts=2: */
