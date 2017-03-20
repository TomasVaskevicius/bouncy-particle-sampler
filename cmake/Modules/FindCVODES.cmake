# Try to find CVODES include directories and static libraries required
# to run the Stan-Math library.

find_package(PkgConfig)

find_path(CVODES_HEADERS cvodes/cvodes.h)
find_library(SUNDIALS_CVODES_LIB NAMES sundials_cvodes)
find_library(SUNDIALS_NVECSERIAL_LIB NAMES sundials_nvecserial)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CVODES
                                  DEFAULT_MSG
                                  CVODES_HEADERS
                                  SUNDIALS_CVODES_LIB
                                  SUNDIALS_NVECSERIAL_LIB)

mark_as_advanced(CVODES_INCLUDE_DIR CVODES_LIB NVECSERIAL_LIB)

set(CVODES_LIBRARIES ${SUNDIALS_CVODES_LIB} ${SUNDIALS_NVECSERIAL_LIB})
set(CVODES_INCLUDE_DIR ${CVODES_HEADERS} )
