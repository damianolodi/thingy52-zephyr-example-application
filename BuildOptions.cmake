###################
# Project Options #
###################

##############################################
# Default Settings for CMake Cache Variables #
##############################################

# Set a default build type if none was specified
set(default_build_type "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build."
      FORCE
  )
  # Set the possible values of build type for cmake-gui/ccmake
  set_property(CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
  )
endif()

# Export compile_commands.json file.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
