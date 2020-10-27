"""
MultiAPI module.

"""

from __future__ import print_function

import inspect
import traceback
import multiprocessing
import threading
import sys

try:
    from . import LowLevel
    from . import APIError
except Exception:
    import LowLevel
    import APIError

# For backwards compatibility
API = LowLevel


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

    def __init__(self, device_family, jlink_arm_dll_path=None, log=False, log_str=None, log_file_path=None, api_lock_factory=threading.Lock):
        """
        Constructor. Initializes multiprocessing queues, creates a subprocess for the API instance and runs it.

        @param enum string or int device_family:   The series of device pynrfjprog will interact with.
        @param optional string jlink_arm_dll_path: Absolute path to the JLinkARM DLL that you want nrfjprog to use. Must be provided if your environment is not standard or your SEGGER installation path is not the default path. See JLink.py for details. Does not support unicode paths.
        @param optional bool log:                  If present and true, will enable logging to sys.stderr with the default log string appended to the beginning of each debug output line.
        @param optional string log_str:            If present, will enable logging to sys.stderr with overwritten default log string appended to the beginning of each debug output line.
        @param optional string log_file_path:      If present, will enable logging to log_file specified. This file will be opened in write mode in API.__init__() and closed when api.close() is called.
        @param optional function api_lock_factory: A factory function that produces a synchronization primitive with a context manager interface that will be used to guard accesses to the runner thread. By default a threading Lock is used.
                                                   If a different synchronization primitive is necessary, for example if accessing MultiAPI from a separate multiprocessing thread, or using asyncio, provide an appropriate replacement.
        """

        if hasattr(multiprocessing, "get_all_start_methods"):
            start_methods = multiprocessing.get_all_start_methods()
            if "forkserver" in start_methods:
                mp_context = multiprocessing.get_context('forkserver')
                fork_no_exec = False

            elif "spawn" in start_methods:
                mp_context = multiprocessing.get_context('spawn')
                fork_no_exec = False
            else:
                mp_context = multiprocessing.get_context('fork')
                fork_no_exec = True
        else:
            mp_context = multiprocessing
            fork_no_exec = not sys.platform.lower().startswith('win')

        # If we're using fork without exec, and the requested API is already instantiated, fail.
        if LowLevel.API.is_instantiated() and fork_no_exec:
            raise APIError.APIError(APIError.NrfjprogdllErr.PYTHON_ALREADY_INSTANTIATED_ERROR, "LowLevel.API is already instantiated in this thread.")

        self._CmdPipeRunner, self._CmdPipeHost = mp_context.Pipe()
        self._CmdAckPipeHost,  self._CmdAckPipeRunner = mp_context.Pipe()
        self._api_lock_factory = api_lock_factory
        self._api_lock = None

        self._runner_process = mp_context.Process(target=self._runner, args=(device_family, jlink_arm_dll_path, log, log_str, log_file_path))
        self._runner_process.daemon = True
        self._runner_process.start()

        self._terminated = False

    def __getattr__(self, name):
        if hasattr(API.API, name):
            return lambda *args, **kwargs: self._execute(name, *args, **kwargs)
        else:
            raise AttributeError("'MultiAPI' or 'LowLevel' object has no attribute '{}'".format(name))

    def is_alive(self):
        """Checks if MultiAPI is still alive.
        As long as instance is alive, it is able to execute API commands.
        MultiAPI is alive from instantiation until terminate() is called.
        """
        return not self._terminated

    @staticmethod
    def fork_no_exec():
        if hasattr(multiprocessing, "get_all_start_methods"):
            start_methods = multiprocessing.get_all_start_methods()
            if "forkserver" in start_methods:
                return False

            elif "spawn" in start_methods:
                return False
            else:
                return True
        else:
            return not sys.platform.lower().startswith('win')

    def terminate(self):
        """Terminates all background processes and threads.
        Calls process.terminate() on all running background processes.
        Closes all multiprocessing queues to stop background threads.
        After terminate() any public member function except is_alive() and terminate() will fail.
        """
        if self.is_alive():
            self._runner_process.terminate()
            self._CmdPipeRunner.close()
            self._CmdPipeHost.close()
            self._CmdAckPipeRunner.close()
            self._CmdAckPipeHost.close()

            self._runner_process.join()

            self._terminated = True

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

        if self._api_lock is None:
            self._api_lock = self._api_lock_factory()

        if not self.is_alive():
            raise APIError.APIError(APIError.NrfjprogdllErr.INVALID_OPERATION.value, "Runner process terminated, API is unavailable.")

        with self._api_lock:
            self._CmdPipeHost.send(_Command(func_name, *args, **kwargs))

            ack = self._CmdAckPipeHost.recv()

            if ack.exception is not None:
                print(ack.stacktrace)
                raise ack.exception
            if ack.result is not None:
                return ack.result

    def _runner(self, device_family, jlink_arm_dll_path, log, log_str, log_file):
        """Runs methods in separate thread.
        Attempts to call any method received from _execute through _CmdQueue as a member of api class.
        Return values and exceptions are passed back through the _CmdAckQueue to _execute.

        @param device_family:        Family of target device.
        @param jlink_arm_dll_path:   Path to target jlinkarm DLL. Does not support unicode paths.
        @param log:                  Whether or not API should log.
        @param log_str:              Prepend string to log output.
        @param log_file:             Target file for log output.
        """
        api = LowLevel.API(device_family, jlink_arm_dll_path, log=log, log_str=log_str, log_file_path=log_file)
        api_functions = dict(inspect.getmembers(api, inspect.ismethod))

        while True:
            cmd = self._CmdPipeRunner.recv()
            try:
                res = api_functions[cmd.cmd](*cmd.args, **cmd.kwargs)
            except Exception as e:
                self._CmdAckPipeRunner.send(_CommandAck(exception=e, stacktrace=traceback.format_exc()))
            else:
                # Instances of api will be returned by __enter__, and cannot be pickled, they should not be returned.
                if isinstance(res, type(api)):
                    res = None

                self._CmdAckPipeRunner.send(_CommandAck(result=res))

    def __enter__(self):
        self._execute("__enter__")
        return self

    def __exit__(self, type, value, traceback):
        # Run sub-exit. We can't pickle tracebacks, so it is not passed.
        self._execute("__exit__", None, None, None)
        self.terminate()

