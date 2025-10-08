//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <catch2/catch_all.hpp>
#include <mimic++/mimic++.hpp>
#include <mimic++_ext/adapters/Catch2.hpp>

namespace finally = mimicpp::finally;
namespace then = mimicpp::then;
namespace expect = mimicpp::expect;

namespace gimo
{
    template <typename T>
    struct traits;
}

template <typename T>
struct gimo::traits<std::optional<T>>
{
    static constexpr auto null{std::nullopt};
};
