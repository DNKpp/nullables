//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/algorithm/AndThen.hpp"
#include "gimo_ext/std_optional.hpp"

namespace
{

    /*
    template <typename Alternative>
struct value_or_algorithm
{
    template <typename Nullable>
    [[nodiscard]]
    constexpr auto operator()(Nullable&& opt)
    {
        if (detail::has_value(opt))
        {
            return std::invoke(*this, has_value_tag{}, std::forward<Nullable>());
        }

        return std::invoke(*this, is_empty_tag{}, std::forward<Nullable>());
    }

    template <typename Nullable>
    [[nodiscard]]
    constexpr auto operator()(detail::has_value_tag const, Nullable&& opt)
    {
        return value(std::forward<Nullable>(opt));
    }

    template <typename Nullable>
    [[nodiscard]]
    constexpr auto operator()(is_empty_tag const, [[maybe_unused]] Nullable&& opt)
    {
        return alternative;
    }

    Alternative alternative;
};*/
}

TEST_CASE(
    "AndThenAlgorithm invokes its action only when the input has a value."
    "[algorithm]")
{
    mimicpp::Mock<
        std::optional<int>(float)&,
        std::optional<int>(float) const&,
        std::optional<int>(float)&&,
        std::optional<int>(float) const&&>
        action{};

    using Algorithm = gimo::detail::and_then_t<decltype(action)>;
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm const&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm&&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm const&&>);

    SECTION("When input has a value, the action is invoked.")
    {
        constexpr std::optional opt{1337.f};
        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            SCOPED_EXP action.expect_call(1337.f)
                and finally::returns(std::optional{42});
            Algorithm andThen{std::move(action)};
            decltype(auto) result = andThen(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            SCOPED_EXP std::as_const(action).expect_call(1337.f)
                and finally::returns(std::optional{42});
            Algorithm andThen{std::move(action)};
            decltype(auto) result = std::as_const(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            SCOPED_EXP std::move(action).expect_call(1337.f)
                and finally::returns(std::optional{42});
            Algorithm andThen{std::move(action)};
            decltype(auto) result = std::move(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            SCOPED_EXP std::move(std::as_const(action)).expect_call(1337.f)
                and finally::returns(std::optional{42});
            Algorithm andThen{std::move(action)};
            decltype(auto) result = std::move(std::as_const(andThen))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }
    }

    SECTION("When input is empty, action is not invoked.")
    {
        Algorithm andThen{std::move(action)};
        constexpr std::optional<float> opt{};

        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            decltype(auto) result = andThen(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            decltype(auto) result = std::as_const(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            decltype(auto) result = std::move(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            decltype(auto) result = std::move(std::as_const(andThen))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }
    }
}

TEST_CASE(
    "AndThenAlgorithm receives the nullable forwarded.",
    "[algorithm]")
{
    mimicpp::Mock<
        std::optional<int>(float&) const,
        std::optional<int>(float const&) const,
        std::optional<int>(float&&) const,
        std::optional<int>(float const&&) const>
        action{};
    using Algorithm = gimo::detail::and_then_t<decltype(std::cref(action))>;
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm const&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm&&>);
    STATIC_REQUIRE(gimo::applicable_on_impl<std::optional<float>, Algorithm const&&>);

    Algorithm const andThen{std::cref(action)};

    std::optional opt{1337.f};
    SECTION("When argument is provided as lvalue-ref.")
    {
        SCOPED_EXP action.expect_call(matches::type<float&>)
            and expect::arg<0>(matches::eq(1337.f))
            and finally::returns(std::optional{42});
        decltype(auto) result = andThen(opt);
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(42 == result);
    }

    SECTION("When argument is provided as const lvalue-ref.")
    {
        SCOPED_EXP action.expect_call(matches::type<float const&>)
            and expect::arg<0>(matches::eq(1337.f))
            and finally::returns(std::optional{42});
        decltype(auto) result = andThen(std::as_const(opt));
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(42 == result);
    }

    SECTION("When argument is provided as rvalue-ref.")
    {
        SCOPED_EXP action.expect_call(matches::type<float&&>)
            and expect::arg<0>(matches::eq(1337.f))
            and finally::returns(std::optional{42});
        decltype(auto) result = andThen(std::move(opt));
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(42 == result);
    }

    SECTION("When argument is provided as const rvalue-ref.")
    {
        SCOPED_EXP action.expect_call(matches::type<float const&&>)
            and expect::arg<0>(matches::eq(1337.f))
            and finally::returns(std::optional{42});
        decltype(auto) result = andThen(std::move(std::as_const(opt)));
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(42 == result);
    }
}
