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
    struct algorithm_traits;

    template <typename Traits, typename Action, typename Nullable, typename... Steps>
    [[nodiscard]]
    static constexpr auto test_and_execute(Action&& action, Nullable&& opt, Steps&&... steps)
    {
        if (detail::has_value(opt))
        {
            return Traits::on_value(
                std::forward<Action>(action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        return Traits::template on_null<Nullable>(
            std::forward<Action>(action),
            std::forward<Steps>(steps)...);
    }

    template <typename Nullable, typename Traits, typename Action>
    concept applicable_on_impl_internal = Traits::template is_applicable_on<Nullable, Action>;

    template <typename Nullable, typename Algorithm>
    concept applicable_on_impl = requires {
        requires applicable_on_impl_internal<
            Nullable,
            typename std::remove_cvref_t<Algorithm>::traits_type,
            const_ref_like_t<Algorithm, typename std::remove_cvref_t<Algorithm>::action_type>>;
    };

    template <typename Traits, typename Action>
    class BasicAlgorithm
    {
    public:
        using traits_type = Traits;
        using action_type = Action;

        template <typename... Args>
            requires std::constructible_from<Action, Args&&...>
        [[nodiscard]] explicit constexpr BasicAlgorithm(Args&&... args) noexcept(std::is_nothrow_constructible_v<Action, Args&&...>)
            : m_Action{std::forward<Args>(args)...}
        {
        }

        template <applicable_on_impl<BasicAlgorithm&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) &
        {
            return detail::test_and_execute<Traits>(
                m_Action,
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) const&
        {
            return detail::test_and_execute<Traits>(
                m_Action,
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) &&
        {
            return detail::test_and_execute<Traits>(
                std::move(m_Action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) const&&
        {
            return detail::test_and_execute<Traits>(
                std::move(m_Action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) &
        {
            return Traits::on_value(
                m_Action,
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) const&
        {
            return Traits::on_value(
                m_Action,
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) &&
        {
            return Traits::on_value(
                std::move(m_Action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) const&&
        {
            return Traits::on_value(
                std::move(m_Action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) &
        {
            return Traits::template on_null<Nullable>(
                m_Action,
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) const&
        {
            return Traits::template on_null<Nullable>(
                m_Action,
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) &&
        {
            return Traits::template on_null<Nullable>(
                std::move(m_Action),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on_impl<BasicAlgorithm const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) const&&
        {
            return Traits::template on_null<Nullable>(
                std::move(m_Action),
                std::forward<Steps>(steps)...);
        }

    private:
        [[maybe_unused]] Action m_Action;
    };

    template <typename T>
    struct is_applicable;

    template <typename Nullable, typename Algorithm>
    concept applicable_on = is_applicable<std::remove_cvref_t<Algorithm>>::template value<Algorithm&&, Nullable&&>;

    template <typename Derived>
    class ComposableAlgorithmBase
    {
    public:
        template <applicable_on<Derived&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) &
        {
            return test_and_execute(
                self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) const&
        {
            return test_and_execute(
                self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) &&
        {
            return test_and_execute(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, Steps&&... steps) const&&
        {
            return test_and_execute(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) &
        {
            return Derived::on_value_impl(
                self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) const&
        {
            return Derived::on_value_impl(
                self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) &&
        {
            return Derived::on_value_impl(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_value(Nullable&& opt, Steps&&... steps) const&&
        {
            return Derived::on_value_impl(
                std::move(*this).self(),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) &
        {
            return Derived::template on_null_impl<Nullable>(
                self(),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) const&
        {
            return Derived::template on_null_impl<Nullable>(
                self(),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) &&
        {
            return Derived::template on_null_impl<Nullable>(
                std::move(*this).self(),
                std::forward<Steps>(steps)...);
        }

        template <applicable_on<Derived const&&> Nullable, typename... Steps>
        [[nodiscard]]
        constexpr auto on_null(Steps&&... steps) const&&
        {
            return Derived::template on_null_impl<Nullable>(
                std::move(*this).self(),
                std::forward<Steps>(steps)...);
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

        template <typename Self, typename Nullable, typename... Steps>
        [[nodiscard]]
        static constexpr auto test_and_execute(Self&& self, Nullable&& opt, Steps&&... steps)
        {
            if (detail::has_value(opt))
            {
                return Derived::on_value_impl(
                    std::forward<Self>(self),
                    std::forward<Nullable>(opt),
                    std::forward<Steps>(steps)...);
            }

            return Derived::template on_null_impl<Nullable>(
                std::forward<Self>(self),
                std::forward<Steps>(steps)...);
        }
    };
}

#endif
