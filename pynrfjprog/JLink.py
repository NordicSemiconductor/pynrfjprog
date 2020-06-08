"""
This module provides some utility functions to locate a SEGGER JLINK dll. 

It is no longer used in LowLevel.API or MultiAPI.API, but maintained and tested in case it is still in use with third parties.

These functions expect a standard OS installation and a default installation path for the SEGGER shared library. If these 
conditions are not met the absolute path of the SEGGER JLinkARM shared library must be provided when instantiating an
API or MultiAPI object.
"""

import fnmatch
import os
import sys


if sys.platform.lower().startswith('win'):
    _DEFAULT_SEGGER_ROOT_PATH = os.path.join(os.environ['PROGRAMFILES(X86)'], 'SEGGER') if 'PROGRAMFILES(X86)' in os.environ else os.path.join(os.environ['PROGRAMFILES'], 'SEGGER')
elif sys.platform.lower().startswith('linux'):
    _DEFAULT_SEGGER_ROOT_PATH = r'/opt/SEGGER/JLink'
elif sys.platform.lower().startswith('dar'):
    _DEFAULT_SEGGER_ROOT_PATH = r'/Applications/SEGGER/JLink'


def find_latest_dll():
    if not os.path.isdir(_DEFAULT_SEGGER_ROOT_PATH):
        return None

    if sys.platform.lower().startswith('win'):
        # Newest Segger J-Link software is now installed in the folder "JLink"
        # If the "JLink" folder does not exist, look for the "highest" folder that matches JLink_V*
        if os.path.exists(os.path.join(_DEFAULT_SEGGER_ROOT_PATH, 'JLink')):
            jlink_sw_dir = os.path.join(_DEFAULT_SEGGER_ROOT_PATH, 'JLink')
        else:
            jlink_sw_dir = sorted([f for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'JLink_V*')])[-1]

        jlink_file_name = 'JLink_x64.dll' if sys.maxsize > 2 ** 32 else 'JLinkARM.dll'
        # Walk through latest Segger JLinkARM installation folder in search of the appropriate DLL.
        for root, dirs, files in os.walk(os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_sw_dir)):
            if jlink_file_name in files:
                return os.path.join(root, jlink_file_name)

    elif sys.platform.lower().startswith('linux'):
        # Adding .dummy to filenames in linux (.so.x.x.x.dummy) because python compare strings will then work properly with the version number compare.
        jlink_so_files = sorted([f + ".dummy" for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'libjlinkarm.so*')])
        return os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_so_files[-1][:-len(".dummy")])

    elif sys.platform.lower().startswith('dar'):
        jlink_dylib_files = sorted([f for f in os.listdir(_DEFAULT_SEGGER_ROOT_PATH) if fnmatch.fnmatch(f, 'libjlinkarm.*dylib')])
        return os.path.join(_DEFAULT_SEGGER_ROOT_PATH, jlink_dylib_files[-1])
    