//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_ALGORITHM_HPP
#define GIMO_ALGORITHM_HPP

#pragma once

#include "gimo/Common.hpp"

#include <concepts>
#include <functional>
#include <utility>

namespace gimo::detail
{
    template <typename T>
    struct is_applicable;

    template <typename Nullable, typename Algorithm>
    concept applicable_on = is_applicable<std::remove_cvref_t<Algorithm>>::template value<Algorithm, Nullable>;

    template <typename Derived>
    class ComposableAlgorithmBase
    {
    public:
        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(self(), has_value_tag{}, std::forward<Nullable>(opt), steps...);
            }

            return Derived::execute(self(), is_empty_tag{}, std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(self(), has_value_tag{}, std::forward<Nullable>(opt), steps...);
            }

            return Derived::execute(self(), is_empty_tag{}, std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &&
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(std::move(*this).self(), has_value_tag{}, std::forward<Nullable>(opt), steps...);
            }

            return Derived::execute(std::move(*this).self(), is_empty_tag{}, std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&&
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(std::move(*this).self(), has_value_tag{}, std::forward<Nullable>(opt), steps...);
            }

            return Derived::execute(std::move(*this).self(), is_empty_tag{}, std::forward<Nullable>(opt), steps...);
        }

        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) &
        {
            return Derived::execute(
                self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) const&
        {
            return Derived::execute(
                self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) &&
        {
            return Derived::execute(
                std::move(*this).self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) const&&
        {
            return Derived::execute(
                std::move(*this).self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) &
        {
            return Derived::execute(
                self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) const&
        {
            return Derived::execute(
                self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) &&
        {
            return Derived::execute(
                std::move(*this).self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <applicable_on<Derived const&&> Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) const&&
        {
            return Derived::execute(
                std::move(*this).self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
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
    };
}

namespace gimo
{
    template <typename Action>
    class AndThenAlgorithm
        : private detail::ComposableAlgorithmBase<AndThenAlgorithm<Action>>
    {
        friend class detail::ComposableAlgorithmBase<AndThenAlgorithm>;

    public:
        [[nodiscard]]
        explicit constexpr AndThenAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

        using detail::ComposableAlgorithmBase<AndThenAlgorithm>::operator();

    private:
        Action m_Action;

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            detail::has_value_tag const tag,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                execute(std::forward<Self>(self), tag, std::forward<Nullable>(opt)),
                steps...);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            [[maybe_unused]] detail::has_value_tag const,
            Nullable&& opt)
        {
            return std::invoke(
                std::forward<Self>(self).m_Action,
                value(std::forward<Nullable>(opt)));
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            detail::is_empty_tag const tag,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                tag,
                execute(std::forward<Self>(self), tag, std::forward<Nullable>(opt)),
                steps...);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            [[maybe_unused]] Self&& self,
            [[maybe_unused]] detail::is_empty_tag const tag,
            [[maybe_unused]] Nullable&& opt)
        {
            using Result = std::invoke_result_t<Action, Nullable&&>;

            return detail::construct_empty<Result>();
        }
    };

    template <typename Action>
    struct detail::is_applicable<AndThenAlgorithm<Action>>
    {
        template <typename Self, nullable Nullable>
            requires std::same_as<AndThenAlgorithm<Action>, std::remove_cvref_t<Self>>
        static constexpr bool value = requires {
            requires nullable<
                std::invoke_result_t<
                    cv_ref_like_t<Self, Action>,
                    Nullable>>;
        };
    };

    template <typename Action>
    class OrElseAlgorithm
        : private detail::ComposableAlgorithmBase<OrElseAlgorithm<Action>>
    {
        friend class detail::ComposableAlgorithmBase<OrElseAlgorithm>;

    public:
        [[nodiscard]]
        explicit constexpr OrElseAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

        using detail::ComposableAlgorithmBase<OrElseAlgorithm>::operator();

    private:
        Action m_Action;

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            [[maybe_unused]] Self&& self,
            detail::has_value_tag const tag,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                tag,
                std::forward<Nullable>(opt),
                steps...);
        }

        template <typename Self, nullable Nullable>
        static constexpr auto execute(
            [[maybe_unused]] Self&& self,
            [[maybe_unused]] detail::has_value_tag const tag,
            Nullable&& opt)
        {
            return std::forward<Nullable>(opt);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            detail::is_empty_tag const tag,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                execute(std::forward<Self>(self), tag, std::forward<Nullable>(opt)),
                steps...);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            [[maybe_unused]] detail::is_empty_tag const tag,
            [[maybe_unused]] Nullable&& opt)
        {
            return std::invoke(std::forward<Self>(self).m_Action);
        }
    };

    template <typename Action>
    struct detail::is_applicable<OrElseAlgorithm<Action>>
    {
        template <typename Self, nullable Nullable>
            requires std::same_as<OrElseAlgorithm<Action>, std::remove_cvref_t<Self>>
        static constexpr bool value = requires {
            requires std::same_as<
                std::remove_cvref_t<Nullable>,
                std::remove_cvref_t<std::invoke_result_t<cv_ref_like_t<Self, Action>>>>;
        };
    };

    template <typename Action>
    struct TransformAlgorithm
        : private detail::ComposableAlgorithmBase<TransformAlgorithm<Action>>
    {
        friend class detail::ComposableAlgorithmBase<TransformAlgorithm>;

    public:
        [[nodiscard]]
        explicit constexpr TransformAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

        using detail::ComposableAlgorithmBase<TransformAlgorithm>::operator();

    private:
        Action m_Action;

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            Self&& self,
            detail::has_value_tag const tag,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                tag,
                detail::rebind_value<Nullable>(
                    std::invoke(
                        std::forward<Self>(self).action,
                        value(std::forward<Nullable>(opt)))),
                steps...);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto execute(
            [[maybe_unused]] Self&& self,
            detail::is_empty_tag const tag,
            [[maybe_unused]] Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            using Result = std::invoke_result_t<Action, decltype(value(std::forward<Nullable>()))>;

            return std::invoke(
                first,
                tag,
                detail::construct_empty<rebind_value_t<Nullable, Result>>(),
                steps...);
        }
    };

    template <typename Action>
    struct detail::is_applicable<TransformAlgorithm<Action>>
    {
        template <typename Self, nullable Nullable>
            requires std::same_as<TransformAlgorithm<Action>, std::remove_cvref_t<Self>>
        static constexpr bool value = requires {
            requires rebindable_to<
                std::invoke_result_t<
                    cv_ref_like_t<Self, Action>,
                    reference_type_t<Nullable>>,
                std::remove_cvref_t<Nullable>>;
        };
    };
}

#endif
