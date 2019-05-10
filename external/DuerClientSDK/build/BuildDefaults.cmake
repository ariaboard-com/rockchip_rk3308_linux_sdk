# Append custom CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Setup default build options, like compiler flags and build type.
include(BuildOptions)

# Setup package requirement variables.
include(PackageConfigs)

# Setup Test Options variables.
include(TestOptions)
