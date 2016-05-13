[![Build Status](https://travis-ci.org/mjdietzx/pynrfjprog.svg?branch=master)](https://travis-ci.org/mjdietzx/pynrfjprog)
[![PyPI](https://img.shields.io/pypi/l/Django.svg)](https://opensource.org/licenses/BSD-3-Clause)

# pynrfjprog
Python wrapper around the nrfjprog dynamic link library (DLL). Use of this API allows developers to program/debug nRF5 devices from the interpreter, write simple scripts for a more efficient development work flow, or write automated test frameworks. It can also be used to create applications in Python (i.e. command-line tools).

## Use-cases
*  Maximizing development efficiency: i.e. a script to perform various operations everytime an application is built and run (could be hooked into a Makefile or automated build system etc...).
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
pynrfjprog\
  # LICENSE, README.md, setup.py and requirements.txt (used to install this module).
  pynrfjprog\
    __init__.py # Package marker to make pynrfjprog a module. Also defines the version number.
    API.py # Wrapper around the nrfjprog DLL.
    MultiAPI.py # Allow multiple devices (up to 128) to be programmed simultaneiously.
    JLink.py # Finds the JLinkARM DLL required by pynrfjprog.
    Hex.py # DEPRECATED. Use [intelhex](https://pypi.python.org/pypi/IntelHex) instead.
      win_dll\ # nrfjprog libraries.
      osx_dylib\
      linux_64bit_so\
      linux_32bit_so\
      examples\ # Simple examples to demonstrate pynrfjprog's use.
      docs\ # Automatically generated documentation.
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
import API

api = API.API('NRF52')
api.open()
api.enum_smu_snr()
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

## Contributing
Contributing is encouraged along with the following coding standards.
* [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html)
* http://www.clifford.at/style.html
* [Semantic versioning](http://semver.org/)
