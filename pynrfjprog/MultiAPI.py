"""
MultiAPI module.

"""

import inspect
import multiprocessing

try:
    from . import API
except Exception:
    import API

# WORKAROUND!
# When bundling the script to an .exe, multiprogramming.Process
# won't work properly (when accessing a queue from the parent
# process, we get IllegalHandle or Access Denied).
# This workaround is directly copied from here:

#   https://github.com/pyinstaller/pyinstaller/wiki/Recipe-Multiprocessing
#
import os
import sys

try:
    # Python 3.4+
    if sys.platform.startswith('win'):
        import multiprocessing.popen_spawn_win32 as forking
    else:
        import multiprocessing.popen_fork as forking
except ImportError:
    import multiprocessing.forking as forking

if sys.platform.startswith('win'):
    # First define a modified version of Popen.
    class _Popen(forking.Popen):
        def __init__(self, *args, **kw):
            if hasattr(sys, 'frozen'):
                # We have to set original _MEIPASS2 value from sys._MEIPASS
                # to get --onefile mode working.
                os.putenv('_MEIPASS2', sys._MEIPASS)
            try:
                super(_Popen, self).__init__(*args, **kw)
            finally:
                if hasattr(sys, 'frozen'):
                    # On some platforms (e.g. AIX) 'os.unsetenv()' is not
                    # available. In those cases we cannot delete the variable
                    # but only set it to the empty string. The bootloader
                    # can handle this case.
                    if hasattr(os, 'unsetenv'):
                        os.unsetenv('_MEIPASS2')
                    else:
                        os.putenv('_MEIPASS2', '')

    # Second override 'Popen' class with our modified version.
    forking.Popen = _Popen
    # print('\t\tworkaround complete')
# END WORKAROUND

"""
Deprecated: Do not use, use log parameter in MultiAPI constructor instead.
"""
DEBUG_OUTPUT = False


class _Command(object):
    def __init__(self, cmd, *args):
        self.cmd = cmd
        self.args = args


class _CommandAck(object):
    def __init__(self, exception=None, result=None):
        self.exception = exception
        self.result = result


class MultiAPI(object):
    """
    Main class of the module. Instance the class several times to get access to nrfjprog.dll functions, including FICR modifying functions, in Python for several devices simultaneously.

    Note: A copy of nrfjprog.dll must be found in the working directory.
    """

    def __init__(self, device_family, jlink_arm_dll_path=None, log=False, log_str=None, log_file_path=None):
        """
        Constructor. Initializes multiprocessing queues, creates a subprocess for the API instance and runs it.

        @param enum, string or int device_family: The series of device pynrfjprog will interact with.
        @param (optional) string jlink_arm_dll_path: Absolute path to the JLinkARM DLL that you want nrfjprog to use.
        @param (optional) bool log: If present and true, will enable logging to sys.stderr with the default log string appended to the beginning of each debug output line.
        @param (optional) string log_str: If present, will enable logging to sys.stderr with overwriten default log string appended to the beginning of each debug output line.
        @param (optional) string log_file_path: If present, will enable logging to log_file specified. This file will be opened in write mode in API.__init__() and closed when api.close() is called.
        """
        self.CmdQueue = multiprocessing.Queue()
        self.CmdAckQueue = multiprocessing.Queue()

        if DEBUG_OUTPUT:
            log = True

        self.runner = multiprocessing.Process(target=self._runner, args=(device_family, jlink_arm_dll_path, log, log_str, log_file_path))
        self.runner.daemon = True
        self.runner.start()

    def dll_version(self):
        self.CmdQueue.put(_Command('dll_version'))
        return self._wait_for_completion()

    def open(self):
        self.CmdQueue.put(_Command('open'))
        return self._wait_for_completion()

    def close(self):
        self.CmdQueue.put(_Command('close'))
        return self._wait_for_completion()

    def enum_emu_snr(self):
        self.CmdQueue.put(_Command('enum_emu_snr'))
        return self._wait_for_completion()

    def is_connected_to_emu(self):
        self.CmdQueue.put(_Command('is_connected_to_emu'))
        return self._wait_for_completion()

    def connect_to_emu_with_snr(self, serial_number, jlink_speed_khz=API.API._DEFAULT_JLINK_SPEED_KHZ):
        self.CmdQueue.put(_Command('connect_to_emu_with_snr', serial_number, jlink_speed_khz))
        return self._wait_for_completion()

    def connect_to_emu_without_snr(self, jlink_speed_khz=API.API._DEFAULT_JLINK_SPEED_KHZ):
        self.CmdQueue.put(_Command('connect_to_emu_without_snr', jlink_speed_khz))
        return self._wait_for_completion()

    def disconnect_from_emu(self):
        self.CmdQueue.put(_Command('disconnect_from_emu'))
        return self._wait_for_completion()

    def recover(self):
        self.CmdQueue.put(_Command('recover'))
        return self._wait_for_completion()

    def is_connected_to_device(self):
        self.CmdQueue.put(_Command('is_connected_to_device'))
        return self._wait_for_completion()

    def connect_to_device(self):
        self.CmdQueue.put(_Command('connect_to_device'))
        return self._wait_for_completion()

    def readback_protect(self, desired_protection_level):
        self.CmdQueue.put(_Command('readback_protect', desired_protection_level))
        return self._wait_for_completion()

    def readback_status(self):
        self.CmdQueue.put(_Command('readback_status'))
        return self._wait_for_completion()

    def read_region_0_size_and_source(self):
        self.CmdQueue.put(_Command('read_region_0_size_and_source'))
        return self._wait_for_completion()

    def debug_reset(self):
        self.CmdQueue.put(_Command('debug_reset'))
        return self._wait_for_completion()

    def sys_reset(self):
        self.CmdQueue.put(_Command('sys_reset'))
        return self._wait_for_completion()

    def pin_reset(self):
        self.CmdQueue.put(_Command('pin_reset'))
        return self._wait_for_completion()

    def disable_bprot(self):
        self.CmdQueue.put(_Command('disable_bprot'))
        return self._wait_for_completion()

    def erase_all(self):
        self.CmdQueue.put(_Command('erase_all'))
        return self._wait_for_completion()

    def erase_page(self, addr):
        self.CmdQueue.put(_Command('erase_page', addr))
        return self._wait_for_completion()

    def erase_uicr(self):
        self.CmdQueue.put(_Command('erase_uicr'))
        return self._wait_for_completion()

    def write_u32(self, addr, data, control):
        self.CmdQueue.put(_Command('write_u32', addr, data, control))
        return self._wait_for_completion()

    def read_u32(self, addr):
        self.CmdQueue.put(_Command('read_u32', addr))
        return self._wait_for_completion()

    def write(self, addr, data, control):
        self.CmdQueue.put(_Command('write', addr, data, control))
        return self._wait_for_completion()

    def read(self, addr, length):
        self.CmdQueue.put(_Command('read', addr, length))
        return self._wait_for_completion()

    def is_halted(self):
        self.CmdQueue.put(_Command('is_halted'))
        return self._wait_for_completion()

    def halt(self):
        self.CmdQueue.put(_Command('halt'))
        return self._wait_for_completion()

    def run(self, pc, sp):
        self.CmdQueue.put(_Command('run', pc, sp))
        return self._wait_for_completion()

    def go(self):
        self.CmdQueue.put(_Command('go'))
        return self._wait_for_completion()

    def step(self):
        self.CmdQueue.put(_Command('step'))
        return self._wait_for_completion()

    def is_ram_powered(self):
        self.CmdQueue.put(_Command('is_ram_powered'))
        return self._wait_for_completion()

    def power_ram_all(self):
        self.CmdQueue.put(_Command('power_ram_all'))
        return self._wait_for_completion()

    def unpower_ram_section(self, section_index):
        self.CmdQueue.put(_Command('unpower_ram_section', section_index))
        return self._wait_for_completion()

    def read_cpu_register(self, register_name):
        self.CmdQueue.put(_Command('read_cpu_register', register_name))
        return self._wait_for_completion()

    def write_cpu_register(self, register_name, value):
        self.CmdQueue.put(_Command('write_cpu_register', register_name, value))
        return self._wait_for_completion()

    def read_device_version(self):
        self.CmdQueue.put(_Command('read_device_version'))
        return self._wait_for_completion()

    def read_debug_port_register(self, reg_addr):
        self.CmdQueue.put(_Command('read_debug_port_register', reg_addr))
        return self._wait_for_completion()

    def write_debug_port_register(self, reg_addr, data):
        self.CmdQueue.put(_Command('write_debug_port_register', reg_addr, data))
        return self._wait_for_completion()

    def read_access_port_register(self, ap_index, reg_addr):
        self.CmdQueue.put(_Command('read_access_port_register', ap_index, reg_addr))
        return self._wait_for_completion()

    def write_access_port_register(self, ap_index, reg_addr, data):
        self.CmdQueue.put(_Command('write_access_port_register', ap_index, reg_addr, data))
        return self._wait_for_completion()

    def rtt_set_control_block_address(self, address):
        self.CmdQueue.put(_Command('rtt_set_control_block_address', address))
        return self._wait_for_completion()

    def rtt_start(self):
        self.CmdQueue.put(_Command('rtt_start'))
        return self._wait_for_completion()

    def rtt_is_control_block_found(self):
        self.CmdQueue.put(_Command('rtt_is_control_block_found'))
        return self._wait_for_completion()

    def rtt_stop(self):
        self.CmdQueue.put(_Command('rtt_stop'))
        return self._wait_for_completion()

    def rtt_read(self, channel_index, length):
        self.CmdQueue.put(_Command('rtt_read', channel_index, length))
        return self._wait_for_completion()

    def rtt_write(self, channel_index, msg):
        self.CmdQueue.put(_Command('rtt_write', channel_index, msg))
        return self._wait_for_completion()

    def rtt_read_channel_count(self):
        self.CmdQueue.put(_Command('rtt_read_channel_count'))
        return self._wait_for_completion()

    def rtt_read_channel_info(self, channel_index, direction):
        self.CmdQueue.put(_Command('rtt_read_channel_info', channel_index, direction))
        return self._wait_for_completion()

    def _wait_for_completion(self):
        ack = self.CmdAckQueue.get()

        if ack.exception is not None:
            raise ack.exception
        if ack.result is not None:
            return ack.result

    def _runner(self, device_family, jlink_arm_dll_path, log, log_str, log_file):
        """
        Function that runs in a subprocess and executes the commands.

        """
        api = self._api_setup(device_family, jlink_arm_dll_path, log, log_str, log_file)
        api_functions = dict(inspect.getmembers(api, inspect.ismethod))

        while True:
            cmd = self.CmdQueue.get()
            try:
                res = api_functions[cmd.cmd](*cmd.args)
            except Exception as e:
                self.CmdAckQueue.put(_CommandAck(exception=e))
            else:
                self.CmdAckQueue.put(_CommandAck(result=res))

    def _api_setup(self, device_family, jlink_arm_dll_path, log, log_str, log_file):
        """
        Method to instance the class API from the API module.

        """
        return API.API(device_family, jlink_arm_dll_path, log=log, log_str=log_str, log_file_path=log_file)

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, type, value, traceback):
        self.close()
        self.runner.terminate()

    def __del__(self):
        if self.runner.is_alive():
            self.close()
            self.runner.terminate()
