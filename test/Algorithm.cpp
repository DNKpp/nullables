
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

    template <typename T>
    concept nullable = requires(T& object) {
        { traits<T>::null } -> std::convertible_to<T>;
        { std::as_const(object) == traits<T>::null } -> std::convertible_to<bool>;
        { object = traits<T>::null } -> std::same_as<T&>;
    };

    template <typename T, typename Nullable>
    concept rebindable_in = nullable<Nullable>
                         && requires(T&& obj) {
                                {
                                    typename traits<Nullable>::template rebind_value<std::remove_cvref_t<T>>{std::forward<T>(obj)}
                                } -> nullable;
                            };

    template <typename Nullable>
    inline constexpr auto null_v{traits<Nullable>::null};

    template <typename Nullable, typename Value>
    using rebind_value_t = typename traits<Nullable>::template rebind_value<Value>;

    namespace detail
    {
        template <typename Nullable>
        [[nodiscard]]
        constexpr auto make_null()
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
    }

    template <typename Action>
    struct and_then_algorithm
    {
        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps)
        {
            if (detail::has_value(opt))
            {
                return std::invoke(*this, has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return std::invoke(*this, is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                std::invoke(action, value(std::forward<Nullable>(opt))),
                steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            using Result = std::invoke_result_t<Action, Nullable&&>;

            return std::invoke(
                first,
                is_empty_tag{},
                detail::make_null<Result>(),
                steps...);
        }

        Action action;
    };

    template <typename Action>
    struct or_else_algorithm
    {
        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps)
        {
            if (detail::has_value(opt))
            {
                return std::invoke(*this, has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return std::invoke(*this, is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                has_value_tag{},
                std::forward<Nullable>(opt),
                steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                std::invoke(action),
                steps...);
        }

        Action action;
    };

    template <typename Action>
    struct transform_algorithm
    {
        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(Nullable&& opt, auto&... steps)
        {
            if (detail::has_value(opt))
            {
                return std::invoke(*this, has_value_tag{}, std::forward<Nullable>(), steps...);
            }

            return std::invoke(*this, is_empty_tag{}, std::forward<Nullable>(), steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(has_value_tag const, Nullable&& opt, auto& first, auto&... steps)
        {
            return std::invoke(
                first,
                has_value_tag{},
                detail::rebind_value<Nullable>(std::invoke(action, value(std::forward<Nullable>(opt)))),
                steps...);
        }

        template <typename Nullable>
        [[nodiscard]]
        constexpr auto operator()(is_empty_tag const, [[maybe_unused]] Nullable&& opt, auto& first, auto&... steps)
        {
            using Result = std::invoke_result_t<Action, decltype(value(std::forward<Nullable>()))>;

            return std::invoke(
                first,
                is_empty_tag{},
                detail::make_null<rebind_value_t<Nullable, Result>>(),
                steps...);
        }

        Action action;
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
    pipeline{}
        .append(and_then_algorithm<int>{})
        .append(or_else_algorithm<int>{})
        .append(transform_algorithm<int>{})
        .append(value_or_algorithm<int>{});
}
