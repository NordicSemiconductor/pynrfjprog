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

#ifndef highlevelnrfjprogdll_FUNC_H
#define highlevelnrfjprogdll_FUNC_H

#include <stdint.h>

#include "DllCommonDefinitions.h"


#define major_version (10) 
#define minor_version (10) 
#define micro_version (0) 

#define FIRMWARE_STRING_LENGTH  NRFJPROG_STRING_LENGTH

#if defined(__cplusplus)


extern "C" {
#endif

/* Log function prototypes for logging and progress reporting operations. */

/* Provides short strings that describe the currently performed action. */
typedef void progress_callback(const char * process);

/* Probe handle type definition. */
typedef void * Probe_handle_t;

typedef enum {
    RESET_NONE     = 0,
    RESET_SYSTEM   = 1,
    RESET_DEBUG    = 2,
    RESET_PIN      = 3
} reset_action_t;

typedef enum {
    ERASE_NONE                 = 0,      /* Do nothing. */
    ERASE_ALL                  = 1,      /* Erase whole chip. */
    ERASE_PAGES                = 2,      /* Erase specified sectors, excluding UICR. */
    ERASE_PAGES_INCLUDING_UICR = 3       /* Erase specified sectors, with UICR support. */
} erase_action_t;

typedef enum {
    VERIFY_NONE = 0,      /* Do nothing. */
    VERIFY_READ = 1,      /* Verify by reading back contents. */
    VERIFY_HASH = 2,      /* Verify by hashing contents, faster than VERIFY_READ. */
} verify_action_t;

typedef struct {
    verify_action_t verify;           /* Select post-program Verify action. */
    erase_action_t chip_erase_mode;   /* Select pre-program erase mode for internal flash memories. */
    erase_action_t qspi_erase_mode;   /* Select pre-program erase mode for external QSPI memories. */
    reset_action_t reset;             /* Select post-program Reset action. */
} program_options_t;

typedef struct {
    bool readram;
    bool readcode;
    bool readuicr;
    bool readficr;
    bool readqspi;
/* Workaround for issue https://bugs.python.org/issue22273 in ctypes, force by-value struct argument into stack.*/
    bool reserved[3]; 
} read_options_t;

typedef struct {
    
    /* Type of device we are dealing with. */
    device_family_t     device_family;
    device_version_t    device_type;
    
    /* Code flash info. */
    uint32_t            code_address;
    uint32_t            code_page_size;
    uint32_t            code_size;

    /* Info flash info. */
    uint32_t            uicr_address;
    uint32_t            info_page_size;

    /* RAM info. */
    bool                code_ram_present;
    uint32_t            code_ram_address;
    uint32_t            data_ram_address;
    uint32_t            ram_size;

    /* QSPI info. */
    bool                qspi_present;
    uint32_t            xip_address;
    uint32_t            xip_size;

    /* Pin reset. */
    uint32_t            pin_reset_pin;

} device_info_t;

typedef struct {
    uint32_t          serial_number;                            /* Debug probe serial number. */
    uint32_t          clockspeed_khz;                           /* Requested SWD frequency in kHz. Actual frequency may differ because of device limitations.  */
    char              firmware_string[NRFJPROG_STRING_LENGTH];  /* Firmware version of debug probe. */
    uint32_t          num_com_ports;                            /* Number of com ports available in debug probe. */
    com_port_info_t   com_ports[NRFJPROG_COM_PER_JLINK];        /* Array of at least num_com_ports com port info structs. */
} probe_info_t;

typedef struct {
    uint32_t version_major;                              /* Major version of interface library. */
    uint32_t version_minor;                              /* Minor version of interface library. */
    char     version_revision;                           /* Revision of interface library. */

    char     file_path[NRFJPROG_MAX_PATH];               /* Path to interface library binary. */
} library_info_t;


/**
 * @brief  Returns the highlevelnrfjprogdll DLL version.
 *
 * @param   major                               Pointer to store the DLL major version.
 * @param   minor                               Pointer to store the DLL minor version.
 * @param   micro                               Pointer to store the DLL micro version.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The major parameter is NULL.
 *                                              The minor parameter is NULL.
 *                                              The micro parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_dll_version(uint32_t * major, uint32_t * minor, uint32_t * micro);


/**
 * @brief   Opens the highlevelnrfjprogdll DLL and prepares it for use.
 *
 * @details Opens the highlevelnrfjprogdll DLL and prepares it for use, loading the necessary functions from the nrfjprogdll DLL. If default_jlink_path is given,
 *          the provided path will be used as the default value for the jlink_path parameter in NRFJPROG_probe_init() function. Note that the path should include
 *          the name of the DLL itself (i.e. "JLinkARM.dll"). If default_jlink_path is not given, an automatic search for the latest installed DLL will be performed.
 *          Only JLinkARM DLLs whose versions are greater than a minimum version will be accepted. The minimum version for the JLinkARM DLL is defined in 
 *          MIN_JLINK_MAJOR_VERSION and MIN_JLINK_MINOR_VERSION defines. If logging mechanism is desired, a function pointer can be passed in log_cb parameter or 
 *          prog_cb parameters. For details of the function type accepted by the function, see log_callback and progress_callback typedefs elsewhere in this file.
 *
 * @pre     Before the execution of this function the DLL must not be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function.
 *
 * @post    After the execution of this function the DLL will be ready for use.
 * @post    After the execution of this function certain resources will be allocated. To deallocate these resources and allocated memory see NRFJPROG_dll_close() function.
 *
 * @param   default_jlink_path                          Path to a JLinkARM DLL. May be NULL.
 * @param   log_cb                                      Callback for reporting informational and error messages. May be NULL.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                           The NRFJPROG_dll_open() function has already been called.
 * @retval  JLINKARM_DLL_NOT_FOUND                      The automatic search for the JLINKARM DLL did not yield a usable DLL.
 *                                                      The jlink_path provided did not yield a usable DLL.
 * @retval  JLINKARM_DLL_TOO_OLD                        The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED            An error occurred while opening the JLinkARM DLL.
 *                                                      A required function could not be loaded from the DLL.
 * @retval  NRFJPROG_SUB_DLL_NOT_FOUND                  The DLL could not find the NRFJPROG DLL, please reinstall nRF tools.
 * @retval  NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED        An error occurred while opening the NRFJPROG DLL.
 * @retval  NRFJPROG_SUB_DLL_COULD_NOT_LOAD_FUNCTIONS   A required function could not be loaded from the NRFJPROG DLL.
 */
nrfjprogdll_err_t NRFJPROG_dll_open(const char * default_jlink_path, msg_callback * log_cb);

/**
 * @brief   Closes the highlevelnrfjprogdll DLL.
 *
 * @details Closes the highlevelnrfjprogdll DLL and frees allocated resources. NRFJPROG_dll_close must be called before unloading the 
 *          highlevelnrfjprogdll DLL if NRFJPROG_dll_open() has been called.
 *
 * @post    After the execution of this function the DLL will not be ready for use. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @post    After the execution of this function the allocated resources in NRFJPROG_dll_open() function will be deallocated.
 */
void NRFJPROG_dll_close(void);


/**
 * @brief   Checks if the highlevelnrfjprogdll DLL is ready for use.
 *
 * @details Checks if the NRFJPROG_dll_open() function has been called since the last call to NRFJPROG_dll_close() or since the loading 
 *          of the highlevelnrfjprogdll DLL.
 *
 * @param   is_opened                           Pointer to store the state of the DLL.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The is_opened parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_dll_open(bool * is_opened);


/**
 * @brief   Returns the serial numbers of the USB J-Link emulators connected to the PC.
 *
 * @details Returns the amount and serial numbers of the USB J-Link emulators connected to the PC. If serial_numbers_len is 0, 
 *          only the amount of USB J-Link emulators connected to the PC is returned. Since NRFJPROG_get_connected_probes() function
 *          is a general function that does not receive a Probe_handle_t as first parameter but connects to the emulators, the default
 *          JLinkARM DLL passed or located in NRFJPROG_dll_open() will be used.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 *
 * @param   serial_numbers                      Array in which to store the enumerated serial numbers.
 * @param   serial_numbers_len                  Number of uint32_t values that can be stored in the
 *                                              serial_numbers array (may be zero).
 * @param   num_available                       The number of serial numbers that were enumerated.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The serial_numbers parameter is NULL.
 *                                              The num_available parameter is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers.
 */
nrfjprogdll_err_t NRFJPROG_get_connected_probes(uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);


/**
 * @brief   Allocates and initializes a new probe connection handle.
 *
 * @details Allocates and initializes a new probe connection handle. The probe handle initialized and returned in the debug_probe pointer will 
 *          be required in every function that performs a connection to a device. The used JLINKARM DLL in the afore mentioned operations requiring the 
 *          handle can be provided in the jlink_path parameter. If not provided, the default JLinkARM DLL passed or located in NRFJPROG_dll_open() will be used.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the Segger JLink emulator must be physically connected to a powered board with a Nordic nRF51 or nRF52 device.
 *
 * @during  During the execution of this function, if the device was not in debug interface mode an unavoidable pin reset will occur. The pin reset will not occur if the debugger used is the Nordic's JLink On-Board-Debugger.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_reset() function.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset(), NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Pointer to probe handle type.
 * @param   log_cb                              Callback for reporting informational and error messages. May be NULL.
 * @param   serial_number                       Serial number of the emulator to connect to.
 * @param   jlink_path                          Path to the JLinkARM DLL to be used with this probe. May be NULL. Does not support unicode paths.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers
 * @retval  EMULATOR_NOT_CONNECTED              The emulator serial_number is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_probe_init(Probe_handle_t * debug_probe, progress_callback * prog_cb, msg_callback * log_cb, uint32_t serial_number, const char * jlink_path);


/**
 * @brief   Allocates and initializes a new probe connection handle.
 *
 * @details Allocates and initializes a new probe connection handle. The probe handle initialized and returned in the debug_probe pointer will
 *          be required in every function that performs a connection to a device. The used JLINKARM DLL in the afore mentioned operations requiring the
 *          handle can be provided in the jlink_path parameter. If not provided, the default JLinkARM DLL passed or located in NRFJPROG_dll_open() will be used.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the Segger JLink emulator must be physically connected to a powered board with a Nordic nRF91 device.
 *
 * @during  During the execution of this function, if the device was not in debug interface mode an unavoidable pin reset will occur. The pin reset will not occur if the debugger used is the Nordic's JLink On-Board-Debugger.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_reset() function.
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset(), NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Pointer to probe handle type.
 * @param   prog_cb                             Callback for reporting currently performed action. May be NULL.
 * @param   log_cb                              Callback for reporting informational and error messages. May be NULL.
 * @param   serial_number                       Serial number of the emulator to connect to.
 * @param   coprocessor                         Coprocessor to target.
 * @param   jlink_path_cstr                     Path to the JLinkARM DLL to be used with this probe. May be NULL. Does not support unicode paths.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers
 * @retval  EMULATOR_NOT_CONNECTED              The emulator serial_number is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_dfu_init(Probe_handle_t * debug_probe, progress_callback * prog_cb, msg_callback * log_cb, uint32_t serial_number, coprocessor_t coprocessor, const char * jlink_path_cstr);

/**
 * @brief   Allocates and initializes a new probe connection handle.
 *
 * @details Allocates and initializes a new probe connection handle. The probe handle initialized and returned in the debug_probe pointer will
 *          be required in every function that performs a connection to a device. 
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the device must be physically connected to a powered board with a device running mcuboot in serial recovery mode.
 *
 * @post    After the execution of this function, the device will be in application mode.
 *
 * @param   debug_probe                         Pointer to probe handle type.
 * @param   prog_cb                             Callback for reporting currently performed action. May be NULL.
 * @param   log_cb                              Callback for reporting informational and error messages. May be NULL.
 * @param   serial_port                         Serial port of the device connected.
 * @param   baud_rate                           Serial port baud rate.
 * @param   response_timeout                    Wait time for a response (milliseconds).
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 */
    nrfjprogdll_err_t NRFJPROG_mcuboot_dfu_init(Probe_handle_t * debug_probe,
                                                progress_callback * prog_cb,
                                                msg_callback * log_cb,
                                                const char * serial_port,
                                                const uint32_t baud_rate,
                                                const uint32_t response_timeout);

/**
* @brief   Allocates and initializes a new probe connection handle.
*
* @details Allocates and initializes a new probe connection handle. The probe handle initialized and returned in
* the debug_probe pointer will be required in every function that performs a connection to a device.
*
* @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see
* NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
* @pre     Before the execution of this function the device must be physically connected to a powered board with a
* device running modem dfu application. The device must be reset after each modem dfu attempt.
*
* @post    After the execution of this function, the device will be ready for serial DFU operations.
*
* @param   debug_probe                          Pointer to probe handle type.
* @param   prog_cb                              Callback for reporting currently performed action. May be NULL.
* @param   log_cb                               Callback for reporting informational and error messages. May be
* NULL.
* @param   serial_port                          Serial port of the device connected.
* @param   baud_rate                            Serial port baud rate.
* @param   response_timeout                     Wait time for a response (milliseconds).
*
* @retval  SUCCESS
* @retval  INVALID_OPERATION                    The NRFJPROG_dll_open() function has not been called.
* @retval  INVALID_PARAMETER                    The debug_probe pointer is NULL.
* @retval  SERIAL_PORT_RESOURCE_ERROR           Unable to open serial port.
* @retval  OUT_OF_MEMORY                        Could not allocate new probe object
 */
nrfjprogdll_err_t NRFJPROG_modemdfu_dfu_serial_init(Probe_handle_t * debug_probe,
                                                    progress_callback * prog_cb,
                                                    msg_callback * log_cb,
                                                    const char * serial_port,
                                                    const uint32_t baud_rate,
                                                    const uint32_t response_timeout);


/**
 * @brief   Uninitializes an initialized debug_probe handle.
 *
 * @details Uninitializes an initialized debug_probe handle and frees the allocated resources.
 *
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Pointer to probe handle type.
 * @param   prog_cb                             Callback for reporting currently performed action. May be NULL.
 * @param   log_cb                              Callback for reporting informational and error messages. May be NULL.
 * @param   serial_number                       Serial number of the emulator to connect to.
 * @param   coprocessor                         Coprocessor to target.
 * @param   jlink_path_cstr                     Path to the JLinkARM DLL to be used with this probe. May be NULL. Does not support unicode paths.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 *                                              The debug_probe is not initialized.
 */
nrfjprogdll_err_t NRFJPROG_probe_uninit(Probe_handle_t * debug_probe);


/**
 * @brief   Attempts to reset the selected J-Link OB.
 *
 * @details Connects to and resets the J-Link OB.
            This operation is only available in debug probes of type J-Link OB-SAM3U128-V2-NordicSemi.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers
 * @retval  EMULATOR_NOT_CONNECTED              The emulator serial_number is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_probe_reset(Probe_handle_t debug_probe);


/**
 * @brief   Replaces the firmware on the selected J-Link probe.
 *
 * @details Connects to and replaces the firmware on the selected J-Link debug probe with the firmware version shipped with
 *          the selected J-Link DLL.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @post    After the execution of this function, the debug probe will be reset and available.
 *
 * @param   debug_probe                         Probe handle.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  OUT_OF_MEMORY                       Could not allocate buffer for reading serial numbers
 * @retval  EMULATOR_NOT_CONNECTED              The emulator serial_number is not connected to the PC.
 */
nrfjprogdll_err_t NRFJPROG_probe_replace_fw(Probe_handle_t debug_probe);


/**
* @brief   Initializes the probe handle for QSPI communication.
*
* @details Initializes the probe handle for QSPI communication with the information provided in the qspi_init_params struct.
*          This information will be used to perform QSPI operations if needed.
*
* @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
*
* @post    After the execution of this function, the probe will be able to perform QSPI operations.
*
* @param   debug_probe                         Probe handle.
* @param   memory_size                         Size of the attached qspi flash memory.
* @param   qspi_ini_path                       Path to the .ini containing initialization parameters for the QSPI memory interface.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
*                                              The qspi_ini_path parameter is null.
*                                              Could not find the supplied .ini file.
*                                              The supplied .ini file has syntax errors.
*                                              One of the QSPI parameters failed verification.
* @retval  INVALID_DEVICE_FOR_OPERATION        The connected device does not support QSPI.
 */
nrfjprogdll_err_t NRFJPROG_probe_setup_qspi(Probe_handle_t debug_probe, uint32_t memory_size, qspi_init_params_t qspi_init_params);


/**
* @brief   Sets the coprocessor to use for future operations.
*
* @details Initializes the probe handle for multicore devices by setting the target coprocessor.
*
* @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
*
* @post    After the execution of this function, the probe will perform operations on the selected coprocessor.
*
* @param   debug_probe                         Probe handle.
* @param   coprocessor                         Coprocessor to select.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The debug_probe parameter is null..
 */
nrfjprogdll_err_t NRFJPROG_probe_set_coprocessor(Probe_handle_t debug_probe, coprocessor_t coprocessor);


/**
* @brief   Function deprecated in favor of NRFJPROG_get_probe_info(). Will be removed in a future version.
* 
* @details Reads back the serial number of the USB J-LINK emulator used to initialize the provided debug_probe handle.
*
* @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().

* @param   debug_probe                         Probe handle.
* @param   serial_number                       Pointer to where serial number value should be stored.
*
* @retval  SUCCESS
* @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
*                                              The serial_number parameter is null.
 */
nrfjprogdll_err_t NRFJPROG_get_snr(Probe_handle_t debug_probe, uint32_t * serial_number);


/**
 * @brief   Function deprecated in favor of NRFJPROG_get_library_info(). Will be removed in a future version.
 * 
 * @details Reads back the version of the JLINKARM DLL to be used with the provided debug_probe handle.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 * @param   major                               Pointer for storing the DLL major version.
 * @param   minor                               Pointer for storing the DLL minor version.
 * @param   revision                            Pointer for storing the DLL revision.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
 *                                              The major parameter is NULL.
 *                                              The minor parameter is NULL.
 *                                              The revision parameter is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_get_jlinkarm_version(Probe_handle_t debug_probe, uint32_t * major, uint32_t * minor, char * revision);


/**
 * @brief   Reads out information about the provided debug probe.
 * 
 * @details Reads out the version and file path of the detected interface library of the provided debug probe into the provided library_info struct.
 *
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 
 * @param   debug_probe                         Probe handle.
 * @param   library_info                        Pointer to library_info_t struct.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
 *                                              The library_info parameter is null.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_get_library_info(Probe_handle_t debug_probe, library_info_t * library_info);


/**
 * @brief   Reads out information about the provided debug probe.
 * 
 * @details Reads out the version string, serial number, and the programming speed of the provided debug probe into the probe_info struct.
 *
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().

 * @param   debug_probe                         Probe handle.
 * @param   probe_info                          Pointer to probe_info_t struct.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
 *                                              The probe_info parameter is null.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_get_probe_info(Probe_handle_t debug_probe, probe_info_t * probe_info);


/**
 * @brief   Reads information from the device connected to the provided debug_probe handle.
 *
 * @details Reads information from the device connected to the initialized probe.
 *          
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 * @param   device_info                         Pointer to where the device's info should be stored.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe parameter is null.
 *                                              The device_info parameter is null.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The target device is readback protected.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_get_device_info(Probe_handle_t debug_probe, device_info_t * device_info);


/**
 * @brief   Reads readback protection level from the device connected to the provided debug_probe handle.
 *
 * @details Reads readback protection level from the device connected to the initialized probe.
 *          
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 * @param   protection                         Pointer to where the device's readback protection level should be stored.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The dll is not open.
 *                                              The debug_probe parameter is null.
 *                                              The protection parameter is null.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_get_readback_protection(Probe_handle_t debug_probe, readback_protection_status_t * protection);


/**
 * @brief   Enable readback protection in the device connected to the provided debug_probe handle.
 *
 * @details Protects the device connected to the initialized probe at the specified readback protection level.
 *          
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 * @param   device_info                         Pointer to where the device's info should be stored.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The dll is not open.
 *                                              The debug_probe parameter is null.
 *                                              The protection_level parameter is not a valid protection level for this device.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect(Probe_handle_t debug_probe, readback_protection_status_t protection_level);


/**
 * @brief   Programs the provided .hex file onto the device connected to the provided debug_probe handle.
 *
 * @details Programs the provided .hex file onto the device connected to the provided debug_probe handle. By using the program_options parameter, the 
 *          verification, pre-programming erase and post-programming reset can be configured. See the description of program_options_t for details.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     If a QSPI operation is to be performed, QSPI must be enabled in the probe. To enable QSPI, see NRFJPROG_probe_setup_qspi().
 * @pre     Before the execution of this function, access port protection must be disabled. To disable access port protection, see NRFJPROG_recover() function.
 * 
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   hex_path                            Path to file to be programmed.
 * @param   program_options                     Pointer to struct containing programming parameters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The hex file path is an empty string.
 *                                              The hex file cannot be found.
 *                                              The hex file cannot be read.
 *                                              The program_options pointer is NULL.
 *                                              The debug_probe pointer is NULL.
 *                                              The hex file contains data in nonexistent regions.
 *                                              The hex file contains data that crosses memory barriers.
 * @retval  INVALID_OPERATION                   The dll is not open.
 *                                              The hex file was not valid.
 *                                              The hex file cannot be opened.
 *                                              UICR erase requested in ERASE_PAGES mode.
 *                                              ERASE_SECTOR_AND_UICR attempted on QSPI memory.
 *                                              QSPI operation on non-QSPI enabled device.
 *                                              QSPI has not been enabled for this probe.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available because the device is readback protected.
 *                                              The hex file contains data in a write protected region.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The device does not support an attempted operation.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              Attempted to access unpowered RAM.
 * @retval  OUT_OF_MEMORY                       Could not allocate programming buffers.
 * @retval  EMULATOR_NOT_CONNECTED              The emulator serial_number is not connected to the PC.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  VERIFY_ERROR                        Program verification failed.
 */
nrfjprogdll_err_t NRFJPROG_program(Probe_handle_t debug_probe, const char * hex_path, program_options_t program_options);


/**
 * @brief   Reads the memory of the device connected to the provided debug_probe handle into the to the provided .hex file path.
 *
 * @details Reads the memory of the device connected to the provided debug_probe handle into the to the provided .hex file path. The areas of the
 *          device to read can be configured by the use of the read_options paramter. See the description of read_options_t for details.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     If a QSPI operation is to be performed, QSPI must be enabled in the probe. To enable QSPI, see NRFJPROG_probe_setup_qspi().
 * @pre     Before the execution of this function, access port protection must be disabled. To disable access port protection, see NRFJPROG_recover() function.
 * 
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   hex_path                            Path to output file.
 * @param   read_options                        Pointer to struct containing read parameters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The hex file path is an empty string.
 *                                              The hex file cannot be overwritten.
 *                                              The read_options pointer is NULL.
 *                                              The debug_probe pointer is NULL.
 *                                              The hex file cannot be opened for writing.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to read is in unpowered RAM.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available because the device is readback protected.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The connected device does not support an attempted operation.
 * @retval  OUT_OF_MEMORY                       Could not write to the hex file.
 *                                              Could not allocate hex file buffer.
 *                                              Could not extend hex file buffer.
 *                                              Could not save the hex file.
 */
nrfjprogdll_err_t NRFJPROG_read_to_file(Probe_handle_t debug_probe, const char * hex_path, read_options_t read_options);


/**
 * @brief   Compares the contents of the provided .hex file against the contents of the memory of the device connected to the provided debug_probe handle.
 *
 * @details Compares the contents of the provided .hex file against the contents of the memory of the device connected to the provided debug_probe handle. The
 *          compare method can be configured with the verify_action parameter. See the description of verify_action_t for details. 
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     If a QSPI operation is to be performed, QSPI must be enabled in the probe. To enable QSPI, see NRFJPROG_probe_setup_qspi().
 * @pre     Before the execution of this function, access port protection must be disabled. To disable access port protection, see NRFJPROG_recover() function.
 * 
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   hex_path                            Path to file to verify against.
 * @param   verify_action                       Type of verify to perform.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The hex file path is an empty string.
 *                                              The hex file cannot be read.
 *                                              The debug_probe pointer is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 *                                              The address to read is in unpowered RAM.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available because the device is readback protected.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The connected device does not support an attempted operation.
 * @retval  OUT_OF_MEMORY                       Could not allocate program buffers.
 * @retval  VERIFY_ERROR                        Program verification failed.
 */
nrfjprogdll_err_t NRFJPROG_verify(Probe_handle_t debug_probe, const char * hex_path, verify_action_t verify_action);


/**
 * @brief   Erases the flash of the device connected to the provided debug_probe handle.
 *
 * @details Erases the flash of the device connected to the provided debug_probe handle. The target flash of the operation is determined by the start_address parameter.
 *          If the provided start_address parameter is in the XIP region of a device equiped with QSPI, the operation targets the externally QSPI-connected flash.
 *          Otherwise, the operation targets the internal flash of the device. The type of erase can be configured through the erase action parameter. If erase_action 
 *          is ERASE_ALL, all the user available flash including the UICR region if the target is the internal flash will be erased. In this case, the provided 
 *          end_address will be ignored. If erase_action is ERASE_PAGES, all pages between the provided start_address the provided end_address will be erased, including
 *          the UCIR if the start_address and end_address are in the UICR region. The possible erase_action_t ERASE_PAGES_INCLUDING_UICR is invalid for this operation. 
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     If a QSPI operation is to be performed, QSPI must be enabled in the probe. To enable QSPI, see NRFJPROG_probe_setup_qspi().
 * @pre     Before the execution of this function, access port protection must be disabled. To disable access port protection, see NRFJPROG_recover() function.
 *
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   erase_action                        Erase action selector.
 * @param   start_address                       Start address for erase action.
 * @param   end_address                         End address for erase action.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 * @retval  INVALID_OPERATION                   UICR erase requested in ERASE_PAGES mode.
 *                                              ERASE_SECTOR_AND_UICR attempted on QSPI memory.
 *                                              QSPI operation on non-QSPI enabled device.
 *                                              QSPI has not been enabled for this probe.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available because the device is readback protected.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_erase(Probe_handle_t debug_probe, erase_action_t erase_action, uint32_t start_address, uint32_t end_address);


/**
 * @brief   Recovers the device connected to the provided debug_probe handle.
 *
 * @details This operation attempts to recover the device and leave it as it was when it left Nordic factory. It will attempt to
 *          connect, erase all user available flash, halt and eliminate any protection. Note that this operation may take up to 30 s
 *          if the device was readback protected. Note as well that this function only affects internal flash and CPU, but does not 
 *          erase, reset or stop any peripheral, oscillator source nor extarnally QSPI-connected flash. The operation will therefore 
 *          leave the watchdog still operational if it was running.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 * @post    After the execution of this function all device RAM will be powered.
 * @post    After the execution of this function the device code and UICR flash will be erased.
 * @post    After the execution of this function if the device was readback protected and was an nRF52, the device RAM will be erased.
 * @post    After the execution of this function if the device was readback protected, the device will no longer be readback protected.
 * @post    After the execution of this function the POWER.RESETREAS register will be cleared.
 *
 * @param   debug_probe                         Probe handle.

 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_MPU_CONFIG    The operation is not available due to the MPU configuration. The operation is not available due to the presence of Pre-Programmed Factory Code (PPFC). 
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_recover(Probe_handle_t debug_probe);


/**
 * @brief   Reads data_len bytes starting at the addr address from the device connected to the debug_probe handle.
 *
 * @details Reads data_len bytes starting at the addr address from the device connected to the debug_probe handle without verifying that the 
 *          addresses are accessible or even exist. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   addr                                Start address of the region read.
 * @param   data                                Pointer to an array to read to.
 * @param   data_len                            Number of bytes to read.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 *                                              Target area in in XIP region but NRFJPROG_probe_setup_qspi() has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is null.
 *                                              The data pointer is null.
 *                                              Attempted to read out of QSPI area.
 * @retval  RAM_IS_OFF_ERROR                    Attempted to read powered-down RAM.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to read-back protection.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_read(Probe_handle_t debug_probe, uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Reads one 32-bit word at the addr address from the device connected to the debug_probe handle.
 *
 * @details Reads one 32-bit word at the addr address from the device connected to the debug_probe handle without verifying that the address is accessible
 *          or even exists. The addr parameter needs to be 32-bit aligned. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   addr                                Address of the word to read.
 * @param   data                                Pointer to where the read data should be written.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 *                                              Target area in in XIP region but NRFJPROG_probe_setup_qspi() has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is null.
 *                                              The data pointer is null.
 *                                              The addr parameter is not word aligned.
 * @retval  RAM_IS_OFF_ERROR                    Attempted to read powered-down RAM.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to read-back protection.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_read_u32(Probe_handle_t debug_probe, uint32_t addr, uint32_t * data);


/**
 * @brief   Writes data_len bytes starting at the addr address into the device connected to the debug_probe handle.
 *
 * @details Writes data_len bytes starting at the addr address into the device connected to the debug_probe handle without verifying that the addresses
 *          are accessible or even exist. If address is in internal flash or UICR, it will control the NVMC in order to write into it. If the address 
 *          is in the XIP region of the device and the device is equpped with a QSPI peripheral, the operation will control the QSPI in order to write 
 *          into the external flash. Note that if the target address is in unpowered RAM, the operation will fail.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   address                             Start address of the region to write to.
 * @param   data                                Pointer to an array with the data to write.
 * @param   data_len                            Length of the data array.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 *                                              The data parameter is NULL.
 *                                              Attempted to write out of QSPI area.
 * @retval  INVALID_OPERATION                   The target flash is not erased.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  RAM_IS_OFF_ERROR                    Attempted to read disabled RAM.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_write(Probe_handle_t debug_probe, uint32_t address, const uint8_t * data, uint32_t data_len);


/**
 * @brief   Writes one 32-bit word at the address address into the device connected to the debug_probe handle.
 *
 * @details Writes one 32-bit word at the address address into the device connected to the debug_probe handle without verifying that the address
 *          is accessible or even exists. If address is in internal flash or UICR, it will control the NVMC in order to write into it. If the address 
 *          is in the XIP region of the device and the device is equpped with a QSPI peripheral, the operation will control the QSPI in order to write
 *          into the external flash. Writes need to be 32-bit aligned. Note that if the target address is in unpowered RAM, the operation will fail. If
 *          the address is internal or external flash, it will read the contents before writing and fail if not erased.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function, the device CPU will be halted. To unhalt the device CPU, see NRFJPROG_reset() and NRFJPROG_run() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   addr                                Address to write to.
 * @param   data                                Value to write.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 *                                              The address is not 32-bit aligned.
 * @retval  INVALID_OPERATION                   The target flash is not erased.
 * @retval  JLINKARM_DLL_TOO_OLD                The version of JLinkARM is lower than the minimum version required.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  RAM_IS_OFF_ERROR                    Attempted to write disabled RAM.
 * @retval  NVMC_ERROR                          Flash operation failed.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 */
nrfjprogdll_err_t NRFJPROG_write_u32(Probe_handle_t debug_probe, uint32_t address, uint32_t data);


/**
 * @brief   Resets the device connected to the provided debug_probe handle.
 *
 * @details Resets the device connected to the provided debug_probe handle. The type of reset can be configured through the reset_action parameter. See the description of reset_action_t
 *          for details of available resets.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @during  During the execution of this function the emulator mode may be changed to JTAG. If the execution fails the emulator might be left in JTAG mode. If the execution fails, try again to return to SWD mode.
 *
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51 and and reset_action was RESET_SYSTEM. To exit debug interface mode see NRFJPROG_reset().
 * @post    After the execution of this function the device CPU will be running.
 *
 * @param   debug_probe                         Probe handle.
 * @param   reset_action                        Type of reset to perform. See the description of reset_action_t for details. 
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is NULL.
 *                                              The reset_action cannot be encoded as a reset_action_t enum.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  INVALID_DEVICE_FOR_OPERATION        The reset_action parameter is RESET_DEBUG but the device is without CTRL-AP.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The reset_action parameter is RESET_SYSTEM and the device is readback protected.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_reset(Probe_handle_t debug_probe, reset_action_t reset_action);


/**
 * @brief   Starts the device CPU.
 *
 * @details Starts the device CPU with sp as stack pointer and pc as program counter.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *  
 * @post    After the execution of this function the device will be in debug interface mode if the device was an nRF51. To exit debug interface mode see NRFJPROG_reset().
 *
 * @param   debug_probe                         Probe handle.
 * @param   sp                                  New stack pointer.
 * @param   pc                                  New program counter.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_dll_open() function has not been called.
 * @retval  INVALID_PARAMETER                   The debug_probe pointer is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_NOT_FOUND              The jlink_path did not yield a usable DLL.
 * @retval  JLINKARM_DLL_COULD_NOT_BE_OPENED    An error occurred while opening the JLinkARM DLL.
 *                                              A required function could not be loaded from the DLL.
 */
nrfjprogdll_err_t NRFJPROG_run(Probe_handle_t debug_probe, uint32_t pc, uint32_t sp);


/**
 * @brief   Checks if the RTT is started.
 *
 * @details Checks if the NRFJPROG_rtt_start() function has been called since the last call to NRFJPROG_rtt_stop() or since initializing the probe.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 *
 * @param   debug_probe                         Probe handle.
 * @param   started                             Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 * @retval  INVALID_PARAMETER                   The started parameter is null.
 */
nrfjprogdll_err_t NRFJPROG_is_rtt_started(Probe_handle_t handle, bool * started);


/**
 * @brief   Indicates to the dll the location of the RTT control block in the device memory.
 *
 * @details Indicates to the dll the location of the RTT control block in the device memory, to accelerate the discovery of the RTT control block.
 *          If the RTT control block is not located at the address given, NRFJPROG_rtt_start() will never locate the RTT control block.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must not be started. To stop the RTT, see NRFJPROG_rtt_stop() function.
 *
 * @param   debug_probe                         Probe handle.
 * @param   address                             RTT control block address in device memory.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_rtt_start() function has been called.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(Probe_handle_t handle, uint32_t address);


/**
 * @brief   Starts RTT.
 *
 * @details Starts RTT, initiating the search for the RTT control block and preparing the DLL for RTT operations. If NRFJPROG_rtt_set_control_block_address() has been called,
 *          the address provided will be used to locate the control block. Since NRFJPROG_rtt_start() function is not blocking, to find out if NRFJPROG_rtt_start() has located
 *          the RTT control block and is ready to receive commands, see NRFJPROG_rtt_is_control_block_found() function. After the execution of NRFJPROG_rtt_start(), please
 *          ensure that NRFJPROG_rtt_stop() function will be executed.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must not be started. To stop the RTT, see NRFJPROG_rtt_stop() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, the RTT control block must be present in RAM. Normally it is the firmware execution that writes the control block into RAM. To run the firmware, see NRFJPROG_go() and NRFJPROG_run() functions.
 *
 * @post    After the execution of this function, the device will be in debug interface mode. To exit debug interface mode, see NRFJPROG_pin_reset(), NRFJPROG_disconnect_from_emu() and NRFJPROG_close_dll() functions.
 * @post    After the execution of this function, the emulator will be connected to the device. To disconnect from the device, see NRFJPROG_disconnect_from_emu(), NRFJPROG_close_dll() and NRFJPROG_disconnect_from_device() functions.
 *
 * @param   debug_probe                         Probe handle.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              The NRFJPROG_rtt_start() function has already been called.
 * @retval  WRONG_FAMILY_FOR_DEVICE             The device connected is not an NRF52.
 * @retval  NOT_AVAILABLE_BECAUSE_PROTECTION    The operation is not available due to readback protection.
 * @retval  CANNOT_CONNECT                      It is impossible to connect to any nRF device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_start(Probe_handle_t handle);


/**
 * @brief   Checks if RTT control block has been found.
 *
 * @details Checks if RTT control block has been found. Can be used to make sure RTT is ready to be used before calling NRFJPROG_rtt_read() and NRFJPROG_rtt_write() functions.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   is_control_block_found              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The is_found parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(Probe_handle_t handle, bool * is_control_block_found);


/**
 * @brief   Stops RTT.
 *
 * @details Stops RTT and clears the RTT Control Buffer.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @post    After the execution of this function, the RTT control block in RAM will have been erased. Therefore, before another NRFJPROG_rtt_start() can be issued, the control block must be rewritten to RAM. See NRFJPROG_rtt_start() function for details.
 *
 * @param   debug_probe                         Probe handle.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop(Probe_handle_t handle);


/**
 * @brief   Reads from an RTT channel.
 *
 * @details Reads up to data_len characters from the up_channel_index RTT channel.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   up_channel_index                    RTT channel index to read from.
 * @param   data                                Pointer of the location to store the value.
 * @param   data_len                            Number of bytes to read.
 * @param   data_read                           Pointer to the location to store the actual number of read characters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The data_read parameter is null.
 *                                              There is no channel in the device with the given up_channel_index index.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read(Probe_handle_t handle, uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);


/**
 * @brief   Writes to an RTT channel.
 *
 * @details Writes the given data_len length data string into the down_channel_index RTT channel.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   down_channel_index                  RTT channel index to write to.
 * @param   data                                Pointer to a string with the string to write.
 * @param   data_len                            Length of the string.
 * @param   data_written                        Pointer to the location to store the actual number of written characters.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The data parameter is null.
 *                                              The data_read parameter is null.
 *                                              There is no channel in the device with the given down_channel_index index.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_write(Probe_handle_t handle, uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief   Gets the number of RTT channels.
 *
 * @details Gets the number of down and up channels in the device.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   down_channel_number                 Pointer to store the number of down channels.
 * @param   up_channel_number                   Pointer to store the number of up channels.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The down_channel_number parameter is null.
 *                                              The up_channel_number parameter is null.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(Probe_handle_t handle, uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief   Reads the info from one RTT channel.
 *
 * @details Reads the info from one RTT channel of index channel_index of direction dir. Reads the rtt channel size into channel_size and name into channel_name pointers.
 *
 * @pre     Before the execution of this function the DLL must be ready for use. To query the state of the DLL see NRFJPROG_is_dll_open() function. To ready the DLL for use see NRFJPROG_dll_open() function.
 * @pre     Before the execution of this function the provided debug_probe handle must be initialized. To initialize the probe, see NRFJPROG_probe_init().
 * @pre     Before the execution of this function, the RTT must be started. To start the RTT, see NRFJPROG_rtt_start() function.
 * @pre     Before the execution of this function, a connection to the emulator must be established. To establish a connection, see NRFJPROG_connect_to_emu_with_snr() and NRFJPROG_connect_to_emu_without_snr() functions.
 * @pre     Before the execution of this function, a connection to the device must be established. To establish a connection, see NRFJPROG_connect_to_device() and NRFJPROG_rtt_start() functions.
 *
 * @param   debug_probe                         Probe handle.
 * @param   channel_index                       Channel index to read the info from.
 * @param   dir                                 Channel direction of channel to read the info from.
 * @param   channel_name                        Pointer to store the channel name, must be able to store at least 32 characters.
 * @param   channel_name_len                    Size of channel_name storage size.
 * @param   channel_size                        Pointer to store the channel size.
 *
 * @retval  SUCCESS
 * @retval  INVALID_OPERATION                   The NRFJPROG_open_dll() function has not been called.
 *                                              The NRFJPROG_connect_to_emu_with_snr() or NRFJPROG_connect_to_emu_without_snr() function has not been called.
 *                                              There is no connection between the emulator and the device.
 *                                              The NRFJPROG_rtt_start() function has not been called.
 * @retval  INVALID_PARAMETER                   The channel_name parameter is null.
 *                                              The channel_size parameter is null.
 *                                              The channel_size_len parameter is less than 32.
 *                                              The dir parameter is neither UP_DIRECTION or DOWN_DIRECTION defined in rtt_direction_t enum in DllCommonDefinitions.h
 *                                              The channel with index channel_index does not exist in the device.
 * @retval  JLINKARM_DLL_ERROR                  The JLinkARM DLL function returned an error.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(Probe_handle_t handle, uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


#if defined(__cplusplus)
}
#endif

#endif  /* highlevelnrfjprogdll_FUNC_H */

