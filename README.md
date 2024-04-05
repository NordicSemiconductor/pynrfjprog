[![PyPI](https://img.shields.io/static/v1?label=license&message=Nordic%205-Clause%20License&color=brightgreen)](https://github.com/NordicSemiconductor/pynrfjprog/blob/master/LICENSE)
![PyPI](https://img.shields.io/static/v1?label=platform&message=windows%20%7C%20linux%20%7C%20osx&color=lightgrey)
![PyPI](https://img.shields.io/static/v1?label=python&message=>=3.7&color=blue) [![PyPI](https://img.shields.io/pypi/v/pynrfjprog)](https://pypi.org/project/pynrfjprog/)

# pynrfjprog
Python wrapper around the nrfjprog dynamic link libraries (DLL). Use of this API allows developers to program/debug nRF SOC and SIP devices from the interpreter, write simple scripts for a more efficient development work flow, or write automated test frameworks. It can also be used to create applications in Python (i.e. command-line tools).

## Use-cases
*  Maximizing development efficiency: i.e. a script to perform various operations every time an application is built and run (could be hooked into a Makefile or automated build system etc...).
*  Automated testing: [Testing Production Programming tools on nRF5 using pynrfjprog](https://github.com/NordicSemiconductor/nrf52-production-programming/blob/master/tests/example_test_script.py).

## Dependencies
* [SEGGER JLink DLL] (https://www.segger.com/jlink-software.html)
* (Windows-only) [Microsoft Visual C++ Redistributable] (https://learn.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist)

## Structure
```pynrfjprog
pynrfjprog
  ├── pynrfjprog
  │     ├──__init__.py    # Package marker to make pynrfjprog a module. Also defines the version number
  │     ├── API.py        # (Deprecated; included only for backwards compatibility) Alias for LowLevel.py
  │     ├── APIError.py   # Wrapper for the error return codes of the DLL
  │     ├── Hex.py        # (Deprecated; included only for backwards compatibility) Hex parsing library
  │     ├── HighLevel.py  # (Deprecated; included only for backwards compatibility) Wrapper for the nrfjprog highlevel DLL
  │     ├── JLink.py      # (Deprecated; included only for backwards compatibility) Finds the JLinkARM DLL
  │     ├── LowLevel.py   # Wrapper for the nrfjprog DLL, previously API.py
  │     ├── MultiAPI.py   # Allow multiple devices (up to 128) to be programmed simultaneously with a LowLevel API
  │     ├── lib_armhf
  │     │   └── # armhf nrfjprog libraries
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
  └── pyproject.toml
```

## Getting started
To install the latest release from PyPI:
```
python -m pip install pynrfjprog
```
To install from source:
```
python -m pip install path_to_unzipped_pynrfjprog
```
Open the Python interpreter and connect nRF device to PC:
```
from pynrfjprog import LowLevel

with LowLevel.API('NRF52') as api:
    api.enum_emu_snr()
    api.connect_to_emu_without_snr()
    api.erase_all()
    api.write_u32(ADDRESS, DATA, IS_FLASH)
    api.disconnect_from_emu()
```

To work with multiple nRF devices at once:
```
import LowLevel

api = LowLevel.API('NRF52')
api.open()

api2 = LowLevel.API('NRF52')
api2.open()

api3 = LowLevel.API('NRF52')
api3.open()

api.close()
api2.close()
api3.close()
```

To program firmware into the devices:
```
from pynrfjprog import LowLevel

with LowLevel.API() as api:
    api.program_file(<hex_file>)

    # Optional
    api.verify_file(<hex_file>)
```

## Contributing
Contributing is encouraged along with the following coding standards.
* [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html)
* http://www.clifford.at/style.html
* [Semantic versioning](http://semver.org/)
