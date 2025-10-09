"""
Parallel quadratic programming solver for optimal control.
"""

__version__ = "0.0.1"

from .cyqlone import *  # noqa: F403
from .cyqlone import __version__ as __c_version__

assert __version__ == __c_version__

# For Sphinx
__all__ = [v for v in dir() if not v.startswith("_") and v != "cyqlone"] + ["__c_version__"]
