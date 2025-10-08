#include "gimo/Algorithm.hpp"

namespace
{



        /*
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
        constexpr auto operator()(detail::has_value_tag const, Nullable&& opt)
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
    };*/

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
