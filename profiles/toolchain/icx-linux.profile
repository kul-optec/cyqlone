{% set home = os.getenv("HOME") %}
{% set oneapi_candidates = [
    os.getenv("ONEAPI_ROOT"),
    os.path.join(home, "intel/oneapi") if home else None,
    "/opt/intel/oneapi",
] %}
{% set paths = namespace(oneapi_root=None) %}
{% for candidate in oneapi_candidates %}
    {% if candidate and not paths.oneapi_root %}
        {% if os.path.isdir(os.path.join(candidate, "compiler")) %}
            {% set paths.oneapi_root = candidate %}
        {% endif %}
    {% endif %}
{% endfor %}

[settings]
compiler=intel-cc
compiler.version=2025.3
compiler.mode=icx
compiler.cppstd=23
compiler.libcxx=libstdc++11
compiler.libcxx.gcc_version=15.2

[buildenv]
{% if paths.oneapi_root %}
ONEAPI_ROOT={{ paths.oneapi_root }}
PATH+=(path){{ paths.oneapi_root }}/compiler/latest/bin
{% endif %}

[conf]
tools.build:compiler_executables*={"c": "icx", "cpp": "icpx"}
tools.build:cflags+=["-fp-model=precise", "-ffp-contract=on", "-Wno-pass-failed", "-Wno-overriding-option"]
tools.build:cxxflags+=["-fp-model=precise", "-ffp-contract=on", "-Wno-pass-failed", "-Wno-overriding-option"]
{% if paths.oneapi_root %}
tools.intel:installation_path={{ paths.oneapi_root }}
{% endif %}
