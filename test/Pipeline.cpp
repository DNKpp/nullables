//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "gimo/Algorithm.hpp"

#include <memory>

namespace
{
    struct NullableNull
    {
    };

    template <typename T>
    class NullableMock
    {
    public:
        NullableMock() = default;
        NullableMock(NullableMock&&) = default;
        NullableMock& operator=(NullableMock&&) = default;

        constexpr NullableMock([[maybe_unused]] NullableNull const null) noexcept
        {
        }

        mimicpp::Mock<
            T&()&,
            T const&() const&,
            T && ()&&,
            T const && () const&&>
            value{};

        [[nodiscard]]
        constexpr T& operator*() &
        {
            return value();
        }

        [[nodiscard]]
        constexpr T const& operator*() const&
        {
            return value();
        }

        [[nodiscard]]
        constexpr T&& operator*() &&
        {
            return std::move(value)();
        }

        [[nodiscard]]
        constexpr T const&& operator*() const&&
        {
            return std::move(std::as_const(value))();
        }

        mimicpp::Mock<bool() const> is_null{};

        [[nodiscard]]
        constexpr bool operator==([[maybe_unused]] NullableNull const null) const
        {
            return is_null();
        }

        mimicpp::Mock<void()> set_null{};
        NullableMock& operator=([[maybe_unused]] NullableNull const null)
        {
            set_null();
            return *this;
        }
    };
}

template <typename T>
struct gimo::traits<NullableMock<T>>
{
    static constexpr NullableNull null{};
};

void foo(std::common_reference_t<std::optional<int> const&, std::nullopt_t const&>) = delete;
//void foo(std::common_reference_t<NullableMock<int> const&, NullableNull const&>) = delete;

static_assert(std::same_as<std::optional<int> const, std::common_reference_t<std::optional<int> const&, std::nullopt_t const&>>);
static_assert(std::same_as<std::optional<int>, decltype(false? std::optional<int>{} : std::declval<std::nullopt_t>())>);
static_assert(std::convertible_to<std::nullopt_t const&, std::optional<int> const&>);

static_assert(std::same_as<std::optional<int>, decltype(false? std::declval<std::nullopt_t>() : std::optional<int>{})>);

//static_assert(std::same_as<NullableMock<int> const, std::common_reference_t<NullableMock<int> const&, NullableNull const&>>);

static_assert(std::convertible_to<NullableNull const&, NullableMock<int> const&>);
static_assert(gimo::nullable<NullableMock<int>>);

TEST_CASE(
    "Pipelines can be appended.",
    "[pipeline]")
{
    mimicpp::Mock<NullableMock<int>(float)> action1{};
    mimicpp::Mock<NullableMock<bool>(int)> action2{};

    auto pipeline = gimo::and_then(std::ref(action1))
        | gimo::and_then(std::ref(action2));

    NullableMock<float> nullable{NullableNull{}};
    auto result = pipeline.apply(nullable);
}
