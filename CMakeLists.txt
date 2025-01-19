#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.15)
set(MESSAGE_PREFIX "nullables:")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ReadNullablesVersion)
read_nullables_version("include/nullables/Version.hpp" NULLABLES_VERSION)

project(
	nullables
	LANGUAGES		CXX
    VERSION ${NULLABLES_VERSION}
	DESCRIPTION		"TBD"
	HOMEPAGE_URL	"https://github.com/DNKpp/nullables"
)

message(STATUS "${MESSAGE_PREFIX} version: v${PROJECT_VERSION} from: ${nullables_SOURCE_DIR}")

include(GNUInstallDirs)

add_library(nullables INTERFACE)
add_library(nullables::nullables ALIAS nullables)
target_include_directories(nullables
	INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set(CMAKE_CXX_STANDARD 20)
if (NULLABLES_FORCED_CXX_STANDARD)
	message(DEBUG "${MESSAGE_PREFIX} forced c++-standard c++${NULLABLES_FORCED_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD ${NULLABLES_FORCED_CXX_STANDARD})
endif()

target_compile_features(nullables
	INTERFACE
	cxx_std_${CMAKE_CXX_STANDARD}
)

if (CMAKE_SOURCE_DIR STREQUAL nullables_SOURCE_DIR)
	set(IS_TOP_LEVEL_PROJECT ON)
else()
	set(IS_TOP_LEVEL_PROJECT OFF)
endif()

OPTION(NULLABLES_BUILD_TESTS "Determines, whether the tests shall be built." ${IS_TOP_LEVEL_PROJECT})
if (NULLABLES_BUILD_TESTS)
	include(CTest)
	add_subdirectory("test")
endif()

OPTION(NULLABLES_CONFIGURE_DOXYGEN "Determines, whether the doxyfile shall be configured." OFF)
if (NULLABLES_CONFIGURE_DOXYGEN)

	OPTION(NULLABLES_ENABLE_GENERATE_DOCS "Enable the doxygen documentation target." OFF)

	add_subdirectory("docs")

endif()

if(NOT CMAKE_SKIP_INSTALL_RULES)

  include(NullablesInstallRules)

endif()
