//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "nullables/nullables.hpp"

TEST_CASE("NULLABLES_VERSION is an integral number.")
{
    constexpr int version{NULLABLES_VERSION};
    STATIC_REQUIRE(0 < version);
}
