""" 

    This file contains example code meant to be used in order to test the 
    pynrfjprog API and Hex modules. If multiple devices are connected, a 
    pop-up will appear for device selection.
    
    Sample program: program_hex.py
    Requires an nRF Development Kit for visual confirmation (LEDs).
    
    Run from command line:  
        python program_hex.py  
    or if imported as "from pynrfjprog import examples"
        examples.program_hex.run()
    
    Program flow:
        0. An API object is instantiated with UNKNOWN family to read the device family.
        1. An API object is instantiated with the read family. A connection to the debug probe is established.
        2. Flash memory is erased.
        3. A hex file is parsed and programmed into memory.
        4. The device is reset and application is run.

"""

from __future__ import print_function
import os

# Import pynrfjprog API module and HEX parser module
try:
    from .. import LowLevel, Hex
except Exception:
    from pynrfjprog import LowLevel, Hex

def find_blinky_hex(device_family, device_version):
    """ Find the appropriate hex file to program """

    module_dir, module_file = os.path.split(__file__)
    hex_file_path = os.path.join(os.path.abspath(module_dir), device_family.lower() + "_dk_blinky.hex")

    if os.path.exists(hex_file_path):
        return hex_file_path

    device_version_short = device_version.split('_')[0]
    hex_file_path = os.path.join(os.path.abspath(module_dir), device_version_short.lower() + "_dk_blinky.hex")

    if os.path.exists(hex_file_path):
        return hex_file_path

    return None


def run(snr=None):
    """
    Run example script.

    @param (optional) int snr: Specify serial number of DK to run example on.
    """
    print('# Hex file programming example using pynrfjprog started...')
    
    # Detect the device family of your device. Initialize an API object with UNKNOWN family and read the device's family. This step is performed so this example can be run in all devices without customer input.
    print('# Opening API with device family UNKNOWN, reading the device family.')
    with LowLevel.API(LowLevel.DeviceFamily.UNKNOWN) as api:            # Using with construction so there is no need to open or close the API class.
        if snr is not None:
            api.connect_to_emu_with_snr(snr)
        else:
            api.connect_to_emu_without_snr()
        device_family = api.read_device_family()
    
    # Initialize an API object with the target family. This will load nrfjprog.dll with the proper target family.
    api = LowLevel.API(device_family)
    
    # Open the loaded DLL and connect to an emulator probe. If several are connected a pop up will appear.
    api.open()

    try:
        if snr is not None:
            api.connect_to_emu_with_snr(snr)
        else:
            api.connect_to_emu_without_snr()

        device_version = api.read_device_version()

        hex_file_path = find_blinky_hex(device_family, device_version)

        if hex_file_path is None:
            api.close()
            raise Exception("Could not find example binary for device " + device_version.lower() + ".\n" +
                            "This example may not support your device yet.")

        # Erase all the flash of the device.
        print('# Erasing all flash in the microcontroller.')
        api.erase_all()

        # Parse the hex file with the help of the HEX module
        print('# Parsing hex file into segments.')
        test_program = Hex.Hex(hex_file_path)

        # Program the parsed hex into the device's memory.
        print('# Writing %s to device.' % hex_file_path)
        for segment in test_program:
            api.write(segment.address, segment.data, True)

        # Reset the device and run.
        api.sys_reset()
        api.go()
        print('# Application running. Your board should be blinking.')

        # Close the loaded DLL to free resources.
        api.close()

        print('# Example done...')

    except LowLevel.APIError:
        api.close()
        raise
    
 
if __name__ == '__main__':
    run()


