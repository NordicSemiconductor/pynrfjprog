#ifndef NRFDFU_H
#define NRFDFU_H

#include "DllCommonDefinitions.h"

typedef void prog_callback(const char * process);

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    IPCEVENT_NONE,
    IPCEVENT_FAULT,
    IPCEVENT_COMMAND,
    IPCEVENT_DATA
}nrfdfu_ipc_event_t;

typedef struct
{
    uint32_t id[10];
}ipc_dfu_id_t;

typedef struct
{
    union
    {
        uint8_t bytes[32];
        uint32_t words[8];
    };
}digest_buffer_t;

typedef void * connection_handle_t;

/**
* @brief   Initialize IPC DFU.
*
* @details Initializes connection and starts the DFU session with the device.
*
* @post    After the execution of this function, the initial settings for the DFU will be activated.
*
* @param   handle                              Pointer to connection handle to initialize
* @param   jlink_path_cstr                     Path to a jlink dll to use for the connection, or NULL. If NULL, dll will attempt to find a dll.
* @param   family                              Family of the target to connect to.
* @param   snr                                 Serial number of debugger to connect to.
* @param   swd_frequency                       Clock frequency to use for SWD connection.
* @param   callback                            Pointer to logger callback function to use, or NULL.
* @param   param                               Parameter to pass to callback function.
* @param   prog_cb                             Callback function that will indicate progress through an operation.
*
* @retval  SUCCESS
* @retval  OUT_OF_MEMORY                       Memory could not be alocated for the operation.
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The snr parameter is already initialized.
* @retval  JLINKARM_DLL_NOT_FOUND              JLinkarm dll could not be located.
* @retval  NRFJPROG_SUB_DLL_NOT_FOUND          Nrfjprog sub-dll could not be located.
* @retval  NRFJPROG_SUB_DLL_COULD_NOT_LOAD_FUNCTIONS          Could not load functions of nrfjprog sub-dll.
* @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
* @retval  TIME_OUT                            The operation timed out.
*/
nrfjprogdll_err_t NRFDFU_connect_to_ipc_target(connection_handle_t * handle,
                                               const char * jlink_path_cstr,
                                               device_family_t family,
                                               coprocessor_t coprocessor,
                                               uint32_t snr,
                                               uint32_t swd_frequency,
                                               msg_callback_ex * callback,
                                               void * param,
                                               prog_callback * prog_cb);

/**
 * @brief   Initialize MCUBoot DFU connection.
 *
 * @details Initializes connection and starts the DFU session with the device.
 *
 * @post    After the execution of this function, the initial settings for the DFU will be activated.
*
* @param   handle                              Pointer to connection handle to initialize
* @param   serial_port                         Serial port to connect to.
* @param   baud_rate                           Baud rate of serial connection.
* @param   response_timeout                    Time in ms before serial port should time out while waiting for a response from target.
* @param   callback                            Pointer to logger callback function to use, or NULL.
* @param   param                               Parameter to pass to callback function.
* @param   prog_cb                             Callback function that will indicate progress through an operation.
 *
 * @retval  SUCCESS
 * @retval  OUT_OF_MEMORY                       Memory could not be allocated for the operation.
 * @retval  INVALID_PARAMETER                   The handle parameter is NULL.
 * @retval                                      The serial_port parameter is already initialized.
 * @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
 * @retval  TIME_OUT                            The operation timed out.
 */
nrfjprogdll_err_t NRFDFU_connect_to_mcuboot_target(connection_handle_t * handle,
                                                   const char * serial_port,
                                                   const uint32_t baud_rate,
                                                   const uint32_t response_timeout,
                                                   msg_callback_ex * callback,
                                                   void * param,
                                                   prog_callback * prog_cb);

/**
 * @brief   Initialize Modem UART DFU connection.
 *
 * @details Initializes connection and starts the DFU session with the device.
 *
 * @post    After the execution of this function, the initial settings for the DFU will be activated.
*
* @param   handle                              Pointer to connection handle to initialize
* @param   serial_port                         Serial port to connect to.
* @param   baud_rate                           Baud rate of serial connection.
* @param   response_timeout                    Time in ms before serial port should time out while waiting for a response from target.
* @param   callback                            Pointer to logger callback function to use, or NULL.
* @param   param                               Parameter to pass to callback function.
* @param   prog_cb                             Callback function that will indicate progress through an operation.
 *
 * @retval  SUCCESS
 * @retval  OUT_OF_MEMORY                       Memory could not be allocated for the operation.
 * @retval  INVALID_PARAMETER                   The handle parameter is NULL.
 * @retval                                      The serial_port parameter is already initialized.
 * @retval  SERIAL_PORT_RESOURCE_ERROR          Unable to open serial port.
 * @retval  TIME_OUT                            The operation timed out.
 */
nrfjprogdll_err_t NRFDFU_connect_to_modemdfu_target(connection_handle_t * handle,
                                                   const char * serial_port,
                                                   const uint32_t baud_rate,
                                                   const uint32_t response_timeout,
                                                   msg_callback_ex * callback,
                                                   void * param,
                                                   prog_callback * prog_cb);

/**
* @brief   Closes the connection to the underlying DLLs.
*
* @details Closes the connection to the underlying DLLs.
*
* @pre     handle must point to a handle that was previously initialized with any function NRFDFU_connect_to_*_target().
*
* @param   handle                              An initialized connection handle
*
* @post    After the execution of this function, connection will be closed and 
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
*/
nrfjprogdll_err_t NRFDFU_close_connection(connection_handle_t * handle);

/**
* @brief   Programs the provided zip file
*
* @details Initializes connection and starts the DFU session with the device.
*
* @pre     handle must be initialized with any function NRFDFU_connect_to_*_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   package_path                        Path to zip file containing a DFU package compatible with the current DFU connection.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The package_path parameter is NULL.
* @retval                                      The package_path points to a non existing file.
* @retval                                      The package_path points to a file without reading rights.
* @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
*/
nrfjprogdll_err_t NRFDFU_program_package(connection_handle_t handle, const char * package_path);

/**
* @brief   Programs using the list of files provided
*
* @details Programs the files provided as if they were part of a zip file passed to program_package
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_*_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   paths                               List of zero-terminated C strings containing file paths
* @param   num_files                           The number of file paths in paths.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The paths points to a non existing file.
* @retval                                      The paths points to a file without reading rights.
* @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
*/
nrfjprogdll_err_t NRFDFU_program_files(connection_handle_t handle, const char ** paths, uint32_t num_files);

/**
* @brief   Verifies target firmware.
*
* @details Verifies the firmware on the target and checks if the firmware digest is correct.
*
* @pre     handle must be initialized with any function NRFDFU_connect_to_*_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   package_path                        Path to package of files to be verified.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The package_path parameter is NULL.
* @retval                                      The package_path points to a non existing file.
* @retval                                      The package_path points to a file without reading rights.
* @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
*/
nrfjprogdll_err_t NRFDFU_verify_package(connection_handle_t handle, const char * package_path);

/**
* @brief   Updates, and verifies target.
*
* @details Updates the target firmware with the hex file provided. Then verifies the update and checks if the firmware digest is correct.
*
* @pre     handle must be initialized with any function NRFDFU_connect_to_*_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   paths                               List of zero-terminated C strings containing file paths
* @param   num_files                           The number of file paths in paths.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The paths points to a non existing file.
* @retval                                      The paths points to a file without reading rights.
* @retval  FILE_OPERATION_FAILED               Failed to make temporary files.
*/
nrfjprogdll_err_t NRFDFU_verify_files(connection_handle_t handle, const char ** paths, uint32_t num_files);

/**
* @brief   Read part of the target firmware to a buffer.
* @details Reads length bytes from address into buffer.
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_ipc_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   address                             The start address of the read.
* @param   buffer                              A path to where the output is written.
* @param   length                              length of reading opperation.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The address parameter is not page aligned.
* @retval                                      The length parameter is not larger than 0.
* @retval                                      The length parameter is not a multiple of 4.
*/
nrfjprogdll_err_t NRFDFU_read(connection_handle_t handle, uint32_t address, uint8_t * buffer, uint32_t length);

/**
* @brief   Checks if IPC event "event" has been triggered.
*
* @details Reads the event status register corresponding to "event", and checks if an event has occurred.
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_ipc_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   event                               Which event to check
* @param   status                              Pointer where result of check should be stored
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The handle parameter is not a valid IPC connection.
*/
nrfjprogdll_err_t NRFDFU_IPC_get_event_status(connection_handle_t handle, nrfdfu_ipc_event_t event, bool * status);

/**
* @brief   Resets event register.
*
* @details Resets event registers to 0.
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_ipc_target() before this function is run.
*
* @post    After the execution of this function, the event register corresponding to "event" is set to 0.
*
* @param   handle                              An initialized connection handle.
* @param   event                               What event to acknowledge.
*
* @retval  SUCCESS
* @retval  DFU_ERROR                           An error in the DFU process.
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The handle parameter is not a valid IPC connection.
* @retval                                      The event parameter is not a valid nrfdfu_ipc_event_t.
*/
nrfjprogdll_err_t NRFDFU_IPC_acknowledge_event(connection_handle_t handle, nrfdfu_ipc_event_t event);

/**
* @brief   Reads IPC target ID.
*
* @details Reads the ID from the IPC target.
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_ipc_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   id                                  A pointer to an ipc id struct where the id will be stored.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The handle parameter is not a valid IPC connection.
* @retval                                      The id parameter is NULL.
*/
nrfjprogdll_err_t NRFDFU_IPC_read_id(connection_handle_t handle, ipc_dfu_id_t * id);

/**
* @brief   Reads the IPC target digest.
*
* @details Reads the IPC target digest, and stores the value in the provided digest buffer.
*
* @pre     handle must be initialized with the function NRFDFU_connect_to_ipc_target() before this function is run.
*
* @param   handle                              An initialized connection handle
* @param   id                                  A pointer to an IPC id struct where the digest will be stored.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The handle parameter is NULL.
* @retval                                      The handle parameter is not a valid IPC connection.
*/
nrfjprogdll_err_t NRFDFU_IPC_read_digest(connection_handle_t handle, digest_buffer_t * digest);

#if defined(__cplusplus)
}
#endif

#endif /* NRFDFU_H */

