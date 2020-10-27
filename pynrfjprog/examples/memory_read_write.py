""" 

    This file contains example code meant to be used in order to test the 
    pynrfjprog API. If multiple devices are connected, a pop-up will appear
    for device selection.
     
    Sample program: memory_read_write.py
    
    Run from command line:  
        python memory_read_write.py  
    or if imported using "from pynrfjprog import examples"
        examples.memory_read_write.run()
    
    Program flow:
        0. An API object is instantiated with UNKNOWN family to read the device family.
        1. An API object is instantiated with the read family. A connection to the debug probe is established.
        2. Flash memory is erased.
        3. 0xDEADBEEF is written to address 0x0.
        4. 0xBAADF00D is written to address 0x10.
        5. Memory 0x0 and 0x10 is read and printed to console.

"""

from __future__ import print_function

# Import pynrfjprog API module
try:
    from .. import LowLevel
except Exception:
    from pynrfjprog import LowLevel


def run(snr=None):
    """
    Run example script.

    @param (optional) int snr: Specify serial number of DK to run example on.
    """
    print('# Memory read and write example using pynrfjprog started...')
    
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

        # Erase all the flash of the device.
        print('# Erasing all flash in the microcontroller.')
        api.erase_all()

        # Write to addresses 0x0 and 0x10.
        print('# Writing 0xDEADBEEF to memory address 0x0, use NVMC')
        api.write_u32(0x0, 0xDEADBEEF, True)
        print('# Writing 0xBAADF00D to memory address 0x10, use NVMC')
        api.write_u32(0x10, 0xBAADF00D, True)

        # Read from addresses 0x0 and 0x10 and print the result.
        print('# Reading memory address 0x0 and 0x10, and print to console')
        print('Address 0x0 contains: ', hex(api.read_u32(0x0)))
        print('Address 0x10 contains: ', hex(api.read_u32(0x10)))

        # Close the loaded DLL to free resources.
        api.close()

        print('# Example done...')

    except LowLevel.APIError:
        api.close()
        raise
    
 
if __name__ == '__main__':
    run()


