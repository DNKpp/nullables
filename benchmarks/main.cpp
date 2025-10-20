//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/Algorithm.hpp"
#include "gimo/Pipeline.hpp"
#include "gimo_ext/std_optional.hpp"


TEST_CASE("benchmark")
{
    std::optional<int> opt =
        Catch::rngSeed() % 2 == 0 ? std::nullopt : std::optional<int>{1337};

    BENCHMARK("Member")
    {
        return opt.and_then([](auto const& x) { return std::optional{x * x}; })
            .and_then([](int const x) { return std::optional{static_cast<float>(x)}; })
            .and_then([](float const x) { return std::optional{std::fmod(x, 0.5f)}; });
    };

    BENCHMARK("gimo")
    {
        static constexpr auto pipeline =
            gimo::and_then([](auto const& x) { return std::optional{x * x}; })
            | gimo::and_then([](int const x) { return std::optional{static_cast<float>(x)}; })
            | gimo::and_then([](float const x) { return std::optional{std::fmod(x, 0.5f)}; });

        return pipeline.apply(opt);
    };
}
