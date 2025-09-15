import os
import typing


def _is_truthy(s: typing.Optional[str]):
    if s is None:
        return False
    return s.lower() not in ("", "false", "no", "off", "0")


if not typing.TYPE_CHECKING and _is_truthy(os.getenv("CYQLONE_PYTHON_DEBUG")):
    from . import _cyqlone_d  # noqa: F401
    from ._cyqlone_d import *  # noqa: F401, F403
    from ._cyqlone_d import __version__ as __c_version__  # noqa: F401
else:
    from . import _cyqlone  # noqa: F401
    from ._cyqlone import *  # noqa: F401, F403
    from ._cyqlone import __version__ as __c_version__  # noqa: F401

del _is_truthy, typing, os
