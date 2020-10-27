"""
This module serves as a Python wrapper around the nrfjprog Highlevel DLL.

Note: Please look at the highlevelnrfjprogdll.h file provided with in the folder "documentation" for a more elaborate description of the API functions and their side effects.
"""

from __future__ import print_function

import codecs
import ctypes
import os
import logging

from .APIError import *
from .Parameters import *

"""
Logging:
    The HighLevel module uses the python logging module for logging. The following log levels are used:
        DEBUG: Detailed messages about the executed instructions from all layers of nRFJProg and J-Link.
        INFO:  General progress messages from the HighLevel commands.
        ERROR: Error codes and exceptions.
    
    pynrfjprog performs no configuration of logger channels, but not all logging channels receive output by default.    
    ERROR logging is always performed, while DEBUG and INFO log messages can be suppressed to increase performance. 
    By setting log=False, the required callbacks are not passed to the library, disabling INFO and DEBUG level messages.    
"""

# Disable logging to API logger if no other handlers are present
logging.getLogger(__name__).addHandler(logging.NullHandler())


class API(object):
    """
    API model based on the HighLevel DLL.
    Provides a high level, but still C-like API for J-Link debuggers targeting nRF devices.
    Root class of the module. Instance the class to get access to highlevel nrfjprog.dll functions in Python.
    """

    # List of generated probes to make sure their python objects survive to self.close() if neccessary.
    probes = list()

    def __init__(self, log=True):
        """
        Constructor.

        @param (optional) bool log: If present and true, will enable logging to the logging instance returned by logging.getLogger(logid)
        """

        self._logger = logging.getLogger(__name__)

        self.debug_callback = Parameters.Logger.create_callback(self._logger.debug) if log else None

        this_dir, this_file = os.path.split(__file__)

        if sys.maxsize > 2 ** 32:
            libfolder = 'lib_x64'
        else:
            libfolder = 'lib_x86'

        os_name = sys.platform.lower()

        if os_name.startswith('win'):
            highlevel_nrfjprog_dll_name = 'highlevelnrfjprog.dll'
        elif os_name.startswith('lin'):
            highlevel_nrfjprog_dll_name = 'libhighlevelnrfjprog.so'
        elif os_name.startswith('dar'):
            highlevel_nrfjprog_dll_name = 'libhighlevelnrfjprog.dylib'
        else:
            raise Exception('Unsupported operating system!')

        highlevel_nrfjprog_dll_path = os.path.join(os.path.abspath(this_dir), libfolder, highlevel_nrfjprog_dll_name)

        if not os.path.exists(highlevel_nrfjprog_dll_path):
            raise APIError(NrfjprogdllErr.NRFJPROG_SUB_DLL_NOT_FOUND, highlevel_nrfjprog_dll_path, log=self._logger.error)

        try:
            self.lib = ctypes.cdll.LoadLibrary(highlevel_nrfjprog_dll_path)
        except Exception as ex:
            raise APIError(NrfjprogdllErr.NRFJPROG_SUB_DLL_COULD_NOT_BE_OPENED, 'Got error {} for library at {}'.format(repr(ex), highlevel_nrfjprog_dll_path), log=self._logger.error)

    """
    highlevelnrfjprog DLL functions.

    """

    def dll_version(self):
        """
        Returns the highlevelnrfjprog.dll version.

        @return (int, int, int): Tuple containing the major, minor and micro version of the DLL.
        """
        major = ctypes.c_uint32(0)
        minor = ctypes.c_uint32(0)
        micro = ctypes.c_uint32(0)

        result = self.lib.NRFJPROG_dll_version(ctypes.byref(major), ctypes.byref(minor), ctypes.byref(micro))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return major.value, minor.value, micro.value

    def open(self, jlink_arm_dll_path=None):
        """
        @param (optional) str jlink_arm_dll_path: Absolute path to the JLinkARM DLL that you want nrfjprog to use. Does not support unicode paths.
        """

        if jlink_arm_dll_path is not None:
            jlink_arm_dll_path = str(jlink_arm_dll_path).encode('utf-8')

        result = self.lib.NRFJPROG_dll_open(jlink_arm_dll_path, self.debug_callback)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def close(self):
        self.lib.NRFJPROG_dll_close()
        # List.clear is not available in Python 2.7. Manually delete all elements in list instead.
        del self.probes[:]

    def is_open(self):

        is_opened = ctypes.c_bool(False)

        result = self.lib.NRFJPROG_is_dll_open(ctypes.byref(is_opened))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return is_opened.value

    def get_connected_probes(self):
        serial_numbers_len = ctypes.c_uint32(127)
        serial_numbers = (ctypes.c_uint32 * serial_numbers_len.value)(0)
        num_available = ctypes.c_uint32(0)

        result = self.lib.NRFJPROG_get_connected_probes(ctypes.byref(serial_numbers), serial_numbers_len, ctypes.byref(num_available))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        snr = [int(serial_numbers[i]) for i in range(0, min(num_available.value, serial_numbers_len.value))]

        return snr

    def register_probe(self, probe):
        self.probes.append(probe)

    def deregister_probe(self, probe):
        if probe in self.probes:
            self.probes.remove(probe)

    def __enter__(self):
        """
        Called automatically when the 'with' construct is used.

        """
        self.open()
        return self

    def __exit__(self, ex_type, ex_value, traceback):
        """
        Called automatically when the 'with' construct is used.

        """
        self.close()


class Probe(object):
    """
    Abstract object model based on the HighLevel DLL API, targeting one probe at a time.
    Provides a "pythonic" API for J-Link debuggers targeting nRF devices.
    """

    def __init__(self, api, log, log_suffix):
        """
        Base probe constructor

        :param api:         The HighLevel.API instance to use as a library backend
        :type api:          HighLevel.API
        :param log:         If False, info callback and debug callback is not generated, improving performance slightly.
        :type log:          Boolean
        :param log_suffix:  Probe will log to loggging logger pynrfjprog.HighLevel.Probes.<log_suffix>
        :type log_suffix:   String
        """
        if not api.is_open():
            raise APIError(NrfjprogdllErr.INVALID_OPERATION, "Provided API is not open")

        self._program_options = ProgramOptions()
        self._read_options = ReadOptions(readcode=True)
        self._qspi_ini_params = QSPIInitParams()

        self._api = api
        self._handle = None

        self._logger = logging.getLogger("{}.Probes.{}".format(__name__, log_suffix))

        self.info_callback = Parameters.Logger.create_callback(self._logger.info) if log else None
        self.debug_callback = Parameters.Logger.create_callback(self._logger.debug) if log else None
        self._api.register_probe(self)

    def __enter__(self):
        """
        Called automatically when the 'with' construct is used.

        """
        return self

    def __exit__(self, ex_type, ex_value, traceback):
        """
        Called automatically when the 'with' construct is used.

        """
        self.close()

    def close(self):
        if self._handle is not None and self._api.is_open():
            result = self._api.lib.NRFJPROG_probe_uninit(ctypes.byref(self._handle))
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
            self._handle = None
        self._api.deregister_probe(self)

    def probe_reset(self):
        """
        Resets the connected debug probe.
        """

        result = self._api.lib.NRFJPROG_probe_reset(self._handle)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def probe_replace_fw(self):
        """
        Replace the firmware of the connected debug probe.
        """

        result = self._api.lib.NRFJPROG_probe_replace_fw(self._handle)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def setup_qspi(self, memory_size, qspi_ini_params=QSPIInitParams()):

        if not isinstance(qspi_ini_params, QSPIInitParams):
            raise TypeError('The qspi_ini_params parameter must be an instance of class QSPIInitParams.')
        if not is_u32(memory_size):
            raise TypeError('The memory_size parameter must fit an unsigned 32-bit value.')

        memory_size = ctypes.c_uint32(memory_size)

        result = self._api.lib.NRFJPROG_probe_setup_qspi(self._handle, memory_size, qspi_ini_params)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def set_coprocessor(self, coprocessor):
        """
        Sets coprocessor to use for subsequent operations.
        @param CoProcessor coprocessor: Target coprocessor.
        """
        if not is_enum(coprocessor, CoProcessor):
            raise TypeError('Parameter coprocessor must be of type int, str or CoProcessor enumeration.')

        coprocessor = ctypes.c_int(decode_enum(coprocessor, CoProcessor))

        result = self._api.lib.NRFJPROG_probe_set_coprocessor(self._handle, coprocessor)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def get_library_info(self):
        library_info = LibraryInfoStruct(0)
        result = self._api.lib.NRFJPROG_get_library_info(self._handle, ctypes.byref(library_info))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return LibraryInfo(library_info)

    def get_probe_info(self):
        probe_info = ProbeInfoStruct(0)
        result = self._api.lib.NRFJPROG_get_probe_info(self._handle, ctypes.byref(probe_info))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return ProbeInfo(probe_info)

    def get_device_info(self):
        device_info = DeviceInfoStruct(0)
        result = self._api.lib.NRFJPROG_get_device_info(self._handle, ctypes.byref(device_info))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return DeviceInfo(device_info)

    def get_readback_protection(self):
        protection_status = ctypes.c_int(0)
        result = self._api.lib.NRFJPROG_get_readback_protection(self._handle, ctypes.byref(protection_status))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

        return ReadbackProtection(protection_status.value)

    def readback_protect(self, protection_status=ReadbackProtection.ALL):
        if not isinstance(protection_status, ReadbackProtection):
            raise TypeError('Parameter protection_status must be of type int, str or ReadbackProtection enumeration.')

        protection_status = ctypes.c_int(decode_enum(protection_status, ReadbackProtection))

        result = self._api.lib.NRFJPROG_readback_protect(self._handle, protection_status)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def program(self, hex_path, program_options=None):
        hex_path = str(hex_path).encode('utf-8')

        if program_options is None:
            result = self._api.lib.NRFJPROG_program(self._handle, hex_path, self._program_options)
        else:
            if not isinstance(program_options, ProgramOptions):
                raise TypeError('The program_options parameter must be an instance of class ProgramOptions.')

            result = self._api.lib.NRFJPROG_program(self._handle, hex_path, program_options)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def read_to_file(self, hex_path, read_options=None):
        hex_path = str(hex_path).encode('utf-8')

        if read_options is None:
            result = self._api.lib.NRFJPROG_read_to_file(self._handle, hex_path, self._read_options)
        else:
            if not isinstance(read_options, ReadOptions):
                raise TypeError('The program_options parameter must be an instance of class ProgramOptions.')

            result = self._api.lib.NRFJPROG_read_to_file(self._handle, hex_path, read_options)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def verify(self, hex_path, verify_action=VerifyAction.VERIFY_READ):
        if not isinstance(verify_action, VerifyAction):
            raise TypeError('Parameter verify_action must be of type int, str or VerifyAction enumeration.')

        verify_action = ctypes.c_int(decode_enum(verify_action, VerifyAction))

        hex_path = str(hex_path).encode('utf-8')

        result = self._api.lib.NRFJPROG_verify(self._handle, hex_path, verify_action)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def erase(self, erase_action=EraseAction.ERASE_ALL, start_address=0, end_address=0):
        if not isinstance(erase_action, EraseAction):
            raise TypeError('Parameter erase_action must be of type int, str or EraseAction enumeration.')
        if not is_u32(start_address):
            raise TypeError('The start_address parameter must fit an unsigned 32-bit value.')
        if not is_u32(end_address):
            raise TypeError('The end_address parameter must fit an unsigned 32-bit value.')

        erase_action = ctypes.c_int(decode_enum(erase_action, EraseAction))
        start_address = ctypes.c_uint32(start_address)
        end_address = ctypes.c_uint32(end_address)

        result = self._api.lib.NRFJPROG_erase(self._handle, erase_action, start_address, end_address)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def recover(self):
        result = self._api.lib.NRFJPROG_recover(self._handle)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def read(self, address, data_len=4):
        if not is_u32(address):
            raise TypeError('The address parameter must fit an unsigned 32-bit value.')

        if not is_u32(data_len):
            raise TypeError('The data_len parameter must fit an unsigned 32-bit value.')

        address = ctypes.c_uint32(address)
        data_len = ctypes.c_uint32(data_len)

        if data_len.value == 4:
            data = ctypes.c_uint32(0)

            result = self._api.lib.NRFJPROG_read_u32(self._handle, address, ctypes.byref(data))
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)

            return data.value

        else:
            data = (ctypes.c_uint8 * data_len.value)()

            result = self._api.lib.NRFJPROG_read(self._handle, address, ctypes.byref(data), data_len)
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)

            return bytearray(data)

    def write(self, address, data):

        if not is_u32(address):
            raise ValueError('The address parameter must be an unsigned 32-bit value.')

        address = ctypes.c_uint32(address)

        if is_u32(data):
            data = ctypes.c_uint32(data)

            result = self._api.lib.NRFJPROG_write_u32(self._handle, address, data)

            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        elif is_valid_buf(data):

            data_len = ctypes.c_uint32(len(data))
            data = (ctypes.c_uint8 * data_len.value)(*data)

            result = self._api.lib.NRFJPROG_write(self._handle, address, ctypes.byref(data), data_len)

            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        else:
            raise ValueError('The data parameter must be a uint32-representable value, or a sequence of uint8-representable values with at least one item.')

    def reset(self, reset_action=ResetAction.RESET_SYSTEM):
        if not isinstance(reset_action, ResetAction):
            raise TypeError('Parameter reset_action must be of type int, str or ResetAction enumeration.')

        reset_action = ctypes.c_int(decode_enum(reset_action, ResetAction))

        result = self._api.lib.NRFJPROG_reset(self._handle, reset_action)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)

    def run(self, pc, sp):

        if not is_u32(pc):
            raise ValueError('The pc parameter must be an unsigned 32-bit value.')

        if not is_u32(sp):
            raise ValueError('The sp parameter must be an unsigned 32-bit value.')

        pc = ctypes.c_uint32(pc)
        sp = ctypes.c_uint32(sp)

        result = self._api.lib.NRFJPROG_run(self._handle, pc, sp)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result, log=self._logger.error)
        

class MCUBootDFUProbe(Probe):
    """ Specialization of Probe interface for MCUBoot DFU via serial port connection. """
    def __init__(self, api, serial_port, baud_rate=115200, timeout=30000, log=True):
        """
        :param api:                 The HighLevel.API instance to use as a library backend
        :type api:                  HighLevel.API
        :param serial_port:         Serial port reference of target device
        :type serial_port:          Path object or string
        :param baud_rate:           Connection baud rate
        :type baud_rate:            32-bit unsigned integer
        :param timeout:             Connection timeout limit in milliseconds
        :type timeout:              32-bit unsigned integer
        :param log:                 If False, info callback and debug callback is not generated, improving performance slightly.
        :type log:                  Boolean
        """

        serial_port = str(serial_port)
        Probe.__init__(self, api, log, serial_port)

        if not is_u32(baud_rate):
            raise TypeError('The baud_rate parameter must fit an unsigned 32-bit value.')

        if not is_u32(timeout):
            raise TypeError('The timeout parameter must fit an unsigned 32-bit value.')

        try:
            self._handle = ctypes.c_void_p(None)
            serial_port = serial_port.encode('utf-8')
            baud_rate = ctypes.c_uint32(baud_rate)
            timeout = ctypes.c_uint32(timeout)

            result = self._api.lib.NRFJPROG_mcuboot_dfu_init(ctypes.byref(self._handle), self.info_callback, self.debug_callback, serial_port, baud_rate, timeout)
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        except (APIError, TypeError):
            self._handle = None
            raise

    def verify(self, hex_path, verify_action=VerifyAction.VERIFY_NONE):
        """ Override base verify implementation to get correct default action """
        Probe.verify(self, hex_path, verify_action)

class ModemUARTDFUProbe(Probe):
    """ Specialization of Probe interface for Modem UART DFU via serial port connection. """
    def __init__(self, api, serial_port, baud_rate=1000000, timeout=30000, log=True):
        """
        :param api:                 The HighLevel.API instance to use as a library backend
        :type api:                  HighLevel.API
        :param serial_port:         Serial port reference of target device
        :type serial_port:          Path object or string
        :param baud_rate:           Connection baud rate
        :type baud_rate:            32-bit unsigned integer
        :param timeout:             Connection timeout limit in milliseconds
        :type timeout:              32-bit unsigned integer
        :param log:                 If False, info callback and debug callback is not generated, improving performance slightly.
        :type log:                  Boolean
        """

        serial_port = str(serial_port)
        Probe.__init__(self, api, log, serial_port)

        if not is_u32(baud_rate):
            raise TypeError('The baud_rate parameter must fit an unsigned 32-bit value.')

        if not is_u32(timeout):
            raise TypeError('The timeout parameter must fit an unsigned 32-bit value.')

        try:
            self._handle = ctypes.c_void_p(None)
            serial_port = serial_port.encode('utf-8')
            baud_rate = ctypes.c_uint32(baud_rate)
            timeout = ctypes.c_uint32(timeout)

            result = self._api.lib.NRFJPROG_modemdfu_dfu_serial_init(ctypes.byref(self._handle), self.info_callback, self.debug_callback, serial_port, baud_rate, timeout)
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        except (APIError, TypeError):
            self._handle = None
            raise

    def verify(self, hex_path, verify_action=VerifyAction.VERIFY_HASH):
        """ Override base verify implementation to get correct default action """
        Probe.verify(self, hex_path, verify_action)

class IPCDFUProbe(Probe):
    """ Specialization of Probe interface for IPC DFU via SWD debugger connection. """
    def __init__(self, api, snr, coprocessor, jlink_arm_dll_path=None, log=True):
        """
        :param api:                 The HighLevel.API instance to use as a library backend
        :type api:                  HighLevel.API
        :param snr:                 Serial number of connected J-Link debug probe
        :type snr:                  32 bit uint
        :param coprocessor:         Probe will try to connect to the indicated coprocessor during initialization.
        :type coprocessor:          Parameters.CoProcessor
        :param jlink_arm_dll_path:  Path to jlinkarm dll to use as backend. If None, an automated search will look for the library, and select the newest one. Does not support unicode paths.
        :type jlink_arm_dll_path:   String
        :param log:                 If False, info callback and debug callback is not generated, improving performance slightly.
        :type log:                  Boolean
        """
        Probe.__init__(self, api, log, str(snr))

        try:
            self._handle = ctypes.c_void_p(None)
            snr = ctypes.c_uint32(snr)

            if jlink_arm_dll_path is not None:
                jlink_arm_dll_path = str(jlink_arm_dll_path).encode('utf-8')

            if not is_enum(coprocessor, CoProcessor):
                raise TypeError('Parameter direction must be of type int, str or CoProcessor enumeration.')
            coprocessor = ctypes.c_int(decode_enum(coprocessor, CoProcessor))

            result = self._api.lib.NRFJPROG_dfu_init(ctypes.byref(self._handle), self.info_callback, self.debug_callback, snr, coprocessor, jlink_arm_dll_path)
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        except (APIError, TypeError):
            self._handle = None
            raise

    def verify(self, hex_path, verify_action=VerifyAction.VERIFY_HASH):
        """ Override base verify implementation to get correct default action """
        Probe.verify(self, hex_path, verify_action)


class DebugProbe(Probe):
    """ Specialization of Probe interface for SWD debugger connections. """
    def __init__(self, api, snr, coprocessor=None, jlink_arm_dll_path=None, log=True):
        """
        :param api:                 The HighLevel.API instance to use as a library backend
        :type api:                  HighLevel.API
        :param snr:                 Serial number of connected J-Link debug probe
        :type snr:                  32 bit uint
        :param coprocessor:         If not None, probe will try to connect to the indicated coprocessor during initialization.
        :type coprocessor:          Parameters.CoProcessor
        :param jlink_arm_dll_path:  Path to jlinkarm dll to use as backend. If None, an automated search will look for the library, and select the newest one. Does not support unicode paths.
        :type jlink_arm_dll_path:   String
        :param log:                 If False, info callback and debug callback is not generated, improving performance slightly.
        :type log:                  Boolean
        """
        Probe.__init__(self, api, log, str(snr))

        try:
            self._handle = ctypes.c_void_p(None)
            snr = ctypes.c_uint32(snr)

            if jlink_arm_dll_path is not None:
                jlink_arm_dll_path = str(jlink_arm_dll_path).encode('utf-8')

            result = self._api.lib.NRFJPROG_probe_init(ctypes.byref(self._handle), self.info_callback, self.debug_callback, snr, jlink_arm_dll_path)
            if result != NrfjprogdllErr.SUCCESS:
                raise APIError(result, log=self._logger.error)
        except (APIError, TypeError):
            self._handle = None
            raise

        if coprocessor is not None:
            try:
                self.set_coprocessor(coprocessor)
            except (APIError, TypeError):
                self.close()
                raise

    def is_rtt_started(self):
        """
        Checks if the RTT is started.

        @return bool: True if started.
        """
        started = ctypes.c_bool()

        result = self._api.lib.NRFJPROG_is_rtt_started(self._handle, ctypes.byref(started))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return started.value

    def rtt_set_control_block_address(self, addr):
        """
        Indicates to the dll the location of the RTT control block in the device memory.

        @param int addr: Address of the RTT Control Block in memory.
        """
        if not is_u32(addr):
            raise ValueError('The address parameter must be an unsigned 32-bit value.')

        addr = ctypes.c_uint32(addr)

        result = self._api.lib.NRFJPROG_rtt_set_control_block_address(self._handle, addr)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_start(self):
        """
        Starts RTT.

        """
        result = self._api.lib.NRFJPROG_rtt_start(self._handle)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_is_control_block_found(self):
        """
        Checks if RTT control block has been found.

        @return boolean: True if found.
        """
        is_control_block_found = ctypes.c_bool()

        result = self._api.lib.NRFJPROG_rtt_is_control_block_found(self._handle, ctypes.byref(is_control_block_found))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return is_control_block_found.value

    def rtt_stop(self):
        """
        Stops RTT.

        """
        result = self._api.lib.NRFJPROG_rtt_stop(self._handle)
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

    def rtt_read(self, channel_index, length, encoding='utf-8'):
        """
        Reads from an RTT channel.

        @param int channel_index: RTT channel to read.
        @param int length: Number of bytes to read. Note that depending on the encoding parameter, the number of bytes read and the numbers of characters read might differ.
        @param (optional) str or None encoding: Encoding for the data read in order to build a readable string. Default value 'utf-8'. Note that since Python2 native string is coded in ASCII, only ASCII characters will be properly represented.
        @return str or bytearray: Data read. Return type depends on encoding optional parameter. If an encoding is given, the return type will be Python version's native string type. If None is given, a bytearray will be returned.
        """
        if not is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if not is_u32(length):
            raise ValueError('The length parameter must be an unsigned 32-bit value.')

        if encoding is not None and not is_valid_encoding(encoding):
            raise ValueError('The encoding parameter must be either None or a standard encoding in python.')

        channel_index = ctypes.c_uint32(channel_index)
        length = ctypes.c_uint32(length)
        data = (ctypes.c_uint8 * length.value)()
        data_read = ctypes.c_uint32()

        result = self._api.lib.NRFJPROG_rtt_read(self._handle, channel_index, ctypes.byref(data), length, ctypes.byref(data_read))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return bytearray(data[0:data_read.value]) if encoding is None else bytearray(data[0:data_read.value]).decode(encoding).encode('utf-8') if sys.version_info[0] == 2 else bytearray(data[0:data_read.value]).decode(encoding)

    def rtt_write(self, channel_index, msg, encoding='utf-8'):
        """
        Writes to an RTT channel.

        @param int channel_index: RTT channel to write.
        @param sequence msg: Data to write. Any type that implements the sequence API (i.e. string, list, bytearray...) is valid as input.
        @param (optional) str or None encoding: Encoding of the msg to write. Default value 'utf-8'.
        @return int: Number of bytes written.  Note that if non-'latin-1' characters are used, the number of bytes written depends on the encoding parameter given.
        """
        if not is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if encoding is not None and not is_valid_encoding(encoding):
            raise ValueError('The encoding parameter must be either None or a standard encoding in python.')

        msg = bytearray(msg.encode(encoding)) if encoding else bytearray(msg)
        if not is_valid_buf(msg):
            raise ValueError('The msg parameter must be a sequence type with at least one item.')

        channel_index = ctypes.c_uint32(channel_index)
        length = ctypes.c_uint32(len(msg))
        data = (ctypes.c_uint8 * length.value)(*msg)
        data_written = ctypes.c_uint32()

        result = self._api.lib.NRFJPROG_rtt_write(self._handle, channel_index, ctypes.byref(data), length, ctypes.byref(data_written))
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

        result = self._api.lib.NRFJPROG_rtt_read_channel_count(self._handle, ctypes.byref(down_channel_number), ctypes.byref(up_channel_number))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return down_channel_number.value, up_channel_number.value

    def rtt_read_channel_info(self, channel_index, direction):
        """
        Reads the info from one RTT channel.

        @param int channel_index: RTT channel to request info.
        @param int, str, or RTTChannelDirection(IntEnum) direction: Direction of the channel to request info.
        @return (str, int): Tuple containing the channel name and the size of channel buffer.
        """
        if not is_u32(channel_index):
            raise ValueError('The channel_index parameter must be an unsigned 32-bit value.')

        if not is_enum(direction, RTTChannelDirection):
            raise ValueError('Parameter direction must be of type int, str or RTTChannelDirection enumeration.')

        direction = decode_enum(direction, RTTChannelDirection)
        if direction is None:
            raise ValueError('Parameter direction must be of type int, str or RTTChannelDirection enumeration.')

        channel_index = ctypes.c_uint32(channel_index)
        direction = ctypes.c_int(direction.value)
        name_len = ctypes.c_uint32(32)
        name = (ctypes.c_uint8 * 32)()
        size = ctypes.c_uint32()

        result = self._api.lib.NRFJPROG_rtt_read_channel_info(self._handle, channel_index, direction, ctypes.byref(name), name_len, ctypes.byref(size))
        if result != NrfjprogdllErr.SUCCESS:
            raise APIError(result)

        return ''.join(chr(i) for i in name if i != 0), size.value
