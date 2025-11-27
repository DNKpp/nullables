//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_ALGORITHM_AND_THEN_HPP
#define GIMO_ALGORITHM_AND_THEN_HPP

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
    class AndThen
        : private ComposableAlgorithmBase<AndThen<Action>>
    {
        using Super = ComposableAlgorithmBase<AndThen>;
        friend Super;

    public:
        [[nodiscard]]
        explicit constexpr AndThen(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
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
            Self&& self,
            Nullable&& opt,
            auto& first,
            auto&... steps)
        {
            return std::invoke(
                first,
                on_value_impl(std::forward<Self>(self), std::forward<Nullable>(opt)),
                steps...);
        }

        template <typename Self, nullable Nullable>
        [[nodiscard]]
        static constexpr auto on_value_impl(Self&& self, Nullable&& opt)
        {
            return std::invoke(
                std::forward<Self>(self).m_Action,
                value(std::forward<Nullable>(opt)));
        }

        template <nullable Nullable, typename Self>
        [[nodiscard]]
        static constexpr auto on_null_impl(
            [[maybe_unused]] Self&& self,
            auto& first,
            auto&... steps)
        {
            using Result = decltype(on_null_impl<Nullable>(std::forward<Self>(self)));

            return first.template on_null<Result>(steps...);
        }

        template <nullable Nullable, typename Self>
        [[nodiscard]]
        static constexpr auto on_null_impl([[maybe_unused]] Self&& self)
        {
            using Result = std::invoke_result_t<Action, reference_type_t<Nullable>>;

            return detail::construct_empty<Result>();
        }
    };

    template <typename Action>
    struct is_applicable<AndThen<Action>>
    {
        template <typename Self, nullable Nullable>
            requires std::same_as<AndThen<Action>, std::remove_cvref_t<Self>>
        static constexpr bool value = requires {
            requires nullable<
                std::invoke_result_t<
                    const_ref_like_t<Self, Action>,
                    reference_type_t<Nullable>>>;
        };
    };
}

namespace gimo
{
    template <typename Action>
    [[nodiscard]]
    constexpr auto and_then(Action&& action)
    {
        return Pipeline{
            std::make_tuple(detail::AndThen{std::forward<Action>(action)})};
    }
}

#endif
