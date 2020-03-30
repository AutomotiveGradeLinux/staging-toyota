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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// \ingroup  tag_NSSharedMemory
/// \brief    This file contains implementation of class CNSSharedMemWriter.
///           This class provides API to open, close and perform write operation on shared memory.
///
////////////////////////////////////////////////////////////////////////////////////////////////////
//@{
/**
 * @file ns_sharedmem_writer.h
 * @brief \~english This file contains implementation of class CNSSharedMemWriter.
 *        This class provides API to open, close and perform write operation on shared memory.
 *
 */
/** @addtogroup BaseSystem
 *  @{
 */
/** @addtogroup native_service
 *  @ingroup BaseSystem
 *  @{
 */
/** @addtogroup framework_unified
 *  @ingroup native_service
 *  @{
 */
/** @addtogroup native
 *  @ingroup framework_unified
 *  @{
 */
#ifndef __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_WRITER_H__  // NOLINT  (build/header_guard)
#define __NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_WRITER_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <native_service/frameworkunified_types.h>
#include <string>

class CNSSharedMem;

/**
 * @class CNSSharedMemWriter
 * \~english @brief write shared memory
 * \~english @par   Brief Introduction
 *        This class is used for write shared memory.
 *
 */
class CNSSharedMemWriter {
 public:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///        Constructor for CNSSharedMemWriter
  /// \~english @param[in] f_cSharedMemName
  ///        const std::string& - name of the shared memory
  /// \~english @param[in]  f_uiSize
  ///         UI_32 - size of shared memory
  /// \~english @retval None
  /// \~english @par Preconditons
  ///           -None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           Create object of CNSSharedMemWriter class.
  ///           This class don't used to open shared memory object or allocate memory.
  /// \~english @par Classification
  ///           - public
  /// \~english @par Type
  ///           - sync only
  /// \~english @see ~CNSSharedMemWriter
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMemWriter(const std::string &f_cSharedMemName, const UI_32 f_uiSize);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///        Destructor of CNSSharedMemWriter class.
  /// \~english @param None
  /// \~english @retval none
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - Change the state of shared memory to closed state
  /// \~english @par Conditions of processing failure
  ///           -None
  /// \~english @par Detail
  ///      Closes the shared memory, if it is open.
  /// \~english @par
  ///         Please note the following points when using this API for application.
  ///         - Since the destructor does not delete the shared memory area, \n
  ///           the application must execute shm_unlink to delete the shared memory area.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - sync only
  /// \~english @see CNSSharedMemWriter(const std::string&, const BOOL), CNSSharedMemWriter(), Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ~CNSSharedMemWriter();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///        This function opens and maps the shared memory object.
  /// \~english @param None
  /// \~english @retval eFrameworkunifiedStatusOK if shared memory opened successfully
  /// \~english @retval eFrameworkunifiedStatusNullPointer pointer of the CNSSharedMem object is NULL
  /// \~english @retval eFrameworkunifiedStatusFail unable to open shared memory
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - Open shared memory object
  /// \~english @par Conditions of processing failure
  ///           - pointer of the CNSSharedMem object is NULL.[eFrameworkunifiedStatusNullPointer]
  ///           - unable to open shared memory.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///            This function opens and maps the shared memory object.\n
  ///            It creates the shared memory if it does not exists.
  /// \~english @par Classification
  ///           - public
  /// \~english @par Type
  ///           - sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see Close
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Open();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///        This function is used to check whether the shared memory buffer is opened or not.
  /// \~english @retval TRUE - Open
  /// \~english @retval FALSE - Not open
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - None
  /// \~english @par Conditions of processing failure
  ///           - None
  /// \~english @par Detail
  ///           - Check whether the shared memory buffer is opened or not, and return the result.
  /// \~english @par Classification
  ///           - public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @see none
  ////////////////////////////////////////////////////////////////////////////////////////////////
  BOOL IsOpen();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///           This function unmaps the shared memory object.
  /// \~english @retval eFrameworkunifiedStatusOK if shared memory closed successfully
  /// \~english @retval eFrameworkunifiedStatusNullPointer pointer of the CNSSharedMem object is NULL
  /// \~english @retval eFrameworkunifiedStatusFail if shared memory is not open
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - Close the shared memory object
  /// \~english @par Conditions of processing failure
  ///           - Pointer of the CNSSharedMem object is NULL.[eFrameworkunifiedStatusNullPointer]
  ///           - If shared memory is not open.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///           - This function unmaps the shared memory object.\n
  ///             If pointer of the CNSSharedMem object is NULL, will return eFrameworkunifiedStatusNullPointer.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see Open
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus Close();

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///        This function writes the data into the shared memory.
  /// \~english @param[in] buffer
  ///        PSTR - pointer to the buffer containing the data to be written
  /// \~english @param[in] f_uilength
  ///        const UI_32 - length of the data buffer to be written
  /// \~english @retval The number of bytes written
  /// \~english @retval NS_SHM_ERROR if an error occurred
  /// \~english @par Preconditons
  ///        - Open the shared memory object
  /// \~english @par Change of internal status
  ///        - shared memory is changed.
  /// \~english @par Conditions of processing failure
  ///        - m_pShmWriter is NULL.
  ///        - buffer is NULL.
  ///        - f_uilength is 0.
  /// \~english @par Detail
  ///        - This function writes the buffer's data whose length is f_uilength to shared memory.\n
  ///          If successed,return write size,else return NS_SHM_ERROR.
  /// \~english @par Classification
  ///        - Public
  /// \~english @par Type
  ///         Sync only
  /// \~english @par
  ///           - l_iWriteSize:Result
  ///               - NS_SHM_ERROR:Failure
  ///               - Except NS_SHM_ERROR:Success
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  SI_32 Write(PCSTR buffer, const UI_32 f_uilength);

  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// \ingroup CNSSharedMemWriter
  /// \~english @par Brief
  ///           This function clears the shared memory buffer.
  /// \~english @retval eFrameworkunifiedStatusOK if shared memory cleared successfully
  /// \~english @retval eFrameworkunifiedStatusNullPointer pointer of the CNSSharedMem object is NULL
  /// \~english @retval eFrameworkunifiedStatusFail if shared memory is not open
  /// \~english @par Preconditons
  ///           - None
  /// \~english @par Change of internal status
  ///           - shared memory is changed.
  /// \~english @par Conditions of processing failure
  ///           - Pointer of the CNSSharedMem object is NULL.[eFrameworkunifiedStatusNullPointer]
  ///           - If shared memory is not open.[eFrameworkunifiedStatusFail]
  /// \~english @par Detail
  ///           - This function clears the shared memory buffer.\n
  ///             If pointer of the CNSSharedMem object is NULL, will return eFrameworkunifiedStatusNullPointer.
  /// \~english @par Classification
  ///           - Public
  /// \~english @par Type
  ///           - Sync only
  /// \~english @par
  ///           - eFrameworkunifiedStatus:Result
  ///               - eFrameworkunifiedStatusOK:Success
  ///               - Except eFrameworkunifiedStatusOK:Failure
  /// \~english @see None
  ////////////////////////////////////////////////////////////////////////////////////////////////
  EFrameworkunifiedStatus ClearBuf();

 protected:
  // no members in protected

 private:
  ////////////////////////////////////////////////////////////////////////////////////////////////
  /// CNSSharedMemWriter
  /// Constructor of CNSSharedMemWriter class
  ///
  /// \param
  ///
  /// \return     None
  ///
  ////////////////////////////////////////////////////////////////////////////////////////////////
  CNSSharedMemWriter();

  CNSSharedMem *m_pShmWriter;   // pointer to the CNSSharedMem object
};

#endif  /*__NATIVESERVICES_NATIVESERVICES_INC_NATIVE_NS_SHAREDMEM_WRITER_H__*/  // NOLINT  (build/header_guard)
/** @}*/
/** @}*/
/** @}*/
/** @}*/
//@}
