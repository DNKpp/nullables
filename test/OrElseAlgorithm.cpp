//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo_ext/std_optional.hpp"
#include "gimo/Algorithm.hpp"

TEST_CASE(
    "OrElseAlgorithm invokes its action only when the input has no value."
    "[algorithm]")
{
    mimicpp::Mock<
        std::optional<int>()&,
        std::optional<int>() const&,
        std::optional<int>()&&,
        std::optional<int>() const&&>
        action{};

    using Algorithm = gimo::OrElseAlgorithm<decltype(action)>;
    static_assert(gimo::nullable<std::optional<int>>);
    static_assert(std::destructible<gimo::traits<std::optional<int>>>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm const&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm&&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm const&&>);

    SECTION("When input has no value, the action is invoked.")
    {
        constexpr std::optional<int> opt{};
        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            SCOPED_EXP action.expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = orElse(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            SCOPED_EXP std::as_const(action).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::as_const(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            SCOPED_EXP std::move(action).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::move(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            SCOPED_EXP std::move(std::as_const(action)).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::move(std::as_const(orElse))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }
    }

    SECTION("When input has a value, the action is not invoked.")
    {
        gimo::OrElseAlgorithm orElse{std::move(action)};
        constexpr std::optional opt{1337};

        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            decltype(auto) result = orElse(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            decltype(auto) result = std::as_const(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            decltype(auto) result = std::move(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            decltype(auto) result = std::move(std::as_const(orElse))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }
    }
}
