#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(CMakePackageConfigHelpers)

set(NULLABLES_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/nullables")
set(NULLABLES_CMAKE_INSTALL_DIR "${MIMICPP_LIB_INSTALL_DIR}/cmake")
set(NULLABLES_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/nullables")

write_basic_package_version_file(
    "nullables-version.cmake"
    VERSION         ${PROJECT_VERSION}
    COMPATIBILITY   AnyNewerVersion
    ARCH_INDEPENDENT
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/nullables-config.cmake.in"
    "nullables-config.cmake"
    INSTALL_DESTINATION "${NULLABLES_CMAKE_INSTALL_DIR}"
)

install(
	TARGETS						nullables
	EXPORT						nullables-targets
	PUBLIC_HEADER DESTINATION	"${NULLABLES_INCLUDE_INSTALL_DIR}"
)

install(
    DIRECTORY				"include/"
    TYPE					INCLUDE
    FILES_MATCHING PATTERN	"*.hpp"
)

install(
    EXPORT                  nullables-targets
    FILE                    nullables-targets.cmake
    DESTINATION             "${NULLABLES_CMAKE_INSTALL_DIR}"
    NAMESPACE               nullables::
)

install(
    FILES
        "${PROJECT_BINARY_DIR}/nullables-config.cmake"
        "${PROJECT_BINARY_DIR}/nullables-version.cmake"
    DESTINATION "${NULLABLES_CMAKE_INSTALL_DIR}"
)
