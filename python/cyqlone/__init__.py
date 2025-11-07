"""
Parallel quadratic programming solver for optimal control.
"""

__version__ = "0.0.1.dev0"

from .common import *  # noqa: F403
from .common import __version__ as __c_version__
from .cyqlone import *  # noqa: F403

assert __version__ == __c_version__

# For Sphinx
__all__ = [v for v in dir() if not v.startswith("_") and v != "cyqlone" and v != "common"]
__all__ += ["__c_version__"]
