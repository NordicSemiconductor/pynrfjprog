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

#ifndef DLL_COMMON_DEFINITIONS_H
#define DLL_COMMON_DEFINITIONS_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif


#define MIN_JLINK_MAJOR_VERSION (6UL)
#define MIN_JLINK_MINOR_VERSION (42UL)

#define JLINKARM_SWD_MIN_SPEED_KHZ (125UL)
#define JLINKARM_SWD_DEFAULT_SPEED_KHZ (2000UL)
#define JLINKARM_SWD_MAX_SPEED_KHZ (50000UL)

#define NRFJPROG_STRING_LENGTH (256)
#define NRFJPROG_MAX_PATH (260) /* Paths are assumed to not exceed 260 characters. */
#define NRFJPROG_COM_PER_JLINK (10)

/* Deprecated Macro. Use result of NRFJPROG_read_ram_sections_count() function instead. */
#define MAX_RAM_BLOCKS (16)

typedef struct
{
    char path[NRFJPROG_MAX_PATH]; /* OS-specific com port path */
    uint32_t vcom;                /* VCOM out number */
    uint32_t serial_number;       /* Serial number of debug probe */
} com_port_info_t;

typedef enum
{
    RAM_OFF = 0,
    RAM_ON,
} ram_section_power_status_t;

/* Identified device versions of nRF devices. */
typedef enum
{
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    SP = R13,
    R14,
    LR = R14,
    R15,
    PC = R15,
    XPSR,
    MSP,
    PSP
} cpu_registers_t;

/* Possible readback protection status. */
typedef enum
{
    NONE     = 0, // No readback protection enabled
    REGION_0 = 1, // Region 0 is readback protected
    ALL      = 2, // All memories are readback protected
    BOTH     = 3, // Region 0 and Region 1 are readback protected
    SECURE   = 4, // Secure memory regions are protected. Non-secure debugging is available
} readback_protection_status_t;

/* Possible region 0 source. */
typedef enum
{
    NO_REGION_0 = 0,
    FACTORY,
    USER,
} region_0_source_t;

/* Possible flash region protection status. */
typedef enum
{
    PROTECTED = 0,
    UNPROTECTED
} flash_region_protection_status_t;

/* Identified device versions of nRF devices. */
typedef enum
{
    UNKNOWN = 0,

    /* nRF51 versions. */
    /* See http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf51/dita/nrf51/pdflinks/nrf51_comp_matrix.html
     */
    NRF51xxx_xxAA_REV1 = 1,
    NRF51xxx_xxAA_REV2 = 2,

    NRF51xxx_xxAA_REV3 = 3,
    NRF51xxx_xxAB_REV3 = 4,
    NRF51xxx_xxAC_REV3 = 5,

    NRF51802_xxAA_REV3 = 6,
    NRF51801_xxAB_REV3 = 17,

    /* nRF52805 versions. */
    NRF52805_xxAA_REV1   = 0x05280500,
    NRF52805_xxAA_FUTURE = 0x052805FF,

    /* nRF52810 versions. */
    /* See
       http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52/dita/nrf52/compatibility_matrix/nrf52810_comp_matrix.html
     */
    NRF52810_xxAA_REV1   = 13,
    NRF52810_xxAA_REV2   = 0x05281001,
    NRF52810_xxAA_FUTURE = 14,

    /* nRF52811 versions. */
    NRF52811_xxAA_REV1   = 0x05281100,
    NRF52811_xxAA_FUTURE = 0x052811FF,

    /* nRF52820 versions. */
    NRF52820_xxAA_REV1   = 0x05282000,
    NRF52820_xxAA_REV2   = 0x05282001,
    NRF52820_xxAA_FUTURE = 0x052820FF,

    /* nRF52832 versions. */
    /* See
       http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52/dita/nrf52/compatibility_matrix/nrf52832_comp_matrix.html
     */
    NRF52832_xxAA_ENGA   = 7,
    NRF52832_xxAA_ENGB   = 8,
    NRF52832_xxAA_REV1   = 9,
    NRF52832_xxAA_REV2   = 19,
    NRF52832_xxAA_REV3   = 0x05283201,
    NRF52832_xxAA_FUTURE = 11,

    NRF52832_xxAB_REV1   = 15,
    NRF52832_xxAB_REV2   = 20,
    NRF52832_xxAB_REV3   = 0x05283211,
    NRF52832_xxAB_FUTURE = 16,

    /* nRF52833 versions. */
    NRF52833_xxAA_REV1   = 0x05283300,
    NRF52833_xxAA_FUTURE = 0x052833FF,

    /* nRF52840 versions. */
    /* See
       http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.nrf52/dita/nrf52/compatibility_matrix/nrf52840_comp_matrix.html
     */
    NRF52840_xxAA_ENGA   = 10,
    NRF52840_xxAA_ENGB   = 21,
    NRF52840_xxAA_REV1   = 18,
    NRF52840_xxAA_REV2   = 0x05284003,
    NRF52840_xxAA_REV3   = 0x05284004,
    NRF52840_xxAA_FUTURE = 12,

    /* nRF53XXX versions */
    NRF5340_xxAA_ENGA = 0x05340000,
    NRF5340_xxAA_ENGB = 0x05340001,
    NRF5340_xxAA_ENGC = 0x05340002,
    NRF5340_xxAA_ENGD = 0x05340003,
    NRF5340_xxAA_FUTURE = 0x053400FF,

    /* NRF9160 versions. */
    NRF9160_xxAA_REV1   = 0x09160000,
    NRF9160_xxAA_REV2   = 0x09160001,
    NRF9160_xxAA_FUTURE = 0x091600FF,

} device_version_t;

/* Identified device numbers of nRF devices. */
typedef enum
{

    UNKNOWN_NAME = 0x0000000,

    NRF51xxx = 0x05100000,

    NRF51802 = 0x05180200,
    NRF51801 = 0x05180100,

    NRF52805 = 0x05280500,
    NRF52810 = 0x05281000,
    NRF52811 = 0x05281100,

    NRF52820 = 0x05282000,

    NRF52832 = 0x05283200,
    NRF52833 = 0x05283300,
    NRF52840 = 0x05284000,

    NRF5340 = 0x05340000,

    NRF9160 = 0x09160000,
} device_name_t;

/* Identified device memory of nRF devices. */
typedef enum
{

    UNKNOWN_MEM = 0,
    AA          = 1,
    AB          = 2,
    AC          = 3,
} device_memory_t;

/* Identified device revisions of nRF devices. */
typedef enum
{

    UNKNOWN_REV = 0,
    ENGA        = 10,
    ENGB        = 11,
    ENGC        = 12,
    ENGD        = 13,
    REV1        = 20,
    REV2        = 21,
    REV3        = 22,
    FUTURE      = 30,
} device_revision_t;

//****************

/* Identified types of nRF devices */
typedef enum
{
    NRF51_FAMILY   = 0,
    NRF52_FAMILY   = 1,
    NRF53_FAMILY   = 53,
    NRF91_FAMILY   = 91,
    UNKNOWN_FAMILY = 99
} device_family_t;

/* Possible rtt channel directions */
typedef enum
{
    UP_DIRECTION   = 0,
    DOWN_DIRECTION = 1
} rtt_direction_t;

/* QSPI erase length */
typedef enum
{
    ERASE4KB  = 0,
    ERASE32KB = 3,
    ERASE64KB = 1,
    ERASEALL  = 2
} qspi_erase_len_t;

/* QSPI read and write modes */
typedef enum
{
    FASTREAD = 0,
    READ2O   = 1,
    READ2IO  = 2,
    READ4O   = 3,
    READ4IO  = 4
} qspi_read_mode_t;

typedef enum
{
    PP    = 0,
    PP2O  = 1,
    PP4O  = 2,
    PP4IO = 3
} qspi_write_mode_t;

typedef enum
{
    BIT24 = 0,
    BIT32 = 1
} qspi_address_mode_t;

/* QSPI frequency */
typedef enum
{
    M2  = 15,
    M4  = 7,
    M8  = 3,
    M16 = 1,
    M32 = 0,
    M64 = -1,
    M96 = -2
} qspi_frequency_t;

/* QSPI SPI mode */
typedef enum
{
    MODE0 = 0,
    MODE3 = 1
} qspi_spi_mode_t;

/* QSPI custom level IO2 and IO3 */
typedef enum
{
    LEVEL_HIGH = 1,
    LEVEL_LOW  = 0
} qspi_custom_level_io_t;

/* QSPI custom PPSize */
typedef enum
{
    PAGE256 = 0,
    PAGE512 = 1
} qspi_page_program_size_t;

/* QSPI initialization  */
typedef struct
{
    qspi_read_mode_t read_mode;
    qspi_write_mode_t write_mode;
    qspi_address_mode_t address_mode;
    qspi_frequency_t frequency;
    qspi_spi_mode_t spi_mode;
    uint32_t sck_delay;
    qspi_custom_level_io_t custom_instruction_io2_level;
    qspi_custom_level_io_t custom_instruction_io3_level;
    uint32_t CSN_pin;
    uint32_t CSN_port;
    uint32_t SCK_pin;
    uint32_t SCK_port;
    uint32_t DIO0_pin;
    uint32_t DIO0_port;
    uint32_t DIO1_pin;
    uint32_t DIO1_port;
    uint32_t DIO2_pin;
    uint32_t DIO2_port;
    uint32_t DIO3_pin;
    uint32_t DIO3_port;
    uint32_t WIP_index;
    qspi_page_program_size_t pp_size;
} qspi_init_params_t;

typedef enum
{
    CP_APPLICATION = 0,
    CP_MODEM       = 1,
    CP_NETWORK     = 2,
} coprocessor_t;

/* Every DLL function has either a void or nrfjprogdll_err_t return type. */
typedef enum
{
    SUCCESS = 0,

    /* PC Issues */
    OUT_OF_MEMORY = -1,

    /* Wrong use of dll errors. */
    INVALID_OPERATION            = -2,
    INVALID_PARAMETER            = -3,
    INVALID_DEVICE_FOR_OPERATION = -4,
    WRONG_FAMILY_FOR_DEVICE      = -5,
    UNKNOWN_DEVICE               = -6,

    /* Connection issues. */
    EMULATOR_NOT_CONNECTED = -10,
    CANNOT_CONNECT         = -11,
    LOW_VOLTAGE            = -12,
    NO_EMULATOR_CONNECTED  = -13,

    /* Device issues. */
    NVMC_ERROR     = -20,
    RECOVER_FAILED = -21,

    /* Operation not available. */
    NOT_AVAILABLE_BECAUSE_PROTECTION           = -90,
    NOT_AVAILABLE_BECAUSE_MPU_CONFIG           = -91,
    NOT_AVAILABLE_BECAUSE_COPROCESSOR_DISABLED = -92,
    NOT_AVAILABLE_BECAUSE_TRUST_ZONE           = -93,
    NOT_AVAILABLE_BECAUSE_BPROT                = -94,

    /* JlinkARM DLL errors. */
    JLINKARM_DLL_NOT_FOUND           = -100,
    JLINKARM_DLL_COULD_NOT_BE_OPENED = -101,
    JLINKARM_DLL_ERROR               = -102,
    JLINKARM_DLL_TOO_OLD             = -103,
    JLINKARM_DLL_READ_ERROR          = -104,

    /* UART DFU errors */
    SERIAL_PORT_NOT_FOUND        = -110,
    SERIAL_PORT_PERMISSION_ERROR = -111,
    SERIAL_PORT_WRITE_ERROR      = -112,
    SERIAL_PORT_READ_ERROR       = -113,
    SERIAL_PORT_RESOURCE_ERROR   = -114,
    SERIAL_PORT_NOT_OPEN_ERROR   = -115,

    /* nrfjprog sub DLL errors. */
    NRFJPROG_SUB_DLL_NOT_FOUND                = -150,
    NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED      = -151,
    NRFJPROG_SUB_DLL_COULD_NOT_LOAD_FUNCTIONS = -152,

    /* High Level DLL */
    VERIFY_ERROR          = -160,
    RAM_IS_OFF_ERROR      = -161,
    FILE_OPERATION_FAILED = -162,

    /* DFU errors */
    TIME_OUT  = -220,
    DFU_ERROR = -221,

    /* Internal Error */
    INTERNAL_ERROR = -254,

    /* Not implemented. */
    NOT_IMPLEMENTED_ERROR = -255,

} nrfjprogdll_err_t;

typedef enum
{
    critical = 50,
    error    = 40,
    warning  = 30,
    info     = 20,
    debug    = 10,
    none     = 0,
} nrfjprogdll_log_level;

/* Expected log function prototype for logging operations. */
typedef void msg_callback(const char * msg_str);


#if defined(__cplusplus)
}
#endif

#endif /* DLL_COMMON_DEFINITIONS_H */
