"""
Package marker file.

"""

__version__ = "10.23.1" 

import sys
from pathlib import Path
import stat
import logging


def set_worker_executable(on_load=False):
    os_name = sys.platform.lower()
    if os_name.startswith("dar") or os_name.startswith("linux"):
        # On linux and MacOS we need to make sure the jlinkarm worker is executable
        try:
            for lib_path in Path(__file__).parent.glob("lib_*/jlinkarm_nrf_worker*"):
                mode = lib_path.stat().st_mode
                expected = stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH
                if (mode & expected) != expected:
                    logging.info(f"Mark {lib_path} as executable")
                    lib_path.chmod(expected | mode)
        except Exception:
            if on_load:
                raise RuntimeError(
                    "jlinkarm_nrf_worker is not executable, and could not be made executable. \
                    Try doing this manually with python -m pynrfjprog.set_worker_executable"
                )


set_worker_executable(True)
