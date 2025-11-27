//          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef GIMO_PIPELINE_HPP
#define GIMO_PIPELINE_HPP

#pragma once

#include "gimo/Common.hpp"

#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace gimo
{
    template <typename... Steps>
    class Pipeline
    {
        template <typename... Others>
        friend class Pipeline;

    public:
        [[nodiscard]]
        explicit constexpr Pipeline(std::tuple<Steps...> steps)
            : m_Steps{std::move(steps)}
        {
        }

        template <nullable Nullable>
        constexpr auto apply(Nullable&& opt) &
        {
            return apply(*this, std::forward<Nullable>(opt));
        }

        template <nullable Nullable>
        constexpr auto apply(Nullable&& opt) const &
        {
            return apply(*this, std::forward<Nullable>(opt));
        }

        template <nullable Nullable>
        constexpr auto apply(Nullable&& opt) &&
        {
            return apply(std::move(*this), std::forward<Nullable>(opt));
        }

        template <nullable Nullable>
        constexpr auto apply(Nullable&& opt) const &&
        {
            return apply(std::move(*this), std::forward<Nullable>(opt));
        }

        template <typename... SuffixSteps>
        constexpr auto append(Pipeline<SuffixSteps...> suffix) const&
        {
            return append(*this, std::move(suffix.m_Steps));
        }

        template <typename... SuffixSteps>
        constexpr auto append(Pipeline<SuffixSteps...> suffix) &&
        {
            return append(std::move(*this), std::move(suffix.m_Steps));
        }

        template <typename... SuffixSteps>
        [[nodiscard]]
        friend constexpr auto operator|(Pipeline const& prefix, Pipeline<SuffixSteps...> suffix)
        {
            return append(prefix, std::move(suffix).m_Steps);
        }

        template <typename... SuffixSteps>
        [[nodiscard]]
        friend constexpr auto operator|(Pipeline&& prefix, Pipeline<SuffixSteps...> suffix)
        {
            return append(std::move(prefix), std::move(suffix).m_Steps);
        }

    private:
        std::tuple<Steps...> m_Steps{};

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto apply(Self&& self, Nullable&& opt)
        {
            return std::apply(
                [&]<typename First, typename... Others>(First&& first, Others&&... steps) {
                    return std::invoke(
                        std::forward<First>(first),
                        std::forward<Nullable>(opt),
                        std::forward<Others>(steps)...);
                },
                std::forward<Self>(self).m_Steps);
        }

        template <typename Self, typename... SuffixSteps>
        [[nodiscard]]
        static constexpr auto append(Self&& self, std::tuple<SuffixSteps...>&& suffixSteps)
        {
            using Appended = Pipeline<Steps..., SuffixSteps...>;

            return Appended{
                std::tuple_cat(std::forward<Self>(self).m_Steps, std::move(suffixSteps))};
        }
    };
}

#endif
