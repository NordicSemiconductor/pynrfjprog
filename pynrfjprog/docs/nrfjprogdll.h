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

#ifndef NRFJPROGDLL_FUNC_H
#define NRFJPROGDLL_FUNC_H


#include <stdint.h>
#include "DllCommonDefinitions.h"


#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief   Returns the JLinkARM.dll version.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_dll_version(uint32_t * major, uint32_t * minor, char * revision);


/**
 * @brief   Checks if the JLinkARM DLL is open.
 *
 * @details Checks if the NRFJPROG_open_dll() function has been called since the last call to NRFJPROG_close_dll() or since the loading of this dll.
 *
 * @param   opened                              Pointer of the location to store the result.
 *
 * @retval  SUCCESS
 * @retval  INVALID_PARAMETER                   The opened parameter is NULL.
 */
nrfjprogdll_err_t NRFJPROG_is_dll_open(bool * opened);


/**
 * @brief   Opens the JLinkARM DLL and sets the log callback. Prepares the dll for work with an nRF device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_open_dll(const char * jlink_path, msg_callback * cb, device_family_t family);


/**
 * @brief   Closes and frees the JLinkARM DLL.
 *
 * @details For further details, see the device family header file.
 */
void NRFJPROG_close_dll(void);


/**
 * @brief   Enumerates all comports connected to a  given Segger debug probe
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_enum_emu_com(const uint32_t serial_number, com_port_info_t com_ports[], const uint32_t com_ports_len, uint32_t * num_com_ports);


/**
 * @brief   Enumerates the serial numbers of connected USB J-Link emulators.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_enum_emu_snr(uint32_t serial_numbers[], uint32_t serial_numbers_len, uint32_t * num_available);


/**
 * @brief   Checks if the emulator has an established connection with Segger emulator/debugger.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_emu(bool * is_pc_connected_to_emu);


/**
 * @brief   Connects to a given emulator/debugger.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_with_snr(uint32_t serial_number, uint32_t clock_speed_in_khz);


/**
 * @brief   Connects to an emulator/debugger.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_emu_without_snr(uint32_t clock_speed_in_khz);


 /**
  * @brief   Attempts to reset the selected J-Link OB.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_reset_connected_emu(void);


/**
 * @brief   Replaces the firmware on the selected J-Link debug probe.
*
* @details For further details, see the device family header file.
*/
nrfjprogdll_err_t NRFJPROG_replace_connected_emu_fw(void);


/**
 * @brief   Reads the serial number of the emulator connected to.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_connected_emu_snr(uint32_t * serial_number);


/**
* @brief   Reads the firmware identification string of the emulator connected to.
*
* @details For further detalis. see the device family header file.
*/
nrfjprogdll_err_t NRFJPROG_read_connected_emu_fwstr(char * buffer, uint32_t buffer_size);


/**
 * @brief   Disconnects from an emulator.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_disconnect_from_emu(void);


/**
 * @brief   Checks if the modem is held in reset.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_coprocessor_enabled(coprocessor_t coprocessor, bool * is_coprocessor_enabled);


/**
 * @brief   Releases the modem core from reset.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_enable_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Holds the modem core into reset.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_disable_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Selects the default access port.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_select_coprocessor(coprocessor_t coprocessor);


/**
 * @brief   Recovers the device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_recover(void);


/**
 * @brief   Checks if the connected emulator has an established connection with an nRF device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_connected_to_device(bool * is_emu_connected_to_device);


/**
 * @brief   Connects to the nRF device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_connect_to_device(void);


/**
 * @brief   Disonnects from the nRF device.
 *
 * @details For further details, see the device family header file. 
*/
nrfjprogdll_err_t NRFJPROG_disconnect_from_device(void);


/**
 * @brief   Protects the device against read or debug.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_readback_protect(readback_protection_status_t desired_protection);


/**
 * @brief   Returns the status of the readback protection.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_readback_status(readback_protection_status_t * status);


/**
 * @brief   Returns the region 0 size and source of protection if any for nRF51 devices.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_region_0_size_and_source(uint32_t * size, region_0_source_t * source);


/**
 * @brief   Executes a soft reset using the CTRL-AP for nRF52 and onward devices.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_debug_reset(void);


/**
 * @brief   Executes a system reset request.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_sys_reset(void);


/**
 * @brief   Executes a pin reset.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_pin_reset(void);


/**
 * @brief   Disables BPROT, ACL or NVM protection blocks where appropriate depending on device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_disable_bprot(void);


/**
 * @brief   Detects if memory block protection is enabled.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_bprot_enabled(bool * bprot_enabled, uint32_t address_start, uint32_t length);


/**
 * @brief   Erases all code and UICR flash.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_erase_all(void);


/**
 * @brief   Erases a page of code flash.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_erase_page(uint32_t addr);


/**
 * @brief   Erases UICR info page.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_erase_uicr(void);


/**
 * @brief   Writes one uint32_t data into the given address.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_write_u32(uint32_t addr, uint32_t data, bool nvmc_control);


/**
 * @brief   Reads one uint32_t from the given address.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_u32(uint32_t addr, uint32_t * data);


/**
 * @brief   Writes data from the array into the device starting at the given address.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_write(uint32_t addr, const uint8_t * data, uint32_t data_len, bool nvmc_control);


/**
 * @brief   Reads data_len bytes from the device starting at the given address.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Checks if the device CPU is halted.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_halted(bool * is_device_halted);


/**
 * @brief   Halts the device CPU.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_halt(void);


/**
 * @brief   Starts the device CPU with the given pc and sp.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_run(uint32_t pc, uint32_t sp);


/**
 * @brief   Starts the device CPU.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_go(void);


/**
 * @brief   Runs the device CPU for one instruction.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_step(void);


/**
 * @brief   Reads the number of RAM sections in the device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_count(uint32_t * ram_sections_count);


/**
 * @brief   Reads the size in bytes of the RAM sections in the device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_size(uint32_t * ram_sections_size, uint32_t ram_sections_size_len);


/**
 * @brief   Reads the RAM sections power status.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_ram_sections_power_status(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_len);


/**
 * @brief   DEPRECATED. Use NRFJPROG_read_ram_sections_power_status, NRFJPROG_read_ram_sections_size or NRFJPROG_read_ram_sections_count instead.
 *
 * @details Reads the RAM power status. For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_ram_powered(ram_section_power_status_t * ram_sections_power_status, uint32_t ram_sections_power_status_array_size, uint32_t * ram_sections_number, uint32_t * ram_sections_size);


/**
 * @brief   Powers up all RAM sections of the device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_power_ram_all(void);


/**
 * @brief   Powers down a RAM section of the device.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_unpower_ram_section(uint32_t section_index);


/**
 * @brief   Reads a CPU register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_cpu_register(cpu_registers_t register_name, uint32_t * register_value);


/**
 * @brief   Writes a CPU register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_write_cpu_register(cpu_registers_t register_name, uint32_t register_value);


/**
 * @brief   Reads the version of the device connected to the emulator.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_device_version(device_version_t * version);


/**
 * @brief   Reads the version, name, memory and revision of the device connected to the emulator.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_device_info(device_version_t * version, device_name_t * name, device_memory_t * memory, device_revision_t * revision);


/**
 * @brief   Reads the family of the device connected to the emulator. Can only be called if NRFJPROG_open_dll() was called with UNKNOWN_FAMILY as family parameter.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_device_family(device_family_t * family);


/**
 * @brief   Reads a debug port register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_debug_port_register(uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debug port register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_write_debug_port_register(uint8_t reg_addr, uint32_t data);


/**
 * @brief   Reads a debugger access port register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_read_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t * data);


/**
 * @brief   Writes a debugger access port register.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_write_access_port_register(uint8_t ap_index, uint8_t reg_addr, uint32_t data);


/**
 * @brief   Checks if the RTT is started.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_rtt_started(bool * started);


/**
 * @brief   Indicates to the dll the location of the RTT control block in the device memory.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_set_control_block_address(uint32_t address);


/**
 * @brief   Starts RTT.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_start(void);


/**
 * @brief   Checks if RTT control block has been found.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_is_control_block_found(bool * is_control_block_found);


/**
 * @brief   Stops RTT.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_stop(void);


/**
 * @brief   Reads from an RTT channel.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read(uint32_t up_channel_index, char * data, uint32_t data_len, uint32_t * data_read);


/**
 * @brief   Writes to an RTT channel.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_write(uint32_t down_channel_index, const char * data, uint32_t data_len, uint32_t * data_written);


/**
 * @brief   Gets the number of RTT channels.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_count(uint32_t * down_channel_number, uint32_t * up_channel_number);


/**
 * @brief   Reads the info from one RTT channel.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_rtt_read_channel_info(uint32_t channel_index, rtt_direction_t dir, char * channel_name, uint32_t channel_name_len, uint32_t * channel_size);


/**
 * @brief   Checks if the QSPI peripheral is initialized.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_is_qspi_init(bool * initialized);


/**
 * @brief   Initializes the QSPI peripheral.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_init(bool retain_ram, const qspi_init_params_t * init_params);


/**
 * @brief   Uninitializes the QSPI peripheral.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_uninit(void);


/**
 * @brief   Reads from the QSPI-connected external memory.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_read(uint32_t addr, uint8_t * data, uint32_t data_len);


/**
 * @brief   Writes to the QSPI-connected external memory.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_write(uint32_t addr, const uint8_t * data, uint32_t data_len);


/**
 * @brief   Erases the QSPI-connected external memory.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_erase(uint32_t addr, qspi_erase_len_t length);


/**
 * @brief   Sends a custom instruction to the QSPI-connected external memory.
 *
 * @details For further details, see the device family header file.
 */
nrfjprogdll_err_t NRFJPROG_qspi_custom(uint8_t instruction_code,
                                       uint32_t instruction_length,
                                       const uint8_t * data_in,
                                       uint8_t * data_out);


#if defined(__cplusplus)
}
#endif

#endif  /* NRFJPROGDLL_FUNC_H */
