#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(CTest)

set(TARGET_NAME gimo-benchmarks)

add_executable(${TARGET_NAME}
    "main.cpp"
)

target_compile_features(${TARGET_NAME} PRIVATE
    cxx_std_23
)

include(EnableSanitizers)
enable_sanitizers(${TARGET_NAME})

include(EnableWarnings)
find_package(Catch2 REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE
    gimo::gimo
    gimo::internal::enable-warnings

    Catch2::Catch2WithMain
)

target_precompile_headers(${TARGET_NAME} PRIVATE
    <catch2/catch_all.hpp>
)

catch_discover_tests(${TARGET_NAME})
