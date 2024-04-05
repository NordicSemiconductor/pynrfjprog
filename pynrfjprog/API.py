import warnings

try:
    from .LowLevel import API
    from .APIError import *
    from .Parameters import *
    from . import JLink
except Exception:
    import LowLevel.API as API
    from APIError import *
    from Parameters import *
    import JLink

warnings.warn("This module is deprecated; use LowLevel.py instead.", PendingDeprecationWarning)

