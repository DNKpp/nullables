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

namespace finally = mimicpp::finally;

template <typename T>
struct gimo::traits<std::optional<T>>
{
    static constexpr auto null{std::nullopt};
};

TEST_CASE(
    "AndThenAlgorithm invokes its action only when the input has a value."
    "[algorithm]")
{
    mimicpp::Mock<
        std::optional<int>(std::optional<float>)&,
        std::optional<int>(std::optional<float>) const&,
        std::optional<int>(std::optional<float>)&&,
        std::optional<int>(std::optional<float>) const&&>
        action{};

    using Algorithm = gimo::AndThenAlgorithm<decltype(action)>;
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm const&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm&&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<float>, Algorithm const&&>);

    SECTION("When input has a value, the action is invoked.")
    {
        constexpr std::optional opt{1337.f};
        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            SCOPED_EXP action.expect_call(1337.f)
                and finally::returns(std::optional{42});
            gimo::AndThenAlgorithm andThen{std::move(action)};
            decltype(auto) result = andThen(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            SCOPED_EXP std::as_const(action).expect_call(1337.f)
                and finally::returns(std::optional{42});
            gimo::AndThenAlgorithm andThen{std::move(action)};
            decltype(auto) result = std::as_const(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            SCOPED_EXP std::move(action).expect_call(1337.f)
                and finally::returns(std::optional{42});
            gimo::AndThenAlgorithm andThen{std::move(action)};
            decltype(auto) result = std::move(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            SCOPED_EXP std::move(std::as_const(action)).expect_call(1337.f)
                and finally::returns(std::optional{42});
            gimo::AndThenAlgorithm andThen{std::move(action)};
            decltype(auto) result = std::move(std::as_const(andThen))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }
    }

    SECTION("When input is empty, action is not invoked.")
    {
        gimo::AndThenAlgorithm andThen{std::move(action)};
        constexpr std::optional<float> opt{};

        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            decltype(auto) result = andThen(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            decltype(auto) result = std::as_const(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            decltype(auto) result = std::move(andThen)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            decltype(auto) result = std::move(std::as_const(andThen))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(!result);
        }
    }
}

TEST_CASE(
    "OrElseAlgorithm invokes its action only when the input has no value."
    "[algorithm]")
{
    mimicpp::Mock<
        std::optional<int>()&,
        std::optional<int>() const&,
        std::optional<int>()&&,
        std::optional<int>() const&&>
        action{};

    using Algorithm = gimo::OrElseAlgorithm<decltype(action)>;
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm const&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm&&>);
    STATIC_REQUIRE(gimo::detail::applicable_on<std::optional<int>, Algorithm const&&>);

    SECTION("When input has no value, the action is invoked.")
    {
        constexpr std::optional<int> opt{};
        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            SCOPED_EXP action.expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = orElse(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            SCOPED_EXP std::as_const(action).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::as_const(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            SCOPED_EXP std::move(action).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::move(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            SCOPED_EXP std::move(std::as_const(action)).expect_call()
                and finally::returns(std::optional{42});
            gimo::OrElseAlgorithm orElse{std::move(action)};
            decltype(auto) result = std::move(std::as_const(orElse))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(42 == result);
        }
    }

    SECTION("When input has a value, the action is not invoked.")
    {
        gimo::OrElseAlgorithm orElse{std::move(action)};
        constexpr std::optional opt{1337};

        SECTION("When algorithm is used via lvalue-ref overload.")
        {
            decltype(auto) result = orElse(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via const lvalue-ref overload.")
        {
            decltype(auto) result = std::as_const(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via rvalue-ref overload.")
        {
            decltype(auto) result = std::move(orElse)(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }

        SECTION("When algorithm is used via const rvalue-ref overload.")
        {
            decltype(auto) result = std::move(std::as_const(orElse))(opt);
            STATIC_REQUIRE(std::same_as<std::optional<int>, decltype(result)>);
            CHECK(result);
        }
    }
}
