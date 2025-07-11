# - Config file for the Armadillo package
# It defines the following variables
#  ARMADILLO_INCLUDE_DIRS - include directories for Armadillo
#  ARMADILLO_LIBRARY_DIRS - library directories for Armadillo (normally not used!)
#  ARMADILLO_LIBRARIES    - libraries to link against

# Tell the user project where to find our headers and libraries
set(ARMADILLO_INCLUDE_DIRS "/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/armadillo/include")
set(ARMADILLO_LIBRARY_DIRS "/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/armadillo/lib64")

# Our library dependencies (contains definitions for IMPORTED targets)
include("/disk1/2025DA1/group4/projects/p2/Gordian_Placement/Assignment2/third_party/armadillo/share/Armadillo/CMake/ArmadilloLibraryDepends.cmake")

# These are IMPORTED targets created by ArmadilloLibraryDepends.cmake
set(ARMADILLO_LIBRARIES armadillo)

