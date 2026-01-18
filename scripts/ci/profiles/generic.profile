{% set arch_dir = os.path.join(profile_dir, "..", "conan-profiles", "profiles", "arch") %}
include({{ os.path.join(arch_dir, "linux", "generic.profile") }})
