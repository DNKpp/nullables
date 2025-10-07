
namespace
{
    struct has_value_tag
    {
    };

    struct is_empty_tag
    {
    };

    template <typename Nullable>
    struct traits;

    template <typename Nullable>
    inline constexpr auto null_v{traits<std::remove_cvref_t<Nullable>>::null};

    template <typename Nullable, typename Value>
    using rebind_value_t = typename traits<Nullable>::template rebind_value<Value>;

    template <typename Nullable>
    using reference_type_t = decltype(value(std::declval<Nullable&&>()));

    template <typename T>
    concept referencable = std::is_reference_v<T&>;

    template <typename B>
    concept boolean_testable =
        std::convertible_to<B, bool>
        && requires(B&& b) {
               { !std::forward<B>(b) } -> std::convertible_to<bool>;
           };

    template <typename T, typename U>
    concept weakly_equality_comparable_with =
        requires(T const& t, U const& u) {
            { t == u } -> boolean_testable;
            { t != u } -> boolean_testable;
            { u == t } -> boolean_testable;
            { u != t } -> boolean_testable;
        };

    template <typename Null, typename Nullable>
    concept null_for = weakly_equality_comparable_with<Null, Nullable>
        && std::constructible_from<Nullable, Null>
        && std::assignable_from<Nullable&, Null>;

    template <typename T>
    concept nullable = requires(T&& obj) {
        requires null_for<decltype(null_v<T>), T>;
        {value(std::forward<T>(obj))} -> referencable;
    };

    template <typename T, typename Nullable>
    concept rebindable_to = nullable<Nullable>
                         && requires(T&& obj) {
                                {
                                    typename traits<Nullable>::template rebind_value<std::remove_cvref_t<T>>{std::forward<T>(obj)}
                                } -> nullable;
                            };

    namespace detail
    {
        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto construct_empty()
        {
            return Nullable{null_v<Nullable>};
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr bool has_value(Nullable const& target)
        {
            return target == null_v<Nullable>;
        }

        template <typename Nullable, typename Value>
        [[nodiscard]]
        constexpr auto rebind_value(Value&& value)
        {
            return rebind_value_t<Nullable, Value>{std::forward<Value>(value)};
        }

        template <typename T, typename U>
        [[nodiscard]]
        constexpr auto&& forward_like(U&& x) noexcept
        {
            constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
            if constexpr (std::is_lvalue_reference_v<T&&>)
            {
                if constexpr (is_adding_const)
                {
                    return std::as_const(x);
                }
                else
                {
                    return static_cast<U&>(x);
                }
            }
            else
            {
                if constexpr (is_adding_const)
                {
                    return std::move(std::as_const(x));
                }
                else
                {
                    return std::move(x); // NOLINT(*-move-forwarding-reference)
                }
            }
        }

        template <typename T, typename U>
        using cast_like_t = decltype(forward_like<T>(std::declval<U>()));
    }

    template <typename Derived>
    class ComposableAlgorithmBase
    {
    public:
        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &
            requires Derived::template is_applicable_for<Derived&, Nullable&&>
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(self(), has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return Derived::execute(self(), is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&
            requires Derived::template is_applicable_for<Derived const&, Nullable&&>
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(self(), has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return Derived::execute(self(), is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) &&
            requires Derived::template is_applicable_for<Derived&&, Nullable&&>
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(std::move(*this).self(), has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return Derived::execute(std::move(*this).self(), is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps) const&&
            requires Derived::template is_applicable_for<Derived const&&, Nullable&&>
        {
            if (detail::has_value(opt))
            {
                return Derived::execute(std::move(*this).self(), has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return Derived::execute(std::move(*this).self(), is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) &
            requires Derived::template is_applicable_for<Derived&, Nullable&&>
        {
            return Derived::execute(
                self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) const&
            requires Derived::template is_applicable_for<Derived const&, Nullable&&>
        {
            return Derived::execute(
                self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) &&
            requires Derived::template is_applicable_for<Derived&&, Nullable&&>
        {
            return Derived::execute(
                std::move(*this).self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto&... steps) const&&
            requires Derived::template is_applicable_for<Derived const&&, Nullable&&>
        {
            return Derived::execute(
                std::move(*this).self(),
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) &
            requires Derived::template is_applicable_for<Derived&, Nullable&&>
        {
            return Derived::execute(
                self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) const&
            requires Derived::template is_applicable_for<Derived const&, Nullable&&>
        {
            return Derived::execute(
                self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) &&
            requires Derived::template is_applicable_for<Derived&&, Nullable&&>
        {
            return Derived::execute(
                std::move(*this).self(),
                is_empty_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <nullable Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, Nullable&& opt, auto&... steps) const&&
            requires Derived::template is_applicable_for<Derived const&&, Nullable&&>
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
            static_assert(std::derived_from<Derived, ComposableAlgorithmBase>, "Derived must inherit from ComposableAlgorithmBase");
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

    template <typename Action>
    class AndThenAlgorithm
        : private ComposableAlgorithmBase<AndThenAlgorithm<Action>>
    {
        friend class ComposableAlgorithmBase<AndThenAlgorithm>;

    public:
        using ComposableAlgorithmBase<AndThenAlgorithm>::operator();

        [[nodiscard]]
        explicit constexpr AndThenAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

    private:
        template <typename Self, typename Nullable>
            requires std::same_as<AndThenAlgorithm, std::remove_cvref_t<Self>>
        static constexpr bool is_applicable_for = requires {
            requires nullable<
                std::invoke_result_t<
                    detail::cast_like_t<Self, Action>,
                    reference_type_t<Nullable>>>;
        };

        Action m_Action;

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute(Self&& self, has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                std::invoke(
                    std::forward<Self>(self).action,
                    value(std::forward<Nullable>(opt))),
                steps...);
        }

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute([[maybe_unused]] Self&& self, is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            using Result = std::invoke_result_t<Action, Nullable&&>;

            return std::invoke(
                first,
                is_empty_tag{},
                detail::construct_empty<Result>(),
                steps...);
        }
    };

    template <typename Action>
    class OrElseAlgorithm
        : private ComposableAlgorithmBase<OrElseAlgorithm<Action>>
    {
    public:
        using ComposableAlgorithmBase<OrElseAlgorithm>::operator();

        [[nodiscard]]
        explicit constexpr OrElseAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

    private:
        template <typename Self, typename Nullable>
            requires std::same_as<OrElseAlgorithm, std::remove_cvref_t<Self>>
        static constexpr bool is_applicable_for = requires {
            requires std::same_as<
                std::remove_cvref_t<Nullable>,
                std::remove_cvref_t<std::invoke_result_t<detail::cast_like_t<Self, Action>>>>;
        };

        Action m_Action;

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute([[maybe_unused]] Self&& self, has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute(Self&& self, is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                std::invoke(std::forward<Self>(self).action),
                steps...);
        }
    };

    template <typename Action>
    struct TransformAlgorithm
        : private ComposableAlgorithmBase<TransformAlgorithm<Action>>
    {
    public:
        using ComposableAlgorithmBase<TransformAlgorithm>::operator();

        [[nodiscard]]
        explicit constexpr TransformAlgorithm(Action action) noexcept(std::is_nothrow_move_constructible_v<Action>)
            : m_Action{std::move(action)}
        {
        }

    private:
        template <typename Self, typename Nullable>
            requires std::same_as<TransformAlgorithm, std::remove_cvref_t<Self>>
        static constexpr bool is_applicable_for = requires {
            requires rebindable_to<
                std::invoke_result_t<
                    detail::cast_like_t<Self, Action>,
                    reference_type_t<Nullable>>,
                std::remove_cvref_t<Nullable>>;
        };

        Action m_Action;

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute(Self&& self, has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                has_value_tag{},
                detail::rebind_value<Nullable>(
                    std::invoke(
                        std::forward<Self>(self).action,
                        value(std::forward<Nullable>(opt)))),
                steps...);
        }

        template <typename Self, typename Nullable>
        [[nodiscard]]
        static constexpr auto execute([[maybe_unused]] Self&& self, is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            using Result = std::invoke_result_t<Action, decltype(value(std::forward<Nullable>()))>;

            return std::invoke(
                first,
                is_empty_tag{},
                detail::construct_empty<rebind_value_t<Nullable, Result>>(),
                steps...);
        }
    };

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
        constexpr auto operator()(has_value_tag const, Nullable&& opt)
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
    };

    template <typename... Steps>
    struct pipeline
    {
        template <typename Step>
        constexpr auto append(Step&& step) const
        {
            return pipeline<Steps..., std::remove_cvref_t<Step>>{
                std::tuple_cat(steps, std::make_tuple(std::forward<Step>(step)))};
        }

        template <typename Nullable>
        constexpr auto apply(Nullable&& opt) const
        {
            return std::apply(
                [&](auto& first, auto&... steps) {
                    return std::invoke(
                        first,
                        std::forward<Nullable>(opt),
                        steps...);
                });
        }

        std::tuple<Steps...> steps{};
    };
}

TEST_CASE("")
{
    /*pipeline{}
        .append(AndThenAlgorithm<int>{})
        .append(OrElseAlgorithm<int>{})
        .append(TransformAlgorithm<int>{})
        .append(value_or_algorithm<int>{});*/
}
