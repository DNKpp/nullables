//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/Algorithm.hpp"
#include "gimo/Pipeline.hpp"
#include "gimo_ext/std_optional.hpp"

#include <random>
#include <ranges>

TEST_CASE("benchmark")
{
    std::vector<std::optional<int>> optionals(100'000, std::nullopt);
    std::ranges::transform(
        std::views::iota(0, static_cast<int>(optionals.size() / 2u)),
        optionals.begin(),
        [](int const i) { return std::optional{i}; });
    std::ranges::shuffle(optionals, std::mt19937{Catch::getSeed()});

    BENCHMARK("Member")
    {
        return std::ranges::max(
            optionals,
            {},
            [](auto const& opt) {
                return opt.and_then([](auto const& x) { return std::optional{std::to_string(x)}; })
                    .and_then([](std::string const& str) { return std::optional{str.size()}; })
                    .and_then([](std::size_t const length) { return std::optional{static_cast<float>(length)}; })
                    .value_or(0u);
            });
    };

    BENCHMARK("gimo")
    {
        static constexpr auto pipeline =
            gimo::and_then([](auto const& x) { return std::optional{std::to_string(x)}; })
            | gimo::and_then([](std::string const& str) { return std::optional{str.size()}; })
            | gimo::and_then([](std::size_t const length) { return std::optional{static_cast<float>(length)}; });

        return std::ranges::max(
            optionals,
            {},
            [](auto const& opt) { return pipeline.apply(opt).value_or(0u); });
    };
}
