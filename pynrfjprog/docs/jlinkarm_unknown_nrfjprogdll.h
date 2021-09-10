/*
 * Copyright (c) 2010 - 2018, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JLINKARM_UNKNOWN_NRFJPROGDLL_H
#define JLINKARM_UNKNOWN_NRFJPROGDLL_H


#include <stdint.h>
#include "DllCommonDefinitions.h"


#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief   Returns the JLinkARM DLL version.
 *
 * @details Returns the JLinkARM DLL version that has been opened in with the NRFJPROG_open_dll() function.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   major                               Pointer for storing of dll major version.
 * @param   minor                               Pointer for storing of dll minor version.
 * @param   revision                            Pointer for storing of dll revision.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The major parameter is NULL.
 *                                              The minor parameter is NULL.
 *                                              The revision parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_dll_version_inst(nrfjprog_inst_t instance, uint32_t * major, uint32_t * minor, char * revision);
nrfjprogdll_err_t NRFJPROG_dll_version(uint32_t * major, uint32_t * minor, char * revision);

/**
 * @brief   Returns the path to the JLinkARM shared library.
 *
 * @details Returns the path to the JLinkARM DLL that has been opened with the NRFJPROG_open_dll() function.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll()
 *          function.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   buffer                              Buffer to store the path.
 * @param   buffer_size                         Size of buffer
 * @param   bytes_copied                        Pointer to variable that will hold the amount of bytes copied to buffer.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The major parameter is NULL.
 *                                              The minor parameter is NULL.
 *                                              The revision parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_get_jlink_path_inst(nrfjprog_inst_t instance,
                                               char * buffer,
                                               const size_t buffer_size,
                                               size_t * bytes_copied);
nrfjprogdll_err_t NRFJPROG_get_jlink_path(char * buffer, const size_t buffer_size, size_t * bytes_copied);


/**
 * @brief   Attempts to find and return the path to the newest JLinkARM shared library installation.
 *
 * @details Attempts to find the newest JLinkARM shared library installation. The path returned will be the same path found when auto-detecting
 *          JLinkARM DLL in function NRFJPROG_open_dll(). Note that NRFJPROG_find_jlink_path() always returns a system search result, and not which JLinkARM
 *          library is in use. See function NRFJPROG_get_jlink_path() for the latter.
 *
 *          Upon success, the path to the JLinkARM shared library is stored in 'buffer' as a 0-terminated UTF-8 string, and bytes_copied is updated accordingly.
 *          If buffer and buffer_size was passed as NULL and zero, bytes_copied is set to the total size needed to store the returned JLinkARM path.
 *          This can be used to find the buffer size needed. The size of 'bytes_copied' includes 0-terminator.
 *
 *          The search strategy chosen is dependent on platform. On Windows, Windows Registry is searched.
 *          On OSX the default install location "/Applications/SEGGER/JLink/" is scanned for shared library files.
 *          Similarly, on Linux, the default install location "/opt/SEGGER/JLink/" is checked.
 *          On unix-like systems the function might also return a library name compatible with dlopen if no library file is found in the default search path.
 *
 * @param   buffer                              Buffer to store the path. Can be NULL if buffer_size is zero.
 * @param   buffer_size                         Length of buffer.
 * @param   bytes_copied                        Pointer to variable that will hold the amount of bytes copied.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   buffer is NULL while buffer_size is non-zero.
 *                                              bytes_copied is NULL.
 * @retval  JLINKARM_DLL_NOT_FOUND              No search results yielded.
 */
nrfjprogdll_err_t NRFJPROG_find_jlink_path(char * buffer,
                                           uint32_t buffer_size,
                                           uint32_t * bytes_copied);

/**
 * @brief   Checks if the library is ready to execute commands.
 *
 * @details For NRFJPROG_is_dll_open_inst():
 *          Checks if the instance variable provided points to a valid and open nrfjprogdll instance.
 *
 *          For NRFJPROG_is_dll_open():
 *          NRFJPROG_open_dll() function has been called since the last call to NRFJPROG_close_dll().
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   opened                              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called. or instance has been closed.
 * @retval  INVALID_PARAMETER                   The opened parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_dll_open_inst(nrfjprog_inst_t instance, bool * opened);
nrfjprogdll_err_t NRFJPROG_is_dll_open(bool * opened);

/**
 * @brief   Opens the JLinkARM DLL and sets the log callback. Prepares the dll for work with an nRF53 device.
 *
 * @details This function opens the JLinkARM DLL using the received path. The path should include the name of the DLL
 *          itself (i.e. "JLinkARM.dll"). Only JLinkARM DLLs whose versions are greater than a minimum version will be
 *          accepted. The minimum version for the JLinkARM DLL is defined in MIN_JLINK_MAJOR_VERSION and
 *          MIN_JLINK_MINOR_VERSION defines. The log callback may be NULL. In that case no logging mechanism is
 *          provided. The msg_callback typedef is defined in DllCommonDefinitions.h. To close the dll, see NRFJPROG_close()
 *          function.
 *
 * @pre     Before the execution of this function, the dll must not be open. To close the dll, see NRFJPROG_close_dll()
 *          function.
 *
 * @post    After the execution of this function, the JLINKARM DLL pointers will be loaded and some memory reserved. To
 *          unload the pointers and free the memory, see NRFJPROG_close_dll() function.
 * @post    After the execution of this function, the instance variable pointed to by instance_ptr will refer to the
 *          opened nrfjprog session.
 *
 * @param   instance_ptr                        Pointer to variable that will store the working instance of nrfjprog.
 * @param   jlink_path                          Path to the JLinkARM DLL. Does not support unicode paths. If NULL or
 *                                              nullptr, nrfjprog will attempt to find the newest installed J-Link Dll.
 * @param   log_cb                              Callback for reporting informational and error messages. May be NULL.
 * @param   log_param                           User-selectable value that will be passed back when calling log_cb in the _ex version of this function.
 *                                              log_param will not be dereferenced. May be NULL.
 * @param   family                              Defines the device family the next commands are going to be called to.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance_ptr is a NULL pointer.
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has already been called.
 * @retval  INVALID_PARAMETER                   The provided device family is not supported by this DLL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL, or the automatic search
 *                                              failed.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_open_dll_inst(nrfjprog_inst_t * instance_ptr,
                                         const char * jlink_path,
                                         msg_callback_ex * log_cb,
                                         void * callback_param,
                                         device_family_t family);
nrfjprogdll_err_t NRFJPROG_open_dll(const char * jlink_path, msg_callback * cb, device_family_t family);

/**
 * @brief   Closes and frees the JLinkARM DLL.
 *
 * @details Closes and frees the JLinkARM DLL. This function needs to be called before exiting if NRFJPROG_open_dll()
 *          has been called. After the execution of this function, the device CPU will not change its state from running or
 *          halted.
 *
 * @post    After the execution of this function, the JLINKARM DLL function pointers will be unloaded and the reserved
 *          memory freed. To open the dll, see NRFJPROG_open_dll() function.
 * @post    After the execution of this function, the device will not be in debug interface mode. To enter debug
 *          interface mode, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an
 *          emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @post    After the execution of this function, the emulator will be disconnected from the device. To connect to the
 *          device, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the trace and debug resources will be switched off. To switch on the
 *          debug resources, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the instance pointer will be cleared, and the memory associated with
 *          the instace will be freed.
 *
 * @param   instance_ptr                        Pointer to variable that will be closed.
 */
void NRFJPROG_close_dll_inst(nrfjprog_inst_t * instance_ptr);
void NRFJPROG_close_dll(void);


/**
 * @brief   Enumerates all comports connected to a given Segger debug probe
 *
 * @details This function finds all com ports hosted by a given debug probe.
 *          The number of com ports found is written into the num_com_ports parameter. It also copies
 *          up to com_ports_len com_port_info_t objects into the com_ports array parameter.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   serial_number                       Serial number of the debug probe to find the com port of.
 * @param   com_ports                           Array in which to store the enumerated com ports.
 * @param   com_ports_len                       Number of com_port_info_t values that can be stored in the
 *                                              com_ports array.
 * @param   num_com_ports                       The number of com ports that were discovered.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INTERNAL_ERROR                      An internal error has occured.
 * @retval  INVALID_PARAMETER                   The com_ports parameter is NULL.
 *                                              The com_ports_len parameter is 0.
 *                                              The num_available parameter is NULL.
**/
nrfjprogdll_err_t NRFJPROG_enum_emu_com_inst(nrfjprog_inst_t instance, uint32_t serial_number, com_port_info_t com_ports[], uint32_t com_ports_len, uint32_t * num_com_ports);
nrfjprogdll_err_t NRFJPROG_enum_emu_com(uint32_t serial_number, com_port_info_t com_ports[], uint32_t com_ports_len, uint32_t * num_com_ports);


/**
 * @brief   Enumerates the serial numbers of connected USB J-Link emulators.
 *
 * @details This function asks the JLinkARM DLL how many USB J-Link emulators are connected to
 *          the PC, and writes that value into the num_available parameter. It also copies
 *          up to serial_numbers_len serial numbers into the serial_numbers array parameter.
 *          The function can be called with serial_numbers set to NULL and serial_numbers_len
 *          set to zero to obtain the number of connected emulators.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   serial_numbers                      Array in which to store the enumerated serial numbers.
 * @param   serial_numbers_len                  Number of uint32_t values that can be stored in the
 *                                              serial_numbers array (may be zero).
 * @param   num_available                       The number of serial numbers that were enumerated.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 * @retval  INVALID_PARAMETER                   The serial_numbers parameter is NULL but serial_numbers_len is > 0.
 *                                              The num_available parameter is NULL.
 * @retval  OUT_OF_MEMORY                       Memory could not be allocated for the operation.
 */
nrfjprogdll_err_t NRFJPROG_enum_emu_snr_inst(nrfjprog_inst_t instance, uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);
nrfjprogdll_err_t NRFJPROG_enum_emu_snr(uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);


/**
 * @brief   Checks if the emulator has an established connection with Segger emulator/debugger.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   is_pc_connected_to_emu              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The is_connected_to_emu pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_emu_inst(nrfjprog_inst_t instance, bool * is_pc_connected_to_emu);
nrfjprogdll_err_t NRFJPROG_is_connected_to_emu(bool * is_pc_connected_to_emu);


/**
 * @brief   Connects to a given emulator/debugger.
 *
 * @details This function connects to serial_number emulator and sets the SWD communication speed at clock_speed_in_khz.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must not be established. To disconnect from an emulator, see NRFJPROG_disconnect_from_emu() function.
 * @pre     Before the execution of this function, the emulator must be physically connected to a powered board.
 *
 * @post    After the execution of this function, the PC will be connected to an emulator. To disconnect to the emulator, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   serial_number                       Serial number of the emulator to connect to.
 * @param   clock_speed_in_khz                  Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed
 *                                              will be used.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has already been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 * @retval  LOW_VOLTAGE                         Low voltage was detected at the target device.
 * @retval  INVALID_PARAMETER                   The clock_speed_in_khz parameter is not within limits.
 * @retval  EMULATOR_NOT_CONNECTED              The serial_number emulator is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_with_snr_inst(nrfjprog_inst_t instance, uint32_t serial_number, uint32_t clock_speed_in_khz);
nrfjprogdll_err_t NRFJPROG_connect_to_emu_with_snr(uint32_t serial_number, uint32_t clock_speed_in_khz);


/**
 * @brief   Connects to an emulator/debugger.
 *
 * @details This function connects to an available emulator and sets the SWD communication speed at clock_speed_in_khz.
 *          If more than one emulator is available, a pop-up window will appear to make a selection.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must not be established. To disconnect from an emulator, see NRFJPROG_disconnect_from_emu() function.
 * @pre     Before the execution of this function, the emulator must be physically connected to a powered board.
 *
 * @post    After the execution of this function, the PC will be connected to an emulator. To disconnect to the emulator, see NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   clock_speed_in_khz                  Speed for the SWD communication. It must be between JLINKARM_SWD_MIN_SPEED_KHZ
 *                                              and JLINKARM_SWD_MAX_SPEED_KHZ defined in DllCommonDefinitions.h. If the emulator
 *                                              does not support the input clock_speed, the emulators maximum supported speed
 *                                              will be used.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has already been called.
 * @retval  NO_EMULATOR_CONNECTED               There is no emulator connected to the PC.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 * @retval  LOW_VOLTAGE                         Low voltage was detected at the target device.
 * @retval  INVALID_PARAMETER                   The clock_speed_in_khz parameter is not within limits.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_without_snr_inst(nrfjprog_inst_t instance, uint32_t clock_speed_in_khz);
nrfjprogdll_err_t NRFJPROG_connect_to_emu_without_snr(uint32_t clock_speed_in_khz);


/**
 * @brief   Attempts to reset the connected J-Link OB.
 *
 * @details Resets and reconnects to the J-Link OB.
            This operation is only available in debug probes of type J-Link OB-SAM3U128-V2-NordicSemi.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection,
 *          see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the PC will still be connected to the emulator.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has not been called.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The connected debug probe does not support the ResetJLink command.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error, check log for details.
 * @retval  EMULATOR_NOT_CONNECTED              The emulator did not successfully reenumerate within 10s after the reset.
 */
nrfjprogdll_err_t NRFJPROG_reset_connected_emu_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_reset_connected_emu(void);


/**
 * @brief   Replaces the firmware on the connected J-Link debug probe.
 *
 * @details Replaces the firmware on the selected debug probe.
 *          The debug probe firmware is replaced with the fw version that shipped with the J-Link DLL selected in open_dll()
 *          even if a newer version is already present.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection,
 *          see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the debug probe will have been reset.
 * @post    After the execution of this function, the PC will still be connected to the emulator.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error, check log for details.
 * @retval  EMULATOR_NOT_CONNECTED              The emulator did not successfully reenumerate within 10s after the reset.
 */
nrfjprogdll_err_t NRFJPROG_replace_connected_emu_fw_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_replace_connected_emu_fw(void);


/**
 * @brief   Reads the serial number of the emulator connected to.
 *
 * @details Reads the serial number of the emulator connected to.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   serial_number                       Pointer to variable that will contain the snr.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The serial_number pointer is NULL.
 */
nrfjprogdll_err_t NRFJPROG_read_connected_emu_snr_inst(nrfjprog_inst_t instance, uint32_t * serial_number);
nrfjprogdll_err_t NRFJPROG_read_connected_emu_snr(uint32_t * serial_number);


/**
 * @brief   Reads the firmware identification string of the emulator connected to.
 *
 * @details This function reads the firmware identification string of the emulator connected to into the
 *          given buffer. The function will read a maximum of buffer_size-1 characters into the buffer, and 0-terminate it. 
 *          Any excess characters are not read.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   buffer                              Pointer to buffer to contain the firmware string.
 * @param   buffer_size                         Size of the buffer. The user is responsible for ensuring a big enough buffer. A 255 byte buffer is suggested.
 *                                              Maximum buffer_size value is INT_MAX (2147483647).
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   NRFJPROG_open_dll() function has not been called.
 *                                              NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() has not been called.
 * @retval  INVALID_PARAMETER                   The character buffer pointer is a NULL-pointer.
 */
nrfjprogdll_err_t NRFJPROG_read_connected_emu_fwstr_inst(nrfjprog_inst_t instance, char * buffer, uint32_t buffer_size);
nrfjprogdll_err_t NRFJPROG_read_connected_emu_fwstr(char * buffer, uint32_t buffer_size);


/**
 * @brief   Disconnects from an emulator.
 *
 * @details This function disconnects from a connected emulator. This also disconnects from a connected device if connected. Will
 *          not fail if we have never connected to an emulator. After the execution of this function, the device CPU will not change
 *          its state from running or halted.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @post    After the execution of this function, the device will not be in debug interface mode. To enter debug interface mode, see NRFJPROG_connect_to_device() function.
 * @post    After the execution of this function, the PC will be disconnected from an emulator. To connect to an emulator, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu(void);

/**
 * @brief   Selects a new family
 *
 * @details This function switches to the dll api that belongs to the family passed in. J-Link state is preserved.
 *          Its primary use is for switching from UNKNOWN family to a known family, but the function also supports other
 *          switches. 
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 *
 * @post    After the execution of this function, the api will call the functions belonging to the selected family dll.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   family                              New family to interact with
 */
nrfjprogdll_err_t NRFJPROG_select_family_inst(nrfjprog_inst_t instance, device_family_t family);
nrfjprogdll_err_t NRFJPROG_select_family(device_family_t family);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_coprocessor_enabled_inst(nrfjprog_inst_t instance, coprocessor_t coprocessor, bool * is_coprocessor_enabled);
nrfjprogdll_err_t NRFJPROG_is_coprocessor_enabled(coprocessor_t coprocessor, bool * is_coprocessor_enabled);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_enable_coprocessor_inst(nrfjprog_inst_t instance, coprocessor_t coprocessor);
nrfjprogdll_err_t NRFJPROG_enable_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_disable_coprocessor_inst(nrfjprog_inst_t instance, coprocessor_t coprocessor);
nrfjprogdll_err_t NRFJPROG_disable_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_select_coprocessor_inst(nrfjprog_inst_t instance, coprocessor_t coprocessor);
nrfjprogdll_err_t NRFJPROG_select_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_recover_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_recover(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_device_inst(nrfjprog_inst_t instance, bool * is_emu_connected_to_device);
nrfjprogdll_err_t NRFJPROG_is_connected_to_device(bool * is_emu_connected_to_device);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_device_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_connect_to_device(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_device_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_disconnect_from_device(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect_inst(nrfjprog_inst_t instance, readback_protection_status_t desired_protection);
nrfjprogdll_err_t NRFJPROG_readback_protect(readback_protection_status_t desired_protection);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_readback_status_inst(nrfjprog_inst_t instance, readback_protection_status_t * status);
nrfjprogdll_err_t NRFJPROG_readback_status(readback_protection_status_t * status);

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_eraseprotect_enabled_inst(nrfjprog_inst_t instance, bool * status);
nrfjprogdll_err_t NRFJPROG_is_eraseprotect_enabled(bool * status);

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_enable_eraseprotect_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_enable_eraseprotect();

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source_inst(nrfjprog_inst_t instance, uint32_t * size, region_0_source_t * source);
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source(uint32_t * size, region_0_source_t * source);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_debug_reset_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_debug_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_sys_reset_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_sys_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_pin_reset_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_pin_reset(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_disable_bprot_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_disable_bprot(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_bprot_enabled_inst(nrfjprog_inst_t instance, bool * bprot_enabled, uint32_t address_start, uint32_t length);
nrfjprogdll_err_t NRFJPROG_is_bprot_enabled(bool * bprot_enabled, uint32_t address_start, uint32_t length);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_all_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_erase_all(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_page_inst(nrfjprog_inst_t instance, uint32_t addr);
nrfjprogdll_err_t NRFJPROG_erase_page(uint32_t addr);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_uicr_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_erase_uicr(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write_u32_inst(nrfjprog_inst_t instance, uint32_t addr, uint32_t data, bool nvmc_control);
nrfjprogdll_err_t NRFJPROG_write_u32(uint32_t addr, uint32_t data, bool nvmc_control);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_u32_inst(nrfjprog_inst_t instance, uint32_t addr, uint32_t * data);
nrfjprogdll_err_t NRFJPROG_read_u32(uint32_t addr, uint32_t * data);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write_inst(nrfjprog_inst_t instance, uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);
nrfjprogdll_err_t NRFJPROG_write(uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_inst(nrfjprog_inst_t instance, uint32_t addr, uint8_t * data, uint32_t data_len);
nrfjprogdll_err_t NRFJPROG_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_halted_inst(nrfjprog_inst_t instance, bool * is_device_halted);
nrfjprogdll_err_t NRFJPROG_is_halted(bool * is_device_halted);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_halt_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_halt(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_run_inst(nrfjprog_inst_t instance, uint32_t pc, uint32_t sp);
nrfjprogdll_err_t NRFJPROG_run(uint32_t pc, uint32_t sp);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_go_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_go(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_step_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_step(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_count_inst(nrfjprog_inst_t instance, uint32_t * ram_sections_count);
nrfjprogdll_err_t NRFJPROG_read_ram_sections_count(uint32_t * ram_sections_count);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_size_inst(nrfjprog_inst_t instance, uint32_t * ram_sections_size, uint32_t ram_sections_size_len);
nrfjprogdll_err_t NRFJPROG_read_ram_sections_size(uint32_t * ram_sections_size, uint32_t ram_sections_size_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_power_status_inst(nrfjprog_inst_t instance, ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_len);
nrfjprogdll_err_t NRFJPROG_read_ram_sections_power_status(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_power_ram_all_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_power_ram_all(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_unpower_ram_section_inst(nrfjprog_inst_t instance, uint32_t section_index);
nrfjprogdll_err_t NRFJPROG_unpower_ram_section(uint32_t section_index);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_memory_descriptors(memory_description_t memories[], uint32_t memories_len, uint32_t * num_memories_available);
nrfjprogdll_err_t NRFJPROG_read_memory_descriptors_inst(nrfjprog_inst_t instance, memory_description_t memories[], uint32_t memories_len, uint32_t * num_memories_available);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_page_sizes(memory_description_t * memory_description, page_repetitions_t page_reps[], uint32_t page_reps_len, uint32_t * num_page_reps_available);
nrfjprogdll_err_t NRFJPROG_read_page_sizes_inst(nrfjprog_inst_t instance, memory_description_t * memory_description, page_repetitions_t page_reps[], uint32_t page_reps_len, uint32_t * num_page_reps_available);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_cpu_register(cpu_registers_t register_name, uint32_t * register_value);
nrfjprogdll_err_t NRFJPROG_read_cpu_register_inst(nrfjprog_inst_t instance, cpu_registers_t register_name, uint32_t * register_value);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_write_cpu_register_inst(nrfjprog_inst_t instance, cpu_registers_t register_name, uint32_t register_value);
nrfjprogdll_err_t NRFJPROG_write_cpu_register(cpu_registers_t register_name, uint32_t register_value);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_device_version_inst(nrfjprog_inst_t instance, device_version_t * version);
nrfjprogdll_err_t NRFJPROG_read_device_version(device_version_t * version);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_device_info_inst(nrfjprog_inst_t instance, device_version_t * version, device_name_t * name, device_memory_t * memory, device_revision_t * revision);
nrfjprogdll_err_t NRFJPROG_read_device_info(device_version_t * version, device_name_t * name, device_memory_t * memory, device_revision_t * revision);


/**
 * @brief   Reads the family of the device connected to the emulator.
 *
 * @details Reads the family of the device connected to the emulator by the use of NRFJPROG_read_access_port_register() function.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see the appropriate family header file for functions that can take the device out of debug interface mode.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   family                              Pointer of the location to store the device family.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The family parameter is null.
 * @retval  UNKNOWN_DEVICE                      Family identification failed.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out. 
 */
nrfjprogdll_err_t NRFJPROG_read_device_family_inst(nrfjprog_inst_t instance, device_family_t * family);
nrfjprogdll_err_t NRFJPROG_read_device_family(device_family_t * family);


/**
 * @brief   Reads a debug port register.
 *
 * @details Reads into data pointer a debug port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 */
nrfjprogdll_err_t NRFJPROG_read_debug_port_register_inst(nrfjprog_inst_t instance, uint8_t reg_addr, uint32_t * data);
nrfjprogdll_err_t NRFJPROG_read_debug_port_register(uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debug port register.
 *
 * @details Writes data parameter into a debug port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 */
nrfjprogdll_err_t NRFJPROG_write_debug_port_register_inst(nrfjprog_inst_t instance, uint8_t reg_addr, uint32_t data);
nrfjprogdll_err_t NRFJPROG_write_debug_port_register(uint8_t reg_addr, uint32_t data);


/**
 * @brief   Reads a debugger access port register.
 *
 * @details Reads into data pointer a debugger access port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   ap_index                            Access port index for read if ap_access.
 * @param   reg_addr                            Register address to read, either in debug port or access port.
 * @param   data                                Pointer of the location to store the value read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 */
nrfjprogdll_err_t NRFJPROG_read_access_port_register_inst(nrfjprog_inst_t instance, uint8_t ap_index, uint8_t reg_addr, uint32_t * data);
nrfjprogdll_err_t NRFJPROG_read_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debugger access port register.
 *
 * @details Writes data parameter into a debugger access port register.
 *
 * @pre     Before the execution of this function, the dll must be open. To open the dll, see NRFJPROG_open_dll() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 *
 * @param   instance                            A handle to an open nrfjprog instance.
 * @param   ap_index                            Access port index for write if ap_access.
 * @param   reg_addr                            Register address to write, either in debug port or access port.
 * @param   data                                Data to write into the register.
 *
 * @retval  SUCCESS
 * @retval  INVALID_SESSION                     Instance is not a valid nrfjprog instance, or NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_OPERATION                   The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The register address is not 32-bit aligned.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  JLINKARM_DLL_TIMEOUT_ERROR          Communication with the J-Link probe timed out.
 */
nrfjprogdll_err_t NRFJPROG_write_access_port_register_inst(nrfjprog_inst_t instance, uint8_t ap_index, uint8_t reg_addr, uint32_t data);
nrfjprogdll_err_t NRFJPROG_write_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t data);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_rtt_started_inst(nrfjprog_inst_t instance, bool * started);
nrfjprogdll_err_t NRFJPROG_is_rtt_started(bool * started);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address_inst(nrfjprog_inst_t instance, uint32_t address);
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(uint32_t address);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_start_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_rtt_start(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found_inst(nrfjprog_inst_t instance, bool * is_control_block_found);
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(bool * is_control_block_found);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_rtt_stop(void);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_inst(nrfjprog_inst_t instance, uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);
nrfjprogdll_err_t NRFJPROG_rtt_read(uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_write_inst(nrfjprog_inst_t instance, uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);
nrfjprogdll_err_t NRFJPROG_rtt_write(uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count_inst(nrfjprog_inst_t instance, uint32_t * down_channel_number, uint32_t * up_channel_number);
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info_inst(nrfjprog_inst_t instance, uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_is_qspi_init_inst(nrfjprog_inst_t instance, bool * initialized);
nrfjprogdll_err_t NRFJPROG_is_qspi_init(bool * initialized);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION           This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_init_inst(nrfjprog_inst_t instance, bool retain_ram, const qspi_init_params_t * init_params);
nrfjprogdll_err_t NRFJPROG_qspi_init(bool retain_ram, const qspi_init_params_t * init_params);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_init_ini_inst(nrfjprog_inst_t instance, bool retain_ram, const char * ini_file_path);
nrfjprogdll_err_t NRFJPROG_qspi_init_ini(bool retain_ram, const char * qspi_ini_path);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_start_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_qspi_start();


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_configure_inst(nrfjprog_inst_t instance, bool retain_ram, const qspi_init_params_t * init_params);
nrfjprogdll_err_t NRFJPROG_qspi_configure(bool retain_ram, const qspi_init_params_t * init_params);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_configure_ini_inst(nrfjprog_inst_t instance, const char * qspi_ini_path);
nrfjprogdll_err_t NRFJPROG_qspi_configure_ini(const char * qspi_ini_path);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_uninit_inst(nrfjprog_inst_t instance);
nrfjprogdll_err_t NRFJPROG_qspi_uninit(void);

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_set_rx_delay_inst(nrfjprog_inst_t instance, uint8_t rx_delay);
nrfjprogdll_err_t NRFJPROG_qspi_set_rx_delay(uint8_t rx_delay);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_set_size_inst(nrfjprog_inst_t instance, uint32_t qspi_size);
nrfjprogdll_err_t NRFJPROG_qspi_set_size(uint32_t qspi_size);

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_DEVICE_FOR_OPERATION        The version of the connected device does not support this operation.
 */
nrfjprogdll_err_t NRFJPROG_qspi_get_size_inst(nrfjprog_inst_t instance, uint32_t * qspi_size);
nrfjprogdll_err_t NRFJPROG_qspi_get_size(uint32_t * qspi_size);

/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_read_inst(nrfjprog_inst_t instance, uint32_t addr, uint8_t * data, uint32_t data_len);
nrfjprogdll_err_t NRFJPROG_qspi_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_write_inst(nrfjprog_inst_t instance, uint32_t addr, const uint8_t * data, uint32_t data_len);
nrfjprogdll_err_t NRFJPROG_qspi_write(uint32_t addr, const uint8_t * data, uint32_t data_len);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_erase_inst(nrfjprog_inst_t instance, uint32_t addr, qspi_erase_len_t length);
nrfjprogdll_err_t NRFJPROG_qspi_erase(uint32_t addr, qspi_erase_len_t length);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_qspi_custom_inst(nrfjprog_inst_t instance,
                                          uint8_t instruction_code,
                                          uint32_t instruction_length,
                                          const uint8_t * data_in,
                                          uint8_t * data_out);
nrfjprogdll_err_t NRFJPROG_qspi_custom(uint8_t instruction_code,
                                       uint32_t instruction_length,
                                       const uint8_t * data_in,
                                       uint8_t * data_out);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_read_to_file_inst(nrfjprog_inst_t instance, const char * file_path, read_options_t read_options);
nrfjprogdll_err_t NRFJPROG_read_to_file(const char * file_path, read_options_t read_options);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_program_file_inst(nrfjprog_inst_t instance, const char * file_path);
nrfjprogdll_err_t NRFJPROG_program_file(const char * file_path);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_verify_file_inst(nrfjprog_inst_t instance, const char * file_path, verify_action_t verify_action);
nrfjprogdll_err_t NRFJPROG_verify_file(const char * file_path, verify_action_t verify_action);


/**
 * @brief   Operation not available without a known family.
 *
 * @retval  INVALID_OPERATION                   This function cannot be executed without a known family.
 */
nrfjprogdll_err_t NRFJPROG_erase_file_inst(nrfjprog_inst_t instance, const char * file_path, erase_action_t chip_erase_mode, erase_action_t qspi_erase_mode);
nrfjprogdll_err_t NRFJPROG_erase_file(const char * file_path, erase_action_t chip_erase_mode, erase_action_t qspi_erase_mode);


#if defined(__cplusplus)
}
#endif

#endif  /* JLINKARM_UNKNOWN_NRFJPROGDLL_H */
