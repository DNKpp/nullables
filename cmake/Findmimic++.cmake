#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

include(get_cpm)

set(MIMICPP_CONFIG_EXPERIMENTAL_STACKTRACE "cpptrace" CACHE BOOL "" FORCE)
set(MIMICPP_CONFIG_EXPERIMENTAL_PRETTY_TYPES ON CACHE BOOL "" FORCE)
CPMAddPackage("gh:DNKpp/mimicpp@9")
