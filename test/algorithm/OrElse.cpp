//           Copyright Dominic (DNKpp) Koepke 2025.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/algorithm/OrElse.hpp"
#include "gimo_ext/std_optional.hpp"

#include "TestCommons.hpp"

using namespace gimo;

TEMPLATE_TEST_CASE(
    "or_else algorithm invokes its action only when the input has no value.",
    "[algorithm]",
    testing::as_lvalue_ref,
    testing::as_const_lvalue_ref,
    testing::as_rvalue_ref,
    testing::as_const_rvalue_ref)
{
    using Cast = TestType;

    mimicpp::Mock<
        std::optional<int>()&,
        std::optional<int>() const&,
        std::optional<int>()&&,
        std::optional<int>() const&&>
        action{};

    using Algorithm = detail::or_else_t<decltype(action)>;
    STATIC_REQUIRE(gimo::applicable_on<std::optional<int>, typename Cast::template type<Algorithm>>);

    SECTION("When input has no value, the action is invoked.")
    {
        constexpr std::optional<int> opt{};

        SCOPED_EXP Cast::cast(action).expect_call()
            and finally::returns(std::optional{42});

        Algorithm orElse{std::move(action)};
        decltype(auto) result = Cast::cast(orElse)(opt);
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(42 == result);
    }

    SECTION("When input has a value, the action is not invoked.")
    {
        Algorithm orElse{std::move(action)};
        constexpr std::optional opt{1337};

        decltype(auto) result = Cast::cast(orElse)(opt);
        STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
        CHECK(result);
    }
}

TEMPLATE_TEST_CASE(
    "or_else algorithm accepts nullables with any cv-ref qualification.",
    "[algorithm]",
    testing::as_lvalue_ref,
    testing::as_const_lvalue_ref,
    testing::as_rvalue_ref,
    testing::as_const_rvalue_ref)
{
    using Cast = TestType;

    mimicpp::Mock<std::optional<int>() const> const action{};

    using Algorithm = detail::or_else_t<decltype(std::cref(action))>;
    STATIC_REQUIRE(gimo::applicable_on<std::optional<int>, typename Cast::template type<Algorithm>>);

    Algorithm const orElse{std::cref(action)};
    std::optional<int> opt{};

    SCOPED_EXP action.expect_call()
        and finally::returns(std::optional{42});
    decltype(auto) result = orElse(Cast::cast(opt));
    STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
    CHECK(42 == result);
}

TEMPLATE_TEST_CASE(
    "gimo::or_else creates an appropriate pipeline.",
    "[algorithm]",
    testing::as_lvalue_ref,
    testing::as_const_lvalue_ref,
    testing::as_rvalue_ref,
    testing::as_const_rvalue_ref)
{
    using Cast = TestType;

    mimicpp::Mock<std::optional<int>() const> const inner{};
    auto action = [&] { return inner(); };
    using DummyAction = decltype(action);

    decltype(auto) pipeline = or_else(Cast::cast(action));
    STATIC_CHECK(std::same_as<Pipeline<detail::or_else_t<DummyAction>>, decltype(pipeline)>);
    STATIC_CHECK(gimo::applicable_on<std::optional<int>, detail::or_else_t<DummyAction>>);

    SCOPED_EXP inner.expect_call()
        and finally::returns(1337);
    CHECK(1337 == pipeline.apply(std::optional<int>{}));
}
