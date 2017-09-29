""" Examples for pynrfjprog. 
    Each individual example can be used by importing examples as:
    from pynrfjprog import examples;  then calling:
    examples.python_help.run() """

try:
    from . import program_hex
    from . import memory_read_write
    from . import python_help

except Exception:
    import program_hex
    import memory_read_write
    import python_help

