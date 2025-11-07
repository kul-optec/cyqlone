import importlib, sys

_target_name = "._common"
_target = importlib.import_module(_target_name, package=__package__)
sys.modules[__name__] = _target
