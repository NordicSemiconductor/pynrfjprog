
from __future__ import print_function
from builtins import int

import enum
import ctypes
import codecs
import sys
import datetime


NRFJPROG_MAX_PATH = 260
NRFJPROG_STRING_LENGTH = 256
NRFJPROG_COM_PER_JLINK = 10


def decode_string(string):
    """ This function ensures that string output from ctypes is normalized to the local string type. """
    if sys.version_info[0] == 2 or isinstance(string, str):
        return string
    else:
        return string.decode('utf-8',errors='replace')


###################################################################################
#                                                                                 #
#                              Logger class                                       #
#                                                                                 #
###################################################################################


class Logger(object):
    def __init__(self, log=None, log_str_cb=None, log_str=None,  log_file_path=None, log_stringio=None):
        """
        Setup API's debug output logging mechanism.

        """
        self.logger_owns_file = False
        self.log_file = None
        self.log = log
        self.log_str = log_str

        if log_str_cb is not None:
            self.log_func = lambda x: log_str_cb(decode_string(x).strip())
            self.log = True
        else:
            if log_str is None:
                self.log_str = '[NRFJPROG LOG]:'
            else:
                self.log_str = log_str
                self.log = True

            if log_stringio is not None:
                self.log_file = log_stringio
                self.log = True
            elif log_file_path is not None:
                self.log_file = open(log_file_path, 'a', 1)
                self.logger_owns_file = True
                self.log = True
            elif self.log:
                self.log_file = sys.stderr

            self.log_func = self.log_function

        if self.log:
            self.log_func('-----------------------------------------------------------------------')
            self.log_cb = Logger.create_callback(self.log_func)
        else:
            self.log_cb = None
            self.log_func = None

    def __enter__(self):
        self.open()

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def open(self):
        if self.logger_owns_file and self.log_file.closed:
            self.log_file = open(self.log_file.name, 'a', 1)

    def close(self):
        if self.logger_owns_file and not self.log_file.closed:
            self.log_file.close()

    def log_function(self, msg):
        print('{} {} {}'.format(datetime.datetime.now().replace(microsecond=0).isoformat(' '), self.log_str, msg), file=self.log_file)

    @staticmethod
    def create_callback(log_func):
        return ctypes.CFUNCTYPE(None, ctypes.c_char_p)(lambda msg: log_func(decode_string(msg).strip()))


###################################################################################
#                                                                                 #
#                              Low level data types                               #
#                                                                                 #
###################################################################################

# Helper functions for validating data types
def is_u32(value):
    return isinstance(value, int) and 0 <= value <= 0xFFFFFFFF


def is_u8(value):
    return isinstance(value, int) and 0 <= value <= 0xFF


def is_bool(value):
    return isinstance(value, bool) or 0 <= value <= 1


def is_valid_buf(buf):
    if buf is None:
        return False
    for value in buf:
        if not is_u8(value):
            return False
    return len(buf) > 0


def is_valid_encoding(encoding):
    try:
        codecs.lookup(encoding)
    except LookupError:
        return False
    else:
        return True


def is_enum(param, enum_type):
    if isinstance(param, enum_type):
        return True
    if isinstance(param, int) and param in [member for name, member in enum_type.__members__.items()]:
        return True
    elif isinstance(param, str) and param in [name for name, member in enum_type.__members__.items()]:
        return True
    return False


def is_right_class(instance, class_type):
    if instance is None:
        return False
    return isinstance(instance, class_type)


def decode_enum(param, enum_type):
    if not is_enum(param, enum_type):
        return None

    if isinstance(param, int):
        return enum_type(param)
    elif isinstance(param, str):
        return enum_type[param]
    else:
        return param


@enum.unique
class DeviceFamily(enum.IntEnum):
    """
    Wraps device_family_t values from DllCommonDefinitions.h

    """

    NRF51              = 0
    NRF52              = 1
    NRF53              = 53
    NRF91              = 91
    UNKNOWN            = 99


class DeviceVersion(enum.IntEnum):
    """
    Wraps device_version_t values from DllCommonDefinitions.h

    """

    UNKNOWN                 = 0

    NRF51xxx_xxAA_REV1      = 1
    NRF51xxx_xxAA_REV2      = 2
    NRF51xxx_xxAA_REV3      = 3
    NRF51xxx_xxAB_REV3      = 4
    NRF51xxx_xxAC_REV3      = 5
    NRF51802_xxAA_REV3      = 6
    NRF51801_xxAB_REV3      = 17

    NRF52805_xxAA_REV1      = 0x05280500
    NRF52805_xxAA_FUTURE    = 0x052805FF

    NRF52810_xxAA_REV1      = 13
    NRF52810_xxAA_REV2      = 0x05281001
    NRF52810_xxAA_FUTURE    = 14

    NRF52811_xxAA_REV1      = 0x05281100
    NRF52811_xxAA_FUTURE    = 0x052811FF

    NRF52820_xxAA_REV1      = 0x05282000
    NRF52820_xxAA_REV2      = 0x05282001
    NRF52820_xxAA_FUTURE    = 0x052820FF

    NRF52832_xxAA_ENGA      = 7
    NRF52832_xxAA_ENGB      = 8
    NRF52832_xxAA_REV1      = 9
    NRF52832_xxAA_REV2      = 19
    NRF52832_xxAA_REV3      = 0x5283201
    NRF52832_xxAA_FUTURE    = 11

    NRF52832_xxAB_REV1      = 15
    NRF52832_xxAB_REV2      = 20
    NRF52832_xxAB_FUTURE    = 16

    NRF52833_xxAA_REV1      = 0x05283300
    NRF52833_xxAA_FUTURE    = 0x052833FF

    NRF52840_xxAA_ENGA      = 10
    NRF52840_xxAA_ENGB      = 21
    NRF52840_xxAA_REV1      = 18
    NRF52840_xxAA_REV2      = 0x05284003
    NRF52840_xxAA_FUTURE    = 12
    
    NRF5340_xxAA_ENGA       = 0x05340000
    NRF5340_xxAA_ENGB       = 0x05340001
    NRF5340_xxAA_ENGC       = 0x05340002
    NRF5340_xxAA_ENGD       = 0x05340003
    NRF5340_xxAA_FUTURE     = 0x053400FF

    NRF9160_xxAA_REV1       = 0x09160000
    NRF9160_xxAA_REV2       = 0x09160001
    NRF9160_xxAA_FUTURE     = 0x091600FF


class DeviceName(enum.IntEnum):

    UNKNOWN_NAME = 0x0000000

    NRF51xxx = 0x05100000

    NRF51802 = 0x05180200
    NRF51801 = 0x05180100

    NRF52805 = 0x05280500
    NRF52810 = 0x05281000
    NRF52811 = 0x05281100
    NRF52820 = 0x05282000
    NRF52832 = 0x05283200
    NRF52833 = 0x05283300
    NRF52840 = 0x05284000

    NRF5340 = 0x05340000

    NRF9160 = 0x09160000


class DeviceMemory(enum.IntEnum):

    UNKNOWN_MEM     = 0
    AA              = 1
    AB              = 2
    AC              = 3


class DeviceRevision(enum.IntEnum):

    UNKNOWN_REV     = 0
    ENGA            = 10
    ENGB            = 11
    REV1            = 20
    REV2            = 21
    REV3            = 22
    FUTURE          = 30

@enum.unique
class CoProcessor(enum.IntEnum):
    """
    Wraps coprocessor_t values from DllCommonDefinitions.h
    """
    CP_APPLICATION = 0
    CP_MODEM = 1
    CP_NETWORK = 2

@enum.unique
class Region0Source(enum.IntEnum):
    """
    Wraps region_0_source_t values from DllCommonDefinitions.h

    """
    NO_REGION_0             = 0
    FACTORY                 = 1
    USER                    = 2


@enum.unique
class RamPower(enum.IntEnum):
    """
    Wraps ram_power_status_t values from DllCommonDefinitions.h

    """
    OFF                     = 0
    ON                      = 1


@enum.unique
class RTTChannelDirection(enum.IntEnum):
    """
    Wraps rtt_direction_t values from DllCommonDefinitions.h

    """
    UP_DIRECTION            = 0
    DOWN_DIRECTION          = 1


@enum.unique
class ReadbackProtection(enum.IntEnum):
    """
    Wraps readback_protection_status_t values from DllCommonDefinitions.h
    Some modes are not available in all devices.

    """
    NONE                    = 0
    REGION_0                = 1
    ALL                     = 2
    BOTH                    = 3
    SECURE                  = 4


class CpuRegister(enum.IntEnum):
    """
    Wraps cpu_registers_t values from DllCommonDefinitions.h

    """
    R0                      = 0
    R1                      = 1
    R2                      = 2
    R3                      = 3
    R4                      = 4
    R5                      = 5
    R6                      = 6
    R7                      = 7
    R8                      = 8
    R9                      = 9
    R10                     = 10
    R11                     = 11
    R12                     = 12
    R13                     = 13
    SP                      = 13
    R14                     = 14
    LR                      = 14
    R15                     = 15
    PC                      = 15
    XPSR                    = 16
    MSP                     = 17
    PSP                     = 18


###################################################################################
#                                                                                 #
#                                 QSPI data types                                 #
#                                                                                 #
###################################################################################


@enum.unique
class QSPIEraseLen(enum.IntEnum):
    """
    Wraps qspi_erase_len_t values from DllCommonDefinitions.h

    """
    ERASE4KB        = 0
    ERASE32KB       = 3
    ERASE64KB       = 1
    ERASEALL        = 2


@enum.unique
class QSPIReadMode(enum.IntEnum):
    """
    Wraps qspi_read_mode_t values from DllCommonDefinitions.h

    """
    FASTREAD        = 0
    READ2O          = 1
    READ2IO         = 2
    READ4O          = 3
    READ4IO         = 4


@enum.unique
class QSPIWriteMode(enum.IntEnum):
    """
    Wraps qspi_write_mode_t values from DllCommonDefinitions.h

    """
    PP              = 0
    PP2O            = 1
    PP4O            = 2
    PP4IO           = 3


@enum.unique
class QSPIAddressMode(enum.IntEnum):
    """
    Wraps qspi_address_mode_t values from DllCommonDefinitions.h

    """
    BIT24           = 0
    BIT32           = 1


@enum.unique
class QSPIFrequency(enum.IntEnum):
    """
    Wraps qspi_frequency_t values from DllCommonDefinitions.h

    """
    M2              =  15
    M4              =  7
    M8              =  3
    M16             =  1
    M32             =  0
    M64             = -1
    M96             = -2


@enum.unique
class QSPISpiMode(enum.IntEnum):
    """
    Wraps qspi_spi_mode_t values from DllCommonDefinitions.h
    """
    MODE0           = 0
    MODE3           = 1


@enum.unique
class QSPILevelIO(enum.IntEnum):
    """
    Wraps qspi_custom_level_t values from DllCommonDefinitions.h
    """
    LEVEL_HIGH      = 1
    LEVEL_LOW       = 0


@enum.unique
class QSPIPPSize(enum.IntEnum):
    """
    Wraps qspi_custom_level_t values from DllCommonDefinitions.h
    """
    PAGE256         = 0
    PAGE512         = 1


class QSPIInitParams(ctypes.Structure):
    _fields_ = [("read_mode",                    ctypes.c_int),
                ("write_mode",                   ctypes.c_int),
                ("address_mode",                 ctypes.c_int),
                ("frequency",                    ctypes.c_int),
                ("spi_mode",                     ctypes.c_int),
                ("sck_delay",                    ctypes.c_uint32),
                ("custom_instruction_io2_level", ctypes.c_int),
                ("custom_instruction_io3_level", ctypes.c_int),
                ("CSN_pin",                      ctypes.c_uint32),
                ("CSN_port",                     ctypes.c_uint32),
                ("SCK_pin",                      ctypes.c_uint32),
                ("SCK_port",                     ctypes.c_uint32),
                ("DIO0_pin",                     ctypes.c_uint32),
                ("DIO0_port",                    ctypes.c_uint32),
                ("DIO1_pin",                     ctypes.c_uint32),
                ("DIO1_port",                    ctypes.c_uint32),
                ("DIO2_pin",                     ctypes.c_uint32),
                ("DIO2_port",                    ctypes.c_uint32),
                ("DIO3_pin",                     ctypes.c_uint32),
                ("DIO3_port",                    ctypes.c_uint32),
                ("WIP_index",                    ctypes.c_uint32),
                ("pp_size",                      ctypes.c_int)]

    def __init__(self,
                 read_mode=QSPIReadMode.READ4IO,
                 write_mode=QSPIWriteMode.PP4IO,
                 address_mode=QSPIAddressMode.BIT24,
                 frequency=QSPIFrequency.M16,
                 spi_mode=QSPISpiMode.MODE0,
                 sck_delay=0x80,
                 custom_instruction_io2_level=QSPILevelIO.LEVEL_LOW,
                 custom_instruction_io3_level=QSPILevelIO.LEVEL_HIGH,
                 CSN_pin=17,
                 CSN_port=0,
                 SCK_pin=19,
                 SCK_port=0,
                 DIO0_pin=20,
                 DIO0_port=0,
                 DIO1_pin=21,
                 DIO1_port=0,
                 DIO2_pin=22,
                 DIO2_port=0,
                 DIO3_pin=23,
                 DIO3_port=0,
                 WIP_index=0,
                 pp_size=QSPIPPSize.PAGE256):

        ctypes.Structure.__init__(self)
        self.read_mode                    = read_mode
        self.write_mode                   = write_mode
        self.address_mode                 = address_mode
        self.frequency                    = frequency
        self.spi_mode                     = spi_mode
        self.sck_delay                    = sck_delay
        self.custom_instruction_io2_level = custom_instruction_io2_level
        self.custom_instruction_io3_level = custom_instruction_io3_level
        self.CSN_pin                      = CSN_pin
        self.CSN_port                     = CSN_port
        self.SCK_pin                      = SCK_pin
        self.SCK_port                     = SCK_port
        self.DIO0_pin                     = DIO0_pin
        self.DIO0_port                    = DIO0_port
        self.DIO1_pin                     = DIO1_pin
        self.DIO1_port                    = DIO1_port
        self.DIO2_pin                     = DIO2_pin
        self.DIO2_port                    = DIO2_port
        self.DIO3_pin                     = DIO3_pin
        self.DIO3_port                    = DIO3_port
        self.WIP_index                    = WIP_index
        self.pp_size                      = pp_size


class ComPortInfoStruct(ctypes.Structure):
    _fields_ =[("path",             ctypes.c_char * NRFJPROG_MAX_PATH),
               ("vcom",             ctypes.c_uint32),
               ("serial_number",    ctypes.c_uint32)]


class ComPortInfo(object):

    def __init__(self, probe_info):
        """ Info about a COM port. """
        self.path           = decode_string(probe_info.path)
        self.vcom           = probe_info.vcom
        self.serial_number  = probe_info.serial_number

    def __repr__(self):
        return "ComPortInfoStruct({}, {}, {})".format(self.path, self.vcom, self.serial_number)


###################################################################################
#                                                                                 #
#                              High level data types                              #
#                                                                                 #
###################################################################################


@enum.unique
class EraseAction(enum.IntEnum):
    """
    Available erase processes during programming.

    """
    ERASE_NONE              = 0
    ERASE_ALL               = 1
    ERASE_SECTOR            = 2
    ERASE_SECTOR_AND_UICR   = 3


@enum.unique
class ResetAction(enum.IntEnum):
    """
    Available reset modes.

    """
    RESET_NONE              = 0
    RESET_SYSTEM            = 1
    RESET_DEBUG             = 2
    RESET_PIN               = 3


@enum.unique
class VerifyAction(enum.IntEnum):
    """
    Available verify processes during programming.

    """
    VERIFY_NONE             = 0
    VERIFY_READ             = 1
    VERIFY_HASH             = 2


class ProgramOptions(ctypes.Structure):
    _fields_ = [("verify",            ctypes.c_int),
                ("erase_action",      ctypes.c_int),
                ("qspi_erase_action", ctypes.c_int),
                ("reset",             ctypes.c_int)]

    def __init__(self, verify=VerifyAction.VERIFY_NONE, erase_action=EraseAction.ERASE_ALL, qspi_erase_action=EraseAction.ERASE_NONE, reset=ResetAction.RESET_SYSTEM):
        ctypes.Structure.__init__(self)
        self.verify = verify
        self.erase_action = erase_action
        self.qspi_erase_action = qspi_erase_action
        self.reset = reset

    def __repr__(self):
        return "ProgramOptions({}, {}, {}, {})".format(
            self.verify,
            self.erase_action,
            self.qspi_erase_action,
            self.reset
        )


class ReadOptions(ctypes.Structure):
    _fields_ = [("readram",  ctypes.c_bool),
                ("readcode", ctypes.c_bool),
                ("readuicr", ctypes.c_bool),
                ("readficr", ctypes.c_bool),
                ("readqspi", ctypes.c_bool),
                ("reserved", ctypes.c_bool * 3)]

    def __init__(self, readram=False, readcode=False, readuicr=False, readficr=False, readqspi=False):
        ctypes.Structure.__init__(self)
        self.readram = readram
        self.readcode = readcode
        self.readuicr = readuicr
        self.readficr = readficr
        self.readqspi = readqspi

    def __repr__(self):
        return "ReadOptions({}, {}, {}, {}, {})".format(
            self.readram,
            self.readcode,
            self.readuicr,
            self.readficr,
            self.readqspi
        )


class DeviceInfoStruct(ctypes.Structure):
    _fields_ = [
        ("device_family",          ctypes.c_int),
        ("device_type",            ctypes.c_int),

        ("code_address",           ctypes.c_uint32),
        ("code_page_size",         ctypes.c_uint32),
        ("code_size",              ctypes.c_uint32),

        ("uicr_address",           ctypes.c_uint32),
        ("info_page_size",         ctypes.c_uint32),

        ("code_ram_present",       ctypes.c_bool),
        ("code_ram_address",       ctypes.c_uint32),
        ("data_ram_address",       ctypes.c_uint32),
        ("ram_size",               ctypes.c_uint32),

        ("qspi_present",           ctypes.c_bool),
        ("xip_address",            ctypes.c_uint32),
        ("xip_size",               ctypes.c_uint32),

        ("pin_reset_pin",          ctypes.c_uint32)]


class DeviceInfo(object):
    def __init__(self, device_info):
        """ Info about an nRF device. """
        self.device_type         = DeviceVersion(device_info.device_type)
        self.device_family       = DeviceFamily(device_info.device_family)
        self.code_address        = device_info.code_address
        self.code_page_size      = device_info.code_page_size
        self.code_size           = device_info.code_size
        self.uicr_address        = device_info.uicr_address
        self.info_page_size      = device_info.info_page_size
        self.code_ram_present    = device_info.code_ram_present
        self.code_ram_address    = device_info.code_ram_address
        self.data_ram_address    = device_info.data_ram_address
        self.ram_size            = device_info.ram_size
        self.qspi_present        = device_info.qspi_present
        self.xip_address         = device_info.xip_address
        self.xip_size            = device_info.xip_size
        self.pin_reset_pin       = device_info.pin_reset_pin


class ProbeInfoStruct(ctypes.Structure):
    _fields_ = [
        ("serial_number",          ctypes.c_uint32),
        ("clockspeed_khz",         ctypes.c_uint32),
        ("firmware_string",        ctypes.c_char * NRFJPROG_STRING_LENGTH),
        ("num_com_ports",          ctypes.c_uint32),
        ("com_ports",              ComPortInfoStruct * NRFJPROG_COM_PER_JLINK)]


class ProbeInfo(object):
    def __init__(self, probe_info):
        """ Info about a debug probe. """
        self.serial_number       = probe_info.serial_number
        self.clockspeed_khz      = probe_info.clockspeed_khz

        self.firmware_string = decode_string(probe_info.firmware_string)

        self.num_com_ports = probe_info.num_com_ports
        self.com_ports = [ComPortInfo(comport) for comport in probe_info.com_ports[0:self.num_com_ports]]

    def __repr__(self):
        return "ProbeInfo(ProbeInfoStruct({}, {}, {}, {}, [{}]))".format(
            self.serial_number,
            self.clockspeed_khz,
            self.firmware_string,
            self.num_com_ports,
            ", ".join([repr(comport) for comport in self.com_ports])
        )


class LibraryInfoStruct(ctypes.Structure):
    _fields_ = [
        ("version_major",          ctypes.c_uint32),
        ("version_minor",          ctypes.c_uint32),
        ("version_revision",       ctypes.c_char),
        ("file_path",              ctypes.c_char * NRFJPROG_MAX_PATH)]


class LibraryInfo(object):
    def __init__(self, library_info):
        """ Info about an interface library, like JLinkARMDLL. """
        self.version_major          = library_info.version_major
        self.version_minor          = library_info.version_minor

        self.version_revision = decode_string(library_info.version_revision)
        self.file_path = decode_string(library_info.file_path)

    def __repr__(self):
        return "LibraryInfo(LibraryInfoStruct({}, {}, {}, {}))".format(
            self.version_major,
            self.version_minor,
            self.version_revision,
            self.file_path
        )


