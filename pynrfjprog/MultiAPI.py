"""
MultiAPI module.

"""

from __future__ import print_function

import inspect
import traceback
import multiprocessing
import sys

try:
    from . import API
except Exception:
    import API


"""
Deprecated: Do not use, use log parameter in MultiAPI constructor instead.
"""
DEBUG_OUTPUT = False


class _Command(object):
    def __init__(self, cmd, *args, **kwargs):
        self.cmd = cmd
        self.args = args
        self.kwargs = kwargs


class _CommandAck(object):
    def __init__(self, exception=None, result=None, stacktrace=""):
        self.exception = exception
        self.stacktrace = stacktrace
        self.result = result


class MultiAPI(object):
    """
    Main class of the module. Instance the class several times to get access to nrfjprog.dll functions, in Python for several devices simultaneously.

    Class interface mirrors API.API. Any object call MultiAPI.MultiAPI.method() is intercepted by the __getattr__ method.
    If the called method is a member of the regular API, the _execute method is called with the object name and parameters.
    The _execute method passes the method name and parameters to the _runner thread via a multiprocessing Queue.
    The _runner thread attempts to execute the passed method. Exceptions and return values are passed back to the main thread.

    Note: A copy of nrfjprog.dll must be found in the working directory.
    """

    def __init__(self, device_family, jlink_arm_dll_path=None, log=False, log_str=None, log_file_path=None):
        """
        Constructor. Initializes multiprocessing queues, creates a subprocess for the API instance and runs it.

        @param enum string or int device_family:    The series of device pynrfjprog will interact with.
        @param optional string jlink_arm_dll_path: Absolute path to the JLinkARM DLL that you want nrfjprog to use.
        @param optional bool log:                  If present and true, will enable logging to sys.stderr with the default log string appended to the beginning of each debug output line.
        @param optional string log_str:            If present, will enable logging to sys.stderr with overwriten default log string appended to the beginning of each debug output line.
        @param optional string log_file_path:      If present, will enable logging to log_file specified. This file will be opened in write mode in API.__init__() and closed when api.close() is called.
        """
        self.CmdQueue = multiprocessing.Queue()
        self.CmdAckQueue = multiprocessing.Queue()

        if DEBUG_OUTPUT:
            log = True

        self.runner = multiprocessing.Process(target=self._runner, args=(device_family, jlink_arm_dll_path, log, log_str, log_file_path))
        self.runner.daemon = True
        self.runner.start()

    def __getattr__(self, name):
        if hasattr(API.API, name):
            return lambda *args, **kwargs: self._execute(name, *args, **kwargs)
        else:
            raise AttributeError

    def _execute(self, func_name, *args, **kwargs):
        """Passes method and argument to _runner for execution.
        Calls API.API.func_name(*args, **kwargs) in a separate thread.
        Passes the method to be called to the runner, and awaits a return message.
        Reraises any exceptions that occurred during execution.
        Returns the return value of the executed method.

        @param func_name:  Name of API method member to call.
        @param args:       Arguments of func_name.
        @param kwargs:     Named arguments of func_name.
        @returns:          Return value of API.API.func_name(*args, **kwargs)
        """
        self.CmdQueue.put(_Command(func_name, *args, **kwargs))

        ack = self.CmdAckQueue.get()

        if ack.exception is not None:
            print(ack.stacktrace)
            raise ack.exception
        if ack.result is not None:
            return ack.result

    def _runner(self, device_family, jlink_arm_dll_path, log, log_str, log_file):
        """Runs methods in separate thread.
        Attempts to call any method received from _execute through CmdQueue as a member of API.API class.
        Return values and exceptions are passed back through the CmdAckQueue to _execute.

        @param device_family:        Family of target device.
        @param jlink_arm_dll_path:   Path to target jlinkarm DLL.
        @param log:                  Whether or not API should log.
        @param log_str:              Prepend string to log output.
        @param log_file:             Target file for log output.
        """
        api = self._api_setup(device_family, jlink_arm_dll_path, log, log_str, log_file)
        api_functions = dict(inspect.getmembers(api, inspect.ismethod))

        while True:
            cmd = self.CmdQueue.get()
            try:
                res = api_functions[cmd.cmd](*cmd.args, **cmd.kwargs)
            except Exception as e:
                self.CmdAckQueue.put(_CommandAck(exception=e, stacktrace=traceback.format_exc()))
            else:
                self.CmdAckQueue.put(_CommandAck(result=res))

    def _api_setup(self, device_family, jlink_arm_dll_path, log, log_str, log_file):
        """Instantiates a new API.API object. Called in the _runner thread."""
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
