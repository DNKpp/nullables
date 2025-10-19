//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/Algorithm.hpp"
#include "gimo_ext/std_optional.hpp"

TEST_CASE(
    "TransformAlgorithm invokes its action with the contained value, when there is any."
    "[algorithm]")
{
    mimicpp::Mock<
        int(float)&,
        int(float) const&,
        int(float)&&,
        int(float) const&&>
        action{};

    using Algorithm = gimo::TransformAlgorithm<decltype(action)>;
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm const&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm&&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm const&&>);

    SECTION("When input has a value, the action is invoked.")
    {
        constexpr std::optional opt{1337.f};
        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            SCOPED_EXP action.expect_call(1337.f)
                and finally::returns(42);
            gimo::TransformAlgorithm transform{std::move(action)};
            decltype(auto) result = transform(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            SCOPED_EXP std::as_const(action).expect_call(1337.f)
                and finally::returns(42);
            gimo::TransformAlgorithm transform{std::move(action)};
            decltype(auto) result = std::as_const(transform)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            SCOPED_EXP std::move(action).expect_call(1337.f)
                and finally::returns(42);
            gimo::TransformAlgorithm transform{std::move(action)};
            decltype(auto) result = std::move(transform)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            SCOPED_EXP std::move(std::as_const(action)).expect_call(1337.f)
                and finally::returns(42);
            gimo::TransformAlgorithm transform{std::move(action)};
            decltype(auto) result = std::move(std::as_const(transform))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }
    }

    SECTION("When input is empty, action is not invoked.")
    {
        gimo::TransformAlgorithm transform{std::move(action)};
        constexpr std::optional<float> opt{};

        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            decltype(auto) result = transform(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            decltype(auto) result = std::as_const(transform)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            decltype(auto) result = std::move(transform)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            decltype(auto) result = std::move(std::as_const(transform))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }
    }
}
