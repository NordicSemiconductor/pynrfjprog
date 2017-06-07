"""
This module defines one function that returns the absolute path to the JLinkARM.dll (if it exists).

"""

import fnmatch
import os
import sys


if sys.platform.lower().startswith('win'):
    _DEFAULT_SEGGER_ROOT_PATH = r'C:\Program Files (x86)\SEGGER' if 'PROGRAMFILES(X86)' in os.environ else r'C:\Program Files\SEGGER'
elif sys.platform.lower().startswith('linux'):
    _DEFAULT_SEGGER_ROOT_PATH = r'/opt/SEGGER/JLink'
elif sys.platform.lower().startswith('dar'):
    _DEFAULT_SEGGER_ROOT_PATH = r'/Applications/SEGGER/JLink'


def find_latest_dll():

    if not os.path.isdir(_DEFAULT_SEGGER_ROOT_PATH):
        return None

    if sys.platform.lower().startswith('win'):
        jlink_sw_dirs = sorted([f for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'JLink_V*')])
        jlink_file_name = 'JLink_x64.dll' if sys.maxsize > 2**32 else 'JLinkARM.dll'
        # Walk through latest Segger JLinkARM installation folder in search of the appropriate DLL.
        for root, dirs, files in os.walk(os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_sw_dirs[-1])):
            if jlink_file_name in files:
                return os.path.join(root, jlink_file_name)
        
    elif sys.platform.lower().startswith('linux'):
        # Adding .dummy to filenames in linux (.so.x.x.x.dummy) because python compare strings will then work properly with the version number compare.
        jlink_so_files = sorted([f + ".dummy" for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'libjlinkarm.so*')])
        return os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_so_files[-1][:-len(".dummy")])

    elif sys.platform.lower().startswith('dar'):
        jlink_dylib_files = sorted([f for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'libjlinkarm.*dylib')])
        return os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_dylib_files[-1])
