"""
Package marker file.

"""

__version__ = "10.24.0" 

from pathlib import Path

import platform
import stat
import logging


def set_worker_executable(on_load=False):
    os_name = platform.system().lower()

    if os_name == 'linux' or os_name == 'darwin':
        # On linux and MacOS we need to make sure the jlinkarm worker is executable.
        try:
            worker_bin_names = {
                'linux': 'jlinkarm_nrf_worker_linux',
                'darwin': 'jlinkarm_nrf_worker_osx'
            }

            worker_bin_name = worker_bin_names[os_name]

            for worker_binary in Path(__file__).parent.glob(f"lib_*/{worker_bin_name}"):
                mode = worker_binary.stat().st_mode
                expected = stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH

                if (mode & expected) != expected:
                    logging.info(f"Marking {worker_binary} as executable")
                    worker_binary.chmod(expected | mode)
        except Exception:
            if on_load:
                raise RuntimeError(
                    "jlinkarm_nrf_worker is not executable, and could not be made executable. \
                    Try doing this manually with python -m pynrfjprog.set_worker_executable"
                )

set_worker_executable(True)
