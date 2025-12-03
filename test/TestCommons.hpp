//           Copyright Dominic (DNKpp) Koepke 2025.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <type_traits>

namespace gimo::testing
{
    struct as_lvalue_ref
    {
        template <typename T>
        using type = std::remove_cvref_t<T>&;

        template <typename T>
        [[nodiscard]]
        static constexpr type<T> cast(T& obj) noexcept
        {
            return static_cast<type<T>>(obj);
        }
    };

    struct as_const_lvalue_ref
    {
        template <typename T>
        using type = std::remove_cvref_t<T> const&;

        template <typename T>
        [[nodiscard]]
        static constexpr type<T> cast(T& obj) noexcept
        {
            return static_cast<type<T>>(obj);
        }
    };

    struct as_rvalue_ref
    {
        template <typename T>
        using type = std::remove_cvref_t<T>&&;

        template <typename T>
        [[nodiscard]]
        static constexpr type<T> cast(T&& obj) noexcept
        {
            return static_cast<type<T>>(obj);
        }
    };

    struct as_const_rvalue_ref
    {
        template <typename T>
        using type = std::remove_cvref_t<T> const&&;

        template <typename T>
        [[nodiscard]]
        static constexpr type<T> cast(T&& obj) noexcept
        {
            return static_cast<type<T>>(obj);
        }
    };

    using with_qualification_list = std::tuple<
        as_lvalue_ref,
        as_const_lvalue_ref,
        as_rvalue_ref,
        as_const_rvalue_ref>;
}
