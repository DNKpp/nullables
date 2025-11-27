//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_ALGORITHM_OR_ELSE_HPP
#define GIMO_ALGORITHM_OR_ELSE_HPP

#pragma once

#include "gimo/Common.hpp"
#include "gimo/Pipeline.hpp"
#include "gimo/algorithm/Common.hpp"

#include <concepts>
#include <functional>
#include <type_traits>
#include <tuple>
#include <utility>

namespace gimo::detail
{
    template <typename Action>
    class OrElse
        : private ComposableAlgorithmBase<OrElse<Action>>
    {
        using Super = ComposableAlgorithmBase<OrElse>;
        friend Super;

    public:
        [[nodiscard]]
        explicit constexpr OrElse(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

        using Super::operator();
        using Super::on_null;
        using Super::on_value;

    private:
        Action m_Action;

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto on_value_impl(
            [[maybe_unused]] Self&& self,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return first.on_value(std::forward<Nullable>(opt), steps...);
        }

        template <typename Self, nullable Nullable>
        static constexpr auto on_value_impl([[maybe_unused]] Self&& self, Nullable&& opt)
        {
            return std::forward<Nullable>(opt);
        }

        template <nullable Nullable, typename Self>
        [[nodiscard]]
        static constexpr auto on_null_impl(
            Self&& self,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                on_null_impl<Nullable>(std::forward<Self>(self)),
                steps...);
        }

        template <nullable Nullable, typename Self>
        [[nodiscard]]
        static constexpr auto on_null_impl(Self&& self)
        {
            return std::invoke(std::forward<Self>(self).m_Action);
        }
    };

    template <typename Action>
    struct is_applicable<OrElse<Action>>
    {
        template <typename Self, nullable Nullable>
            requires std::same_as<OrElse<Action>, std::remove_cvref_t<Self>>
        static constexpr bool value = requires {
            requires std::same_as<
                std::remove_cvref_t<Nullable>,
                std::remove_cvref_t<std::invoke_result_t<const_ref_like_t<Self, Action>>>>;
        };
    };
}

namespace gimo
{
    template <typename Action>
    [[nodiscard]]
    constexpr auto or_else(Action&& action)
    {
        return Pipeline{
            std::make_tuple(detail::OrElse{std::forward<Action>(action)})};
    }
}

#endif
