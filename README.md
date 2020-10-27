![PyPI](https://img.shields.io/static/v1?label=license&message=Nordic%205-Clause%20License&color=brightgreen)
![PyPI](https://img.shields.io/static/v1?label=platform&message=windows%20%7C%20linux%20%7C%20osx&color=lightgrey)
![PyPI](https://img.shields.io/static/v1?label=python&message=python-2.7%20%7C%20>=3.4&color=blue) ![PyPI](https://img.shields.io/pypi/v/pynrfjprog)

# pynrfjprog
Python wrapper around the nrfjprog dynamic link libraries (DLL). Use of this API allows developers to program/debug nRF SOC and SIP devices from the interpreter, write simple scripts for a more efficient development work flow, or write automated test frameworks. It can also be used to create applications in Python (i.e. command-line tools).

## Use-cases
*  Maximizing development efficiency: i.e. a script to perform various operations every time an application is built and run (could be hooked into a Makefile or automated build system etc...).
*  Automated testing: [Testing Production Programming tools on nRF5 using pynrfjprog](https://github.com/NordicSemiconductor/nrf52-production-programming/blob/master/tests/example_test_script.py).
*  PC Tools: [nrfjprog.exe implemented via pynrfjprog and argparse](https://github.com/NordicSemiconductor/nrfjprog.git).

## Dependencies
```python
"""
Detailed below is how our software is stacked. Each layer depends on the layer below.
"""
pynrfjprog  # Imports and wraps the nrfjprog DLL in Python.
nrfjprogdll # A DLL that wraps SEGGER's JLink API for nRF5 devices.
JLinkARMDLL # A DLL provided by SEGGER that works with SEGGER debuggers. Performs all low level operations with target device.
```

* [J-Link Software and Documentation Pack](https://www.segger.com/jlink-software.html) will install the JLink libraries pynrfjprog depends on in the correct installation directory.
* The nrfjprog libraries are installed with pynrfjprog and are included in pynrfjprog/OPERATING_SYSTEM/.

## Structure
```pynrfjprog
pynrfjprog
  ├── pynrfjprog
  │     ├──__init__.py    # Package marker to make pynrfjprog a module. Also defines the version number
  │     ├── API.py        # Legacy name of LowLevel.py. It's kept for backward support
  │     ├── APIError.py   # Wrapper for the error return codes of the DLL
  │     ├── Hex.py        # Hex parsing library
  │     ├── HighLevel.py  # Wrapper for the nrfjprog highlevel DLL
  │     ├── JLink.py      # Finds the JLinkARM DLL required by pynrfjprog
  │     ├── LowLevel.py   # Wrapper for the nrfjprog DLL, previously API.py
  │     ├── MultiAPI.py   # Allow multiple devices (up to 128) to be programmed simultaneously with a LowLevel API
  │     ├── lib_x64
  │     │   └── # 64-bit nrfjprog libraries
  │     ├── lib_x86
  │     │   └── # 32-bit nrfjprog libraries
  │     ├── docs
  │     │   └── # Header files of the nrfjprog DLL to provide in-depth documentation of the functions that are wrapped
  │     └── examples
  │         └── # Example scripts to show off the different APIs
  ├── LICENSE
  ├── README.md
  ├── requirements.txt
  └── setup.py
  
  
    
```

## Getting started
To install latest release from PyPI:
```
pip install pynrfjprog
```
To install from source navigate to pynrfjprog\:
```
python setup.py install
```
Open the Python interpreter and connect nRF5 device to PC:
```
from pynrfjprog import API

api = API.API('NRF52')
api.open()
api.enum_emu_snr()
api.connect_to_emu_without_snr()
api.erase_all()
api.write_u32(ADDRESS, DATA, IS_FLASH)
api.disconnect_from_emu()
api.close()
```

To work with multiple nRF5 devices at once:
```
import MultiAPI as API

api = API.API('NRF52')
api.open()

api2 = API.API('NRF52')
api2.open()

api3 = API.API('NRF51')
api3.open()

api.close()
api2.close()
api3.close()
```
Note: MultiAPI has the same interface as API, it just allows you to create multiple instances of API.

To program hex files using the HighLevel API:
```
from pynrfjprog import HighLevel

api = HighLevel.API()
api.open()

# To program J-Link probe at snr <snr>:
probe = HighLevel.DebugProbe(api, <snr>)
probe.program(<hex_file>)
probe.close()

# To program MCUBoot target at serial port <serial>:
probe = HighLevel.MCUBootDFUProbe(api, <serial>)
probe.program(<hex_file>)
probe.close()

# To update LTE modem connected to J-Link prbe at snr <snr>:
probe = HighLevel.IPCDFUProbe(api, <snr>)
probe.program(<hex_file>)
probe.close()

api.close()
```
Note: Only one HighLevel API can be instantiated and opened at a time. But, several HighLevel probes can be opened from the same API at the same time, as long as you don't open two probes to the same target.

## Contributing
Contributing is encouraged along with the following coding standards.
* [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html)
* http://www.clifford.at/style.html
* [Semantic versioning](http://semver.org/)
