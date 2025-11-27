//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_ALGORITHM_COMMON_HPP
#define GIMO_ALGORITHM_COMMON_HPP

#pragma once

#include "gimo/Common.hpp"

#include <type_traits>
#include <utility>

namespace gimo::detail
{
    template <typename T>
    struct is_applicable;

    template <typename Nullable, typename Algorithm>
    concept applicable_on = is_applicable<std::remove_cvref_t<Algorithm>>::template value<Algorithm&&, Nullable&&>;

    template <typename Derived>
    class ComposableAlgorithmBase
    {
    public:
        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &
        {
            return test_and_execute(self(), std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&
        {
            return test_and_execute(self(), std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &&
        {
            return test_and_execute(std::move(*this).self(), std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&&
        {
            return test_and_execute(std::move(*this).self(), std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, auto&... steps) &
        {
            return Derived::on_value_impl(
                self(),
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, auto&... steps) const&
        {
            return Derived::on_value_impl(
                self(),
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, auto&... steps) &&
        {
            return Derived::on_value_impl(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, auto&... steps) const&&
        {
            return Derived::on_value_impl(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto on_null(auto&... steps) &
        {
            return Derived::template on_null_impl<Nullable>(self(), steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto on_null(auto&... steps) const &
        {
            return Derived::template on_null_impl<Nullable>(self(), steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto on_null(auto&... steps) &&
        {
            return Derived::template on_null_impl<Nullable>(std::move(*this).self(), steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto on_null(auto&... steps) const &&
        {
            return Derived::template on_null_impl<Nullable>(std::move(*this).self(), steps...);
        }

    protected:
        [[nodiscard]]
        constexpr ComposableAlgorithmBase() = default;

    private:
        static consteval void check() noexcept
        {
            static_assert(std::is_base_of_v<ComposableAlgorithmBase, Derived>, "Derived must inherit from ComposableAlgorithmBase");
        }

        [[nodiscard]]
        constexpr Derived& self() & noexcept
        {
            check();
            return static_cast<Derived&>(*this);
        }

        [[nodiscard]]
        constexpr Derived const& self() const& noexcept
        {
            check();
            return static_cast<Derived const&>(*this);
        }

        [[nodiscard]]
        constexpr Derived&& self() && noexcept
        {
            check();
            return static_cast<Derived&&>(*this);
        }

        [[nodiscard]]
        constexpr Derived const&& self() const&& noexcept
        {
            check();
            return static_cast<Derived const&&>(*this);
        }

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto test_and_execute(Self&& self, Nullable&& opt, auto&... steps)
        {
            if (detail::has_value(opt))
            {
                return Derived::on_value_impl(
                    std::forward<Self>(self),
                    std::forward<Nullable>(opt),
                    steps...);
            }

            return Derived::template on_null_impl<Nullable>(std::forward<Self>(self), steps...);
        }
    };
}

#endif
