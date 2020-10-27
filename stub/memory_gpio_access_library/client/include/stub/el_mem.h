/*
 * @copyright Copyright (c) 2016-2020 TOYOTA MOTOR CORPORATION.
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
 * @file el_mem.h
 * @brief el_mem function header file
 */

#ifndef MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_MEM_H_
#define MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_MEM_H_

/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup memory_gpio_access_library
 *  @ingroup native_service
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <sys/types.h>

/**
* \~english Definitions of the Parametar of type
*/
enum {
  EL_MEM_TYPE_REGION_SYS,            //!< \~english SYS region
  EL_MEM_TYPE_REGION_LOG,            //!< \~english LOG region
  EL_MEM_TYPE_REGION_COM,            //!< \~english COMMUNICATION region
  EL_MEM_TYPE_REGION_NAVI,           //!< \~english NAVI region
  EL_MEM_TYPE_REGION_SH4A,           //!< \~english SH4A region
  EL_MEM_TYPE_REGION_PRAMFS,         //!< \~english PRAMFS region
  EL_MEM_TYPE_REGION_STARTUPBGM,     //!< \~english STARTUPBGM region
  EL_MEM_TYPE_REGION_NAVI_IR,        //!< \~english NAVI picture recognition region
  EL_MEM_TYPE_REGION_MAX             //!< \~english (internal use)
};

/**
* \~english Definitions of the Parametar of memory id
*/
enum {
  // SYS region
  EL_MEM_ID_SYS_ALL   = 0x0001,          //!< \~english all SYS region
  EL_MEM_ID_SYS_BOOT,                    //!< \~english particular: BOOT region

  // LOG region
  EL_MEM_ID_LOG_ALL = 0x1001,            //!< \~english all LOG region

  // COM region
  EL_MEM_ID_COM_ALL = 0x2001,            //!< \~english all COMMUNICATION region
  EL_MEM_ID_COM_SH4A,            //!< \~english particular: SH4A abnornal observation

  // NAVI region
  EL_MEM_ID_NAVI_ALL  = 0x3001,          //!< \~english NAVI region
  EL_MEM_ID_NAVI_LOG,                //!< \~english particular: NAVI(AW) LOG region

  // SH4A region
  EL_MEM_ID_SH4A_ALL  = 0x4001,          //!< \~english SH4A region
  EL_MEM_ID_SH4A_SHM,   //!< \~english particular: SH4A SH share memory region

  // PRAMFS region NOTE:Can be used only when ROM operation
  EL_MEM_ID_PRAMFS_ALL = 0x5001,         //!< \~english all PRAMFS region

  // STARTUPBGM region
  EL_MEM_ID_STARTUPBGM_ALL = 0x6001,     //!< \~english all STARTUPBGM region

  // NAVI region (Picture Recognition: For Image Recognition)
  EL_MEM_ID_NAVI_IR_ALL = 0x7001,  //!< \~english all NAVI picture recognition region
};

/**
 * \~english Valid cache_flag
 */
#define EL_MEM_CACHE_VALID      1

/**
 * \~english Invalid cache_flag
 */
#define EL_MEM_CACHE_INVALID    0

/**\ingroup EL_mem_getOffset
* \~english @par Brief
*        Get offset of function region according to mem_id.
* \~english @param [in] mem_id
*        int   - memory id
* \~english @par
*        Defined memory id as follows.
* \~english @code
*        enum {
*                   // SYS
*                   EL_MEM_ID_SYS_ALL   = 0x0001,          // all SYS region
*                   EL_MEM_ID_SYS_BOOT,                    // particular: BOOT region
*                   // COM
*                   EL_MEM_ID_COM_ALL = 0x2001,            // all COMMUNICATION region
*                   EL_MEM_ID_COM_SH4A,                    // particular: SH4A abnornal observation
*                   // NAVI
*                   EL_MEM_ID_NAVI_ALL  = 0x3001,          // NAVI region
*                   EL_MEM_ID_NAVI_LOG,                    // particular: NAVI(AW) LOG region
*                   // SH4A
*                   EL_MEM_ID_SH4A_ALL  = 0x4001,          // SH4A region
*                   EL_MEM_ID_SH4A_SHM,                    // particular: SH4A SH share memory region
*                   // PRAMFS NOTE:ROM only
*                   EL_MEM_ID_PRAMFS_ALL = 0x5001,         // all PRAMFS region
*                   // STARTUPBGM
*                   EL_MEM_ID_STARTUPBGM_ALL = 0x6001,     // all STARTUPBGM region
*                   // NAVI (Image Recognition)
*                   EL_MEM_ID_NAVI_IR_ALL = 0x7001,        // all NAVI picture recognition region
*        };
*            @endcode
* \~english @retval offset Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Parameter mem_id is not defined.[-1]
* \~english @par Detail
*        - Get offset of function region according to mem_id.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see none
*/
off_t EL_mem_getOffset(int mem_id);  // NOLINT [readability/naming]

/**\ingroup EL_mem_getLength
* \~english @par Brief
*        Get length of function region according to mem_id.
* \~english @param [in] mem_id
*        int   - memory id
* \~english @par
*        Defined memory id as follows.
* \~english @code
*        enum {
*                   // SYS
*                   EL_MEM_ID_SYS_ALL   = 0x0001,          // all SYS region
*                   EL_MEM_ID_SYS_BOOT,                    // particular: BOOT region
*                   // COM
*                   EL_MEM_ID_COM_ALL = 0x2001,            // all COMMUNICATION region
*                   EL_MEM_ID_COM_SH4A,                    // particular: SH4A abnornal observation
*                   // NAVI
*                   EL_MEM_ID_NAVI_ALL  = 0x3001,          // NAVI region
*                   EL_MEM_ID_NAVI_LOG,                    // particular: NAVI(AW) LOG region
*                   // SH4A
*                   EL_MEM_ID_SH4A_ALL  = 0x4001,          // SH4A region
*                   EL_MEM_ID_SH4A_SHM,                    // particular: SH4A SH share memory region
*                   // PRAMFS NOTE:ROM only
*                   EL_MEM_ID_PRAMFS_ALL = 0x5001,         // all PRAMFS region
*                   // STARTUPBGM
*                   EL_MEM_ID_STARTUPBGM_ALL = 0x6001,     // all STARTUPBGM region
*                   // NAVI (Image Recognition)
*                   EL_MEM_ID_NAVI_IR_ALL = 0x7001,        // all NAVI picture recognition region
*        };
*            @endcode
* \~english @retval length Success
* \~english @retval 0 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Parameter mem_id is not defined.[0]
*        - syscall error(open(), ioctl()).[0]
* \~english @par Detail
*        - Get length of function region according to mem_id.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see none
*/
size_t EL_mem_getLength(int mem_id);  // NOLINT [readability/naming]

/**\ingroup EL_mem_exram_mmap_simple
* \~english @par Brief
*        Map device file to memory.(simple)
* \~english @param [in] mem_id
*        int   - memory id
* \~english @param [in] prot
*        int   - Linux standard (same to mmap() of /dev/mem)
* \~english @param [in] flags
*        int   - Linux standard (same to mmap() of /dev/mem)
* \~english @param [in] cache_flag
*        int   - cache flag valid/invalid
* \~english @par
*        Defined memory id as follows.
* \~english @code
*        enum {
*                   // SYS
*                   EL_MEM_ID_SYS_ALL   = 0x0001,          // all SYS region
*                   EL_MEM_ID_SYS_BOOT,                    // particular: BOOT region
*                   // COM
*                   EL_MEM_ID_COM_ALL = 0x2001,            // all COMMUNICATION region
*                   EL_MEM_ID_COM_SH4A,                    // particular: SH4A abnornal observation
*                   // NAVI
*                   EL_MEM_ID_NAVI_ALL  = 0x3001,          // NAVI region
*                   EL_MEM_ID_NAVI_LOG,                    // particular: NAVI(AW) LOG region
*                   // SH4A
*                   EL_MEM_ID_SH4A_ALL  = 0x4001,          // SH4A region
*                   EL_MEM_ID_SH4A_SHM,                    // particular: SH4A SH share memory region
*                   // PRAMFS NOTE:ROM only
*                   EL_MEM_ID_PRAMFS_ALL = 0x5001,         // all PRAMFS region
*                   // STARTUPBGM
*                   EL_MEM_ID_STARTUPBGM_ALL = 0x6001,     // all STARTUPBGM region
*                   // NAVI (Image Recognition)
*                   EL_MEM_ID_NAVI_IR_ALL = 0x7001,        // all NAVI picture recognition region
*        };
*            @endcode
* \~english @par
*        Cache flag as follows.
* \~english @code
*                 #define EL_MEM_CACHE_VALID      1   // valid cache
*                 #define EL_MEM_CACHE_INVALID    0   // invalid cache
*            @endcode
* \~english @retval address Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Parameter mem_id is not defined.[-1]
*        - Open device file failed.[-1]
*        - Map device file to memory error.[-1]
* \~english @par Detail
*        - Get type, offset and length from internal table specified by mem_id, then\n
*        call \ref EL_mem_exram_mmap.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_mem_getLength, EL_mem_exram_mmap, EL_mem_exram_munmap
*/
void *EL_mem_exram_mmap_simple(int mem_id , int prot, int flags , int cache_flag);  // NOLINT [readability/naming]

/**\ingroup EL_mem_exram_mmap
* \~english @par Brief
*        Map device file to memory.
* \~english @param [in] type
*        int   - memory type
* \~english @param [in] offset
*        off_t   - offset(From 0\ref EL_mem_getOffset)
* \~english @param [in] length
*        size_t   - length(From 1\ref EL_mem_getLength)
* \~english @param [in] prot
*        int   - Linux standard (same to mmap() of /dev/mem)
* \~english @param [in] flags
*        int   - Linux standard (same to mmap() of /dev/mem)
* \~english @param [in] cache_flag
*        int   - cache flag valid/invalid
* \~english @par
*        Defined memory type as follows.
* \~english @code
*        enum {
*                   EL_MEM_TYPE_REGION_SYS,            // SYS
*                   EL_MEM_TYPE_REGION_COM,            // COMMUNICATION
*                   EL_MEM_TYPE_REGION_NAVI,           // NAVI
*                   EL_MEM_TYPE_REGION_SH4A,           // SH4A
*                   EL_MEM_TYPE_REGION_PRAMFS,         // PRAMFS
*                   EL_MEM_TYPE_REGION_STARTUPBGM,     // STARTUPBGM
*                   EL_MEM_TYPE_REGION_NAVI_IR,        // NAVI IMAGE
*        };
*            @endcode
* \~english @par
*        Cache flag as follows.
* \~english @code
*                 #define EL_MEM_CACHE_VALID      1   // valid cache
*                 #define EL_MEM_CACHE_INVALID    0   // invalid cache
*            @endcode
* \~english @retval address Success
* \~english @retval -1 Error
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Open device file failed.[-1]
*        - Map device file to memory error.[-1]
* \~english @par Detail
*        - Access device file specified by memory type, map function region to user space.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_mem_exram_munmap
*/
void *EL_mem_exram_mmap(int type, off_t offset , size_t length , int prot, int flags , int cache_flag);  // NOLINT [readability/naming]

/**\ingroup EL_mem_exram_munmap
* \~english @par Brief
*        Unmap device file from memory.
* \~english @param [in] addr
*        void *   - address
* \~english @param [in] length
*        size_t   - size (1 ~ \ref EL_mem_getLength)
* \~english @retval 0 Success
* \~english @retval -1 Error:Set errno according to error type when error occurs.
* \~english @par Prerequisite
*        - Map device file to memory(\ref EL_mem_exram_mmap,
*          \ref EL_mem_exram_mmap_simple).
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Unmap device file from memory error.[-1]
* \~english @par Detail
*        - Unmap specified address.Error occurs when refer to this memory after unmap.
*        - Automatic unmap when process ends.
*        - The memory will not be unmap even the file descriptor is closed.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see EL_mem_exram_mmap, EL_mem_exram_mmap_simple
*/
int EL_mem_exram_munmap(void *addr, size_t length);  // NOLINT [readability/naming]

/**\ingroup EL_mem_getPhysicalAddress
* \~english @par Brief
*        Get physical address.
* \~english @param [in] mem_id
*        int   - memory id
* \~english @par
*        Defined memory id as follows.
* \~english @code
*        enum {
*                   // SYS
*                   EL_MEM_ID_SYS_ALL   = 0x0001,          // all SYS region
*                   EL_MEM_ID_SYS_BOOT,                    // particular: BOOT region
*                   // COM
*                   EL_MEM_ID_COM_ALL = 0x2001,            // all COMMUNICATION region
*                   EL_MEM_ID_COM_SH4A,                    // particular: SH4A abnornal observation
*                   // NAVI
*                   EL_MEM_ID_NAVI_ALL  = 0x3001,          // NAVI region
*                   EL_MEM_ID_NAVI_LOG,                    // particular: NAVI(AW) LOG region
*                   // SH4A
*                   EL_MEM_ID_SH4A_ALL  = 0x4001,          // SH4A region
*                   EL_MEM_ID_SH4A_SHM,                    // particular: SH4A SH share memory region
*                   // PRAMFS NOTE:ROM only
*                   EL_MEM_ID_PRAMFS_ALL = 0x5001,         // all PRAMFS region
*                   // STARTUPBGM
*                   EL_MEM_ID_STARTUPBGM_ALL = 0x6001,     // all STARTUPBGM region
*                   // NAVI (Image Recognition)
*                   EL_MEM_ID_NAVI_IR_ALL = 0x7001,        // all NAVI picture recognition region
*        };
*            @endcode
* \~english @retval physical address Success
* \~english @retval 0 Error: Failed to open device file or get physical address(ioctl)
* \~english @retval -1 Error: Parameter mem_id is not defined.
* \~english @par Prerequisite
*        - None.
* \~english @par Change of internal state
*        - Change of internal state according to the API does not occur.
* \~english @par Conditions of processing failure
*        - Parameter mem_id is not defined.[-1]
*        - Get physical address error.[0]
* \~english @par Detail
*        - Get physical address from dummy-memory driver according to device file specified by mem_id.
*        - Return the sum value of physical address and offset specified by mem_id.
* \~english @par Classification
*        Public
* \~english @par Type
*        Sync
* \~english @see none
*/
u_int64_t EL_mem_getPhysicalAddress(int mem_id);  // NOLINT [readability/naming]

#ifdef __cplusplus
}
#endif  // __cplusplus

/** @}*/  // end of memory_gpio_access_library
/** @}*/  // end of native_service
/** @}*/  // end of BaseSystem
#endif  // MEMORY_GPIO_ACCESS_LIBRARY_CLIENT_INCLUDE_NATIVE_SERVICE_EL_MEM_H_
