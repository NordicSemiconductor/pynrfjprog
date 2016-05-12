"""
This module serves as a Python wrapper around the nrfjprog DLL.

Note: Please look at the nrfjprogdll.h file provided with the tools for a more elaborate description of the API functions and their side effects.
"""

from __future__ import print_function

from builtins import int

import ctypes
import enum
import os
import sys

try:
    from . import JLink
except Exception:
    import JLink

"""
Deprecated: Do not use, use log parameter in API constructor instead.
"""
DEBUG_OUTPUT = False


@enum.unique
class DeviceFamily(enum.IntEnum):
    """
    Wraps device_family_t values from DllCommonDefinitions.h

    """
    NRF51              = 0
    NRF52              = 1

@enum.unique
class DeviceVersion(enum.IntEnum):
    """
    Wraps device_version_t values from DllCommonDefinitions.h

    """
    UNKNOWN                   = 0
    NRF51_XLR1                = 1
    NRF51_XLR2                = 2
    NRF51_XLR3                = 3
    NRF51_L3                  = 4
    NRF51_XLR3P               = 5
    NRF51_XLR3LC              = 6
    NRF52_FP1_ENGA            = 7
    NRF52_FP1_ENGB            = 8
    NRF52_FP1                 = 9

@enum.unique
class NrfjprogdllErr(enum.IntEnum):
    """
    Wraps nrfjprogdll_err_t values from DllCommonDefinitions.h

    """
    SUCCESS                                     =  0
    OUT_OF_MEMORY                               = -1
    INVALID_OPERATION                           = -2
    INVALID_PARAMETER                           = -3
    INVALID_DEVICE_FOR_OPERATION                = -4
    WRONG_FAMILY_FOR_DEVICE                     = -5
    EMULATOR_NOT_CONNECTED                      = -10
    CANNOT_CONNECT                              = -11
    LOW_VOLTAGE                                 = -12
    NO_EMULATOR_CONNECTED                       = -13
    NVMC_ERROR                                  = -20
    NOT_AVAILABLE_BECAUSE_PROTECTION            = -90
    JLINKARM_DLL_NOT_FOUND                      = -100
    JLINKARM_DLL_COULD_NOT_BE_OPENED            = -101
    JLINKARM_DLL_ERROR                          = -102
    JLINKARM_DLL_TOO_OLD                        = -103
    NRFJPROG_SUB_DLL_NOT_FOUND                  = -150
    NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED        = -151
    NOT_IMPLEMENTED_ERROR                       = -255

@enum.unique
class ReadbackProtection(enum.IntEnum):
    """
    Wraps readback_protection_status_t values from DllCommonDefinitions.h

    """
    NONE                       = 0
    REGION_0                   = 1
    ALL                        = 2
    BOTH                       = 3

@enum.unique
class Region0Source(enum.IntEnum):
    """
    Wraps region_0_source_t values from DllCommonDefinitions.h

    """
    NO_REGION_0                = 0
    FACTORY                    = 1
    USER                       = 2

@enum.unique
class RamPower(enum.IntEnum):
    """
    Wraps ram_power_status_t values from DllCommonDefinitions.h

    """
    OFF                    = 0
    ON                     = 1

@enum.unique
class RTTChannelDirection(enum.IntEnum):
    """
    Wraps rtt_direction_t values from DllCommonDefinitions.h

    """
    UP_DIRECTION           = 0
    DOWN_DIRECTION         = 1

@enum.unique
class CpuRegister(enum.IntEnum):
    """
    Wraps cpu_registers_t values from DllCommonDefinitions.h

    """
    R0                        = 0
    R1                        = 1
    R2                        = 2
    R3                        = 3
    R4                        = 4
    R5                        = 5
    R6                        = 6
    R7                        = 7
    R8                        = 8
    R9                        = 9
    R10                       = 10
    R11                       = 11
    R12                       = 12
    R13                       = 13
    R14                       = 14
    R15                       = 15
    XPSR                      = 16
    MSP                       = 17
    PSP                       = 18


class APIError(Exception):
    """
    nrfjprog DLL exception class, inherits from the built-in Exception class.

    """

    def __init__(self, err_code=None):
        """
        Constructs a new object and sets the err_code.

        @param int err_code: The error code returned by the nrfjprog DLL.
        """
        self.err_code = err_code
        if self.err_code in [member.value for name, member in NrfjprogdllErr.__members__.items()]:
            err_str = 'An error was reported by NRFJPROG DLL: {} {}.'.format(self.err_code, NrfjprogdllErr(self.err_code).name)
        else:
            err_str = 'An error was reported by NRFJPROG DLL: {}.'.format(self.err_code)

        Exception.__init__(self, err_str)


class API(object):
    """
    Main class of the module. Instance the class to get access to nrfjprog.dll functions in Python.

    Note: A copy of nrfjprog.dll must be found in the working directory.
    """

    _DEFAULT_JLINK_SPEED_KHZ = 2000

    def __init__(self, device_family, jlink_arm_dll_path=None, log_str_cb=None, log=False, log_str=None, log_file_path=None):
        """
        Constructor.

        @param enum, string or int device_family: The series of device pynrfjprog will interact with.
        @param (optional) string jlink_arm_dll_path: Absolute path to the JLinkARM DLL that you want nrfjprog to use.
        @param (optional) callable object log_str_cb: If present, the log_str_cb will be called to receive log and error information. The log_str_cb object should be callable, expect to receive a string as the only parameter and do not need to return anything.
        @param (optional) bool log: If present and true, will enable logging to sys.stderr with the default log string appended to the beginning of each debug output line.
        @param (optional) string log_str: If present, will enable logging to sys.stderr with overwriten default log string appended to the beginning of each debug output line.
        @param (optional) string log_file_path: If present, will enable logging to log_file specified. This file will be opened in write mode in API.__init__() and closed when api.close() is called.
        """
        self._device_family = None
        self._jlink_arm_dll_path = None
        self._lib = None
        self._log_str_cb = None
        self._log_file = None

        self._device_family = self._decode_enum(device_family, DeviceFamily)
        if self._device_family is None:
            raise ValueError('Parameter device_family must be of type int, str or DeviceFamily enumeration.')

        if jlink_arm_dll_path is None:
            jlink_arm_dll_path = JLink.find_latest_dll()
            if jlink_arm_dll_path is None:
                raise RuntimeError('Could not locate a JLinkARM.dll in the default SEGGER installation path.')
        else:
            if not isinstance(jlink_arm_dll_path, str):
                raise ValueError('Parameter jlink_arm_dll_path must be a string.')

        self._jlink_arm_dll_path = os.path.abspath(jlink_arm_dll_path).encode('ascii')

        if DEBUG_OUTPUT:
            log = True
        self._log_str_cb = self._generate_log_str_cb(log_str_cb, log, log_str, log_file_path)

        this_dir, this_file = os.path.split(__file__)

        if sys.platform.lower().startswith('win'):
            nrfjprog_dll_folder = 'win_dll'
            nrfjprog_dll_name = 'nrfjprog.dll'
        elif sys.platform.lower().startswith('linux'):
            if sys.maxsize > 2**32:
                nrfjprog_dll_folder = 'linux_64bit_so'
            else:
                nrfjprog_dll_folder = 'linux_32bit_so'
            nrfjprog_dll_name = 'libnrfjprogdll.so'
        elif sys.platform.startswith('dar'):
            nrfjprog_dll_folder = 'osx_dylib'
            nrfjprog_dll_name = 'libnrfjprogdll.dylib'

        nrfjprog_dll_path = os.path.join(os.path.abspath(this_dir), nrfjprog_dll_folder, nrfjprog_dll_name)

        if not os.path.exists(nrfjprog_dll_path):
            raise RuntimeError("Failed to locate the NRFJPROG DLL in the pynrfjprog directory.")

        try:
            self._lib = ctypes.cdll.LoadLibrary(nrfjprog_dll_path)
        except Exception as error:
            raise RuntimeError("Could not load the NRFJPROG DLL: '{}'.".format(error))

    """
    nrfjprog.DLL functions.

    """
    def dll_version(self):
        """
        Returns the JLinkARM.dll version.

        @return (int, int, str): Tuple containing the major, minor and revision of the dll.
        """
        major = ctypes.c_uint32()
        minor = ctypes.c_uint32()
        revision = ctypes.c_uint8()

        result = self._lib.NRFJPROG_dll_version(ctypes.byref(major), ctypes.byref(minor), ctypes.byref(revision))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return major.value, minor.value, chr(revision.value)

    def open(self):
        """
        Opens the JLinkARM.dll and sets the log callback. Prepares the dll for work with an nRF5x device.

        """
        # No need to encode self._jlink_arm_dll_path since it is an ASCII string and that is what is expected by ctypes.
        # Function self._log_str_cb has already been encoded in __init__() function.
        device_family = ctypes.c_int(self._device_family.value)

        result = self._lib.NRFJPROG_open_dll(self._jlink_arm_dll_path, self._log_str_cb, device_family)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def close(self):
        """
        Closes and frees the JLinkARM DLL.

        """
        self._lib.NRFJPROG_close_dll()
        if self._log_file is not None and self._log_file is not sys.stderr and self._log_file is not sys.stdout:
            self._log_file.close()

    def enum_emu_snr(self):
        """
        Enumerates the serial numbers of connected USB J-Link emulators.

        @return [int]: A list with the serial numbers.
        """
        serial_numbers_len = ctypes.c_uint32(127)
        serial_numbers = (ctypes.c_uint32 * serial_numbers_len.value)()
        num_available = ctypes.c_uint32()

        result = self._lib.NRFJPROG_enum_emu_snr(ctypes.byref(serial_numbers), serial_numbers_len, ctypes.byref(num_available))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        snr = [int(serial_numbers[i]) for i in range(0, min(num_available.value, serial_numbers_len.value))]

        if len(snr) == 0:
            return None
        else:
            return snr

    def is_connected_to_emu(self):
        """
        Checks if the emulator has an established connection with Segger emulator/debugger.

        @return boolean: True if connected.
        """
        is_connected_to_emu = ctypes.c_bool()

        result = self._lib.NRFJPROG_is_connected_to_emu(ctypes.byref(is_connected_to_emu))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return is_connected_to_emu.value

    def connect_to_emu_with_snr(self, serial_number, jlink_speed_khz=_DEFAULT_JLINK_SPEED_KHZ):
        """
        Connects to a given emulator/debugger.

        @param int serial_number: Serial number of the emulator to connect to.
        @param int jlink_speed_khz: SWDCLK speed [kHz].
        """
        if not self._is_u32(serial_number):
            raise ValueError('The serial_number parameter must be an unsigned 32-bit value.')

        if not self._is_u32(jlink_speed_khz):
            raise ValueError('The jlink_speed_khz parameter must be an unsigned 32-bit value.')

        serial_number = ctypes.c_uint32(serial_number)
        jlink_speed_khz = ctypes.c_uint32(jlink_speed_khz)

        result = self._lib.NRFJPROG_connect_to_emu_with_snr(serial_number, jlink_speed_khz)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def connect_to_emu_without_snr(self, jlink_speed_khz=_DEFAULT_JLINK_SPEED_KHZ):
        """
        Connects to an emulator/debugger.

        @param int jlink_speed_khz: SWDCLK speed [kHz].
        """
        if not self._is_u32(jlink_speed_khz):
            raise ValueError('The jlink_speed_khz parameter must be an unsigned 32-bit value.')

        jlink_speed_khz = ctypes.c_uint32(jlink_speed_khz)

        result = self._lib.NRFJPROG_connect_to_emu_without_snr(jlink_speed_khz)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def disconnect_from_emu(self):
        """
        Disconnects from an emulator.

        """
        result = self._lib.NRFJPROG_disconnect_from_emu()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def recover(self):
        """
        Recovers the device.

        """
        result = self._lib.NRFJPROG_recover()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def is_connected_to_device(self):
        """
        Checks if the connected emulator has an established connection with an nRF device.

        @return boolean: True if connected.
        """
        is_connected_to_device = ctypes.c_bool()

        result = self._lib.NRFJPROG_is_connected_to_device(ctypes.byref(is_connected_to_device))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return is_connected_to_device.value

    def connect_to_device(self):
        """
        Connects to the nRF device.

        """
        result = self._lib.NRFJPROG_connect_to_device()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def readback_protect(self, desired_protection_level):
        """
        Protects the device against read or debug.

        @param int, string, or ReadbackProtection(IntEnum) desired_protection_level: Readback protection level for the target.
        """
        if not self._is_enum(desired_protection_level, ReadbackProtection):
            raise ValueError('Parameter desired_protection_level must be of type int, str or ReadbackProtection enumeration.')

        desired_protection_level = self._decode_enum(desired_protection_level, ReadbackProtection)
        if desired_protection_level is None:
            raise ValueError('Parameter desired_protection_level must be of type int, str or ReadbackProtection enumeration.')

        desired_protection_level = ctypes.c_int(desired_protection_level.value)

        result = self._lib.NRFJPROG_readback_protect(desired_protection_level)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def readback_status(self):
        """
        Returns the status of the readback protection.

        @return string: Readback protection level of the target.
        """
        status = ctypes.c_int()

        result = self._lib.NRFJPROG_readback_status(ctypes.byref(status))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return ReadbackProtection(status.value).name

    def read_region_0_size_and_source(self):
        """
        Returns the region 0 size and source of protection if any for nRF51 devices.

        @return (int, string): Region size and configuration source of protection (either UICR of FICR).
        """
        size = ctypes.c_uint32()
        source = ctypes.c_int()

        result = self._lib.NRFJPROG_read_region_0_size_and_source(ctypes.byref(size), ctypes.byref(source))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return size.value, Region0Source(source.value).name

    def debug_reset(self):
        """
        Executes a soft reset using the CTRL-AP for nRF52 and onward devices.

        """
        result = self._lib.NRFJPROG_debug_reset()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def sys_reset(self):
        """
        Executes a system reset request.

        """
        result = self._lib.NRFJPROG_sys_reset()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def pin_reset(self):
        """
        Executes a pin reset.

        """
        result = self._lib.NRFJPROG_pin_reset()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def disable_bprot(self):
        """
        Disables BPROT or NVM protection blocks for nRF51 devices.

        """
        result = self._lib.NRFJPROG_disable_bprot()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def erase_all(self):
        """
        Erases all code and UICR flash.

        """
        result = self._lib.NRFJPROG_erase_all()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def erase_page(self, addr):
        """
        Erases a page of code flash.

        @param int addr: Start address of the page in code flash to erase.
        """
        if not self._is_u32(addr):
            raise ValueError('The addr parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint32(addr)

        result = self._lib.NRFJPROG_erase_page(addr)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def erase_uicr(self):
        """
        Erases UICR info page.

        """
        result = self._lib.NRFJPROG_erase_uicr()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def write_u32(self, addr, data, control):
        """
        Writes one uint32_t data into the given address.

        @param int addr: Address to write.
        @param int data: Value to write.
        @param boolean control: True for automatic control of NVMC by the function.
        """
        if not self._is_u32(addr):
            raise ValueError('The addr parameter must be an unsigned 32-bit value.')

        if not self._is_u32(data):
            raise ValueError('The data parameter must be an unsigned 32-bit value.')

        if not self._is_bool(control):
            raise ValueError('The control parameter must be a boolean value.')

        addr = ctypes.c_uint32(addr)
        data = ctypes.c_uint32(data)
        control = ctypes.c_bool(control)

        result = self._lib.NRFJPROG_write_u32(addr, data, control)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def read_u32(self, addr):
        """
        Reads one uint32_t from the given address.

        @param  int addr: Address to read.
        @return int: Value read.
        """
        if not self._is_u32(addr):
            raise ValueError('The addr parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint32(addr)
        data = ctypes.c_uint32()

        result = self._lib.NRFJPROG_read_u32(addr, ctypes.byref(data))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return data.value

    def write(self, addr, data, control):
        """
        Writes data from the array into the device starting at the given address.

        @param int addr: Start address of the memory block to write.
        @param [int] data: List of values to write.
        @param boolean control: True for automatic control of NVMC by the function.
        """
        if not self._is_u32(addr):
            raise ValueError('The addr parameter must be an unsigned 32-bit value.')

        if not self._is_valid_buf(data):
            raise ValueError('The data parameter must be a tuple or a list with at least one item.')

        if not self._is_bool(control):
            raise ValueError('The control parameter must be a boolean value.')

        addr = ctypes.c_uint32(addr)
        data_len = ctypes.c_uint32(len(data))
        data = (ctypes.c_uint8 * data_len.value)(*data)
        control = ctypes.c_bool(control)

        result = self._lib.NRFJPROG_write(addr, ctypes.byref(data), data_len, control)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def read(self, addr, data_len):
        """
        Reads data_len bytes from the device starting at the given address.

        @param int addr: Start address of the memory block to read.
        @param int data_len: Number of bytes to read.
        @return [int]: List of values read.
        """
        if not self._is_u32(addr):
            raise ValueError('The addr parameter must be an unsigned 32-bit value.')

        if not self._is_u32(data_len):
            raise ValueError('The data_len parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint32(addr)
        data_len = ctypes.c_uint32(data_len)
        data = (ctypes.c_uint8 * data_len.value)()

        result = self._lib.NRFJPROG_read(addr, ctypes.byref(data), data_len)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return list(data)

    def is_halted(self):
        """
        Checks if the device CPU is halted.

        @return boolean: True if halted.
        """
        is_halted = ctypes.c_bool()

        result = self._lib.NRFJPROG_is_halted(ctypes.byref(is_halted))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return is_halted.value

    def halt(self):
        """
        Halts the device CPU.

        """
        result = self._lib.NRFJPROG_halt()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def run(self, pc, sp):
        """
        Starts the device CPU with the given pc and sp.

        @param int pc: Value for the program counter.
        @param int sp: Value for the stack pointer.
        """
        if not self._is_u32(pc):
            raise ValueError('The pc parameter must be an unsigned 32-bit value.')

        if not self._is_u32(sp):
            raise ValueError('The sp parameter must be an unsigned 32-bit value.')

        pc = ctypes.c_uint32(pc)
        sp = ctypes.c_uint32(sp)

        result = self._lib.NRFJPROG_run(pc, sp)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def go(self):
        """
        Starts the device CPU.

        """
        result = self._lib.NRFJPROG_go()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def step(self):
        """
        Runs the device CPU for one instruction.

        """
        result = self._lib.NRFJPROG_step()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def is_ram_powered(self):
        """
        Reads the RAM power status.

        @return (string, int, int): Tuple containing three elements, a list of the power status of each RAM block, the number of RAM blocks in the device and the size of the RAM blocks in the device.
        """
        status_size = ctypes.c_uint32(64)
        status = (ctypes.c_uint32 * status_size.value)()
        number = ctypes.c_uint32()
        size = ctypes.c_uint32()

        result = self._lib.NRFJPROG_is_ram_powered(ctypes.byref(status), status_size, ctypes.byref(number), ctypes.byref(size))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return [RamPower(elem).name for elem in list(status)[0:min(number.value, status_size.value)]], number.value, size.value

    def power_ram_all(self):
        """
        Powers up all RAM sections of the device.

        """
        result = self._lib.NRFJPROG_power_ram_all()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def unpower_ram_section(self, section_index):
        """
        Powers down a RAM section of the device.

        @param int section_index: RAM block index to power off.
        """
        if not self._is_u32(section_index):
            raise ValueError('The section_index parameter must be an unsigned 32-bit value.')

        section_index = ctypes.c_uint32(section_index)

        result = self._lib.NRFJPROG_unpower_ram_section(section_index)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def read_cpu_register(self, register_name):
        """
        Reads a CPU register.

        @param  int, string, or CPURegister(IntEnum) register_name: CPU register to read.
        @return int: Value read.
        """
        if not self._is_enum(register_name, CpuRegister):
            raise ValueError('Parameter register_name must be of type int, str or CpuRegister enumeration.')

        register_name = self._decode_enum(register_name, CpuRegister)
        if register_name is None:
            raise ValueError('Parameter register_name must be of type int, str or CpuRegister enumeration.')

        register_name = ctypes.c_int(register_name.value)
        value = ctypes.c_uint32()

        result = self._lib.NRFJPROG_read_cpu_register(register_name, ctypes.byref(value))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return value.value

    def write_cpu_register(self, register_name, value):
        """
        Writes a CPU register.

        @param int, string, or CPURegister(IntEnum) register_name: CPU register to write.
        @param int value: Value to write.
        """
        if not self._is_u32(value):
            raise ValueError('The value parameter must be an unsigned 32-bit value.')

        if not self._is_enum(register_name, CpuRegister):
            raise ValueError('Parameter register_name must be of type int, str or CpuRegister enumeration.')

        register_name = self._decode_enum(register_name, CpuRegister)
        if register_name is None:
            raise ValueError('Parameter register_name must be of type int, str or CpuRegister enumeration.')

        register_name = ctypes.c_int(register_name.value)
        value = ctypes.c_uint32(value)

        result = self._lib.NRFJPROG_write_cpu_register(register_name, value)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def read_device_version(self):
        """
        Reads the version of the device connected to the emulator.

        @return string: Version of the target device.
        """
        version = ctypes.c_int()

        result = self._lib.NRFJPROG_read_device_version(ctypes.byref(version))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return DeviceVersion(version.value).name

    def read_debug_port_register(self, addr):
        """
        Reads a debug port register.

        @param int addr: Address to read.
        @return int: Value read.
        """
        if not self._is_u8(addr):
            raise ValueError('The reg_addr parameter must be an unsigned 8-bit value.')

        addr = ctypes.c_uint8(addr)
        data = ctypes.c_uint32()

        result = self._lib.NRFJPROG_read_debug_port_register(addr, ctypes.byref(data))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return data.value

    def write_debug_port_register(self, addr, data):
        """
        Writes a debug port register.

        @param int addr: Address to write.
        @param int data: Value to write.
        """
        if not self._is_u8(addr):
            raise ValueError('The reg_addr parameter must be an unsigned 8-bit value.')

        if not self._is_u32(data):
            raise ValueError('The data parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint8(addr)
        data = ctypes.c_uint32(data)

        result = self._lib.NRFJPROG_write_debug_port_register(addr, data)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def read_access_port_register(self, ap_index, addr):
        """
        Reads a debugger access port register.

        @param int ap_index: Index of the access port to read.
        @param int addr: Address to read.
        @return int: Value read.
        """
        if not self._is_u8(ap_index):
            raise ValueError('The ap_index parameter must be an unsigned 8-bit value.')

        if not self._is_u8(addr):
            raise ValueError('The reg_addr parameter must be an unsigned 8-bit value.')

        ap_index = ctypes.c_uint8(ap_index)
        addr = ctypes.c_uint8(addr)
        data = ctypes.c_uint32()

        result = self._lib.NRFJPROG_read_access_port_register(ap_index, addr, ctypes.byref(data))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return data.value

    def write_access_port_register(self, ap_index, addr, data):
        """
        Writes a debugger access port register.

        @param int ap_index: Index of the access port to write.
        @param int addr: Address to write.
        @param int data: Value to write.
        """
        if not self._is_u8(ap_index):
            raise ValueError('The ap_index parameter must be an unsigned 8-bit value.')

        if not self._is_u8(addr):
            raise ValueError('The reg_addr parameter must be an unsigned 8-bit value.')

        if not self._is_u32(data):
            raise ValueError('The data parameter must be an unsigned 32-bit value.')

        ap_index = ctypes.c_uint8(ap_index)
        addr = ctypes.c_uint8(addr)
        data = ctypes.c_uint32(data)

        result = self._lib.NRFJPROG_write_access_port_register(ap_index, addr, data)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_set_control_block_address(self, addr):
        """
        Indicates to the dll the location of the RTT control block in the device memory.

        @param int addr: Address of the RTT Control Block in memory.
        """
        if not self._is_u32(addr):
            raise ValueError('The address parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint32(addr)

        result = self._lib.NRFJPROG_rtt_set_control_block_address(addr)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_start(self):
        """
        Starts RTT.

        """
        result = self._lib.NRFJPROG_rtt_start()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_is_control_block_found(self):
        """
        Checks if RTT control block has been found.

        @return boolean: True if found.
        """
        is_control_block_found = ctypes.c_bool()

        result = self._lib.NRFJPROG_rtt_is_control_block_found(ctypes.byref(is_control_block_found))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return is_control_block_found.value

    def rtt_stop(self):
        """
        Stops RTT.

        """
        result = self._lib.NRFJPROG_rtt_stop()
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_read(self, channel_index, length):
        """
        Reads from an RTT channel.

        @param int channel_index: RTT channel to read.
        @param int length: Number of characters to read.
        @return string: Data read.
        """
        if not self._is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if not self._is_u32(length):
            raise ValueError('The length parameter must be an unsigned 32-bit value.')

        channel_index = ctypes.c_uint32(channel_index)
        length = ctypes.c_uint32(length)
        data = (ctypes.c_uint8 * length.value)()
        data_read = ctypes.c_uint32()

        result = self._lib.NRFJPROG_rtt_read(channel_index, ctypes.byref(data), length, ctypes.byref(data_read))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return ''.join(chr(i) for i in data[0:data_read.value])

    def rtt_write(self, channel_index, msg):
        """
        Writes to an RTT channel.

        @param int channel_index: RTT channel to write.
        @param string msg: Data to write.
        @return int: Number of characters written.
        """
        if not self._is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if not isinstance(msg, str):
            raise ValueError('The msg parameter must be a text message of str type.')

        channel_index = ctypes.c_uint32(channel_index)
        length = ctypes.c_uint32(len(msg))
        data = (ctypes.c_uint8 * length.value)(*[ord(i) if isinstance(i, str) else i for i in msg.encode('ascii')]) # for i in msg.encode('ascii') returns a string of length 1 in Python 2, but an int (8-bit ascii value) in Python 3. therefore the conditional with ord.
        data_written = ctypes.c_uint32()

        result = self._lib.NRFJPROG_rtt_write(channel_index, ctypes.byref(data), length, ctypes.byref(data_written))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return data_written.value

    def rtt_read_channel_count(self):
        """
        Gets the number of RTT channels.

        @return (int, int): Tuple containing the number of down RTT channels and the number of up RTT channels.
        """
        down_channel_number = ctypes.c_uint32()
        up_channel_number = ctypes.c_uint32()

        result = self._lib.NRFJPROG_rtt_read_channel_count(ctypes.byref(down_channel_number), ctypes.byref(up_channel_number))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return down_channel_number.value, up_channel_number.value

    def rtt_read_channel_info(self, channel_index, direction):
        """
        Reads the info from one RTT channel.

        @param int channel_index: RTT channel to request info.
        @param int, string, or RTTChannelDirection(IntEnum) direction: Direction of the channel to request info.
        @return string: Channel name.
        """
        if not self._is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if not self._is_enum(direction, RTTChannelDirection):
            raise ValueError('Parameter direction must be of type int, str or RTTChannelDirection enumeration.')

        direction = self._decode_enum(direction, RTTChannelDirection)
        if direction is None:
            raise ValueError('Parameter direction must be of type int, str or RTTChannelDirection enumeration.')

        channel_index = ctypes.c_uint32(channel_index)
        direction = ctypes.c_int(direction.value)
        name_len = ctypes.c_uint32(32)
        name = (ctypes.c_uint8 * 32)()
        size = ctypes.c_uint32()

        result = self._lib.NRFJPROG_rtt_read_channel_info(channel_index, direction, ctypes.byref(name), name_len, ctypes.byref(size))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return ''.join(chr(i) for i in name if i != 0), size.value

    """
    Internal helper functions.

    """
    def _generate_log_str_cb(self, log_str_cb, log, log_str, log_file):
        """
        Setup API's debug output logging mechanism.

        """
        if log_str_cb is not None:
            return ctypes.CFUNCTYPE(None, ctypes.c_char_p)(log_str_cb)

        if log is True or log_str is not None or log_file is not None:
            if log_str is None:
                log_str = '[NRFJPROG DLL LOG]: '
            if log_file is None:
                log_file = sys.stderr
            elif log_file is not sys.stderr and log_file is not sys.stdout:
                self._log_file = open(log_file, 'w')

            return ctypes.CFUNCTYPE(None, ctypes.c_char_p)(lambda x: print(log_str + '{}'.format(x.strip()), file=self._log_file))

    def _is_u32(self, value):
        return isinstance(value, int) and 0 <= value <= 0xFFFFFFFF

    def _is_u8(self, value):
        return isinstance(value, int) and 0 <= value <= 0xFF

    def _is_bool(self, value):
        return isinstance(value, bool) or 0 <= value <= 1

    def _is_valid_buf(self, buf):
        for value in buf:
            if not self._is_u8(value):
                return False
        return len(buf) > 0

    def _is_enum(self, param, enum_type):
        if isinstance(param, int) and param in [member.value for name, member in enum_type.__members__.items()]:
            return True
        elif isinstance(param, str) and param in [name for name, member in enum_type.__members__.items()]:
            return True
        elif param in enum_type.__members__.items():
            return True
        return False

    def _decode_enum(self, param, enum_type):
        if not self._is_enum(param, enum_type):
            return None

        if isinstance(param, int):
            return enum_type(param)
        elif isinstance(param, str):
            return enum_type[param]
        else:
            return param

    def __enter__(self):
        """
        Called automatically when the 'with' construct is used.

        """
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        """
        Called automatically when the 'with' construct is used.

        """
        self.close()
