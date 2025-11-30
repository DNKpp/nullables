#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(CTest)

set(TARGET_NAME gimo-tests)

add_executable(${TARGET_NAME}
    "Common.cpp"
    "Pipeline.cpp"
)
add_subdirectory(algorithm)
add_subdirectory(config)

target_include_directories(${TARGET_NAME} PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
)

include(EnableSanitizers)
enable_sanitizers(${TARGET_NAME})

include(EnableWarnings)
find_package(Catch2 REQUIRED)
find_package(gimo-mimic++ REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    gimo::gimo
    gimo::internal::enable-warnings

    Catch2::Catch2WithMain
    mimicpp::mimicpp
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    CATCH_CONFIG_FALLBACK_STRINGIFIER=mimicpp::print
)
target_precompile_headers(${TARGET_NAME} PRIVATE
    "pch.hpp"
)

catch_discover_tests(${TARGET_NAME})
