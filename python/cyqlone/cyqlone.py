import contextlib, importlib, os, sys

_variant = os.getenv("CYQLONE_VARIANT")
if _variant is None:
    _variant = "_generic"
    with contextlib.suppress(ModuleNotFoundError):
        from ._dispatch import get_dispatch_name

        _variant = "_" + get_dispatch_name()

_target_name = "._cyqlone" + _variant
_target = importlib.import_module(_target_name, package=__package__)
setattr(_target, "variant", _variant)
sys.modules[__name__] = _target

for _submod in ("simd8", "simd4", "scalar"):
    _target = importlib.import_module(_target_name + "." + _submod, package=__package__)
    sys.modules[__name__ + "." + _submod] = _target
