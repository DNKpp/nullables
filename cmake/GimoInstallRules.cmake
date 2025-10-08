#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(CMakePackageConfigHelpers)

set(GIMO_LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/gimo")
set(GIMO_CMAKE_INSTALL_DIR "${MIMICPP_LIB_INSTALL_DIR}/cmake")
set(GIMO_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/gimo")

write_basic_package_version_file(
    "gimo-version.cmake"
    VERSION         ${PROJECT_VERSION}
    COMPATIBILITY   AnyNewerVersion
    ARCH_INDEPENDENT
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/gimo-config.cmake.in"
    "gimo-config.cmake"
    INSTALL_DESTINATION "${GIMO_CMAKE_INSTALL_DIR}"
)

install(
	TARGETS						gimo
	EXPORT						gimo-targets
	PUBLIC_HEADER DESTINATION	"${GIMO_INCLUDE_INSTALL_DIR}"
)

install(
    DIRECTORY				"include/"
    TYPE					INCLUDE
    FILES_MATCHING PATTERN	"*.hpp"
)

install(
    EXPORT                  gimo-targets
    FILE                    gimo-targets.cmake
    DESTINATION             "${GIMO_CMAKE_INSTALL_DIR}"
    NAMESPACE               gimo::
)

install(
    FILES
        "${PROJECT_BINARY_DIR}/gimo-config.cmake"
        "${PROJECT_BINARY_DIR}/gimo-version.cmake"
    DESTINATION "${GIMO_CMAKE_INSTALL_DIR}"
)
