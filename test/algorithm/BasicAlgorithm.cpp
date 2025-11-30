//           Copyright Dominic (DNKpp) Koepke 2025.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

// We disable assertions here on purpose so that they do not interfere with our expectations.
#define GIMO_ASSERT(condition, msg, ...) (void(0))

#include "gimo/algorithm/BasicAlgorithm.hpp"
#include "gimo_ext/std_optional.hpp"

using namespace gimo;

namespace
{
    struct TestAlgorithmTraits
    {
        template <nullable Nullable>
        using output = std::optional<std::remove_cvref_t<reference_type_t<Nullable>>>;

        template <nullable Nullable, typename Action>
        static constexpr bool is_applicable_on = true;

        template <typename Action, nullable Nullable, typename... Steps>
        inline static mimicpp::Mock<std::remove_cvref_t<output<Nullable>>(Action, Nullable, Steps...)> on_value_{
            {"TestAlgorithmTraits::on_value", 1u}
        };

        template <typename Action, nullable Nullable, typename... Steps>
        [[nodiscard]]
        static constexpr auto on_value(Action&& action, Nullable&& opt, Steps&&... steps)
        {
            return on_value_<Action&&, Nullable&&, Steps&&...>(
                std::forward<Action>(action),
                std::forward<Nullable>(opt),
                std::forward<Steps>(steps)...);
        }

        template <nullable Nullable, typename Action, typename... Steps>
        inline static mimicpp::Mock<Nullable(Action, Steps...)> on_null_{
            {"TestAlgorithmTraits::on_null", 1u}
        };

        template <nullable Nullable, typename Action, typename... Steps>
        [[nodiscard]]
        static constexpr auto on_null(Action&& action, Steps&&... steps)
        {
            return on_null_<output<Nullable>, Action&&, Steps&&...>(
                std::forward<Action>(action),
                std::forward<Steps>(steps)...);
        }
    };

    struct NullableNull
    {
    };

    template <typename T>
    class NullableMock
    {
    public:
        NullableMock() = default;
        NullableMock(NullableMock&&) = default;
        NullableMock& operator=(NullableMock&&) = default;

        explicit(false) constexpr NullableMock([[maybe_unused]] NullableNull const null) noexcept
        {
        }

        mimicpp::Mock<
            T&()&,
            T const&() const&,
            T && ()&&,
            T const && () const&&>
            value{
                {.name = "NullableMock::value", .stacktraceSkip = 1u}
        };

        [[nodiscard]]
        constexpr T& operator*() &
        {
            return value();
        }

        [[nodiscard]]
        constexpr T const& operator*() const&
        {
            return value();
        }

        [[nodiscard]]
        constexpr T&& operator*() &&
        {
            return std::move(value)();
        }

        [[nodiscard]]
        constexpr T const&& operator*() const&&
        {
            return std::move(std::as_const(value))();
        }

        mimicpp::Mock<bool() const> is_null{
            {.name = "NullableMock::is_null", .stacktraceSkip = 1u}
        };

        [[nodiscard]]
        constexpr bool operator==([[maybe_unused]] NullableNull const null) const
        {
            return is_null();
        }

        mimicpp::Mock<void()> set_null{
            {.name = "NullableMock::set_null", .stacktraceSkip = 1u}
        };

        NullableMock& operator=([[maybe_unused]] NullableNull const null)
        {
            set_null();
            return *this;
        }
    };

    template <typename Action>
    using TestAlgorithm = BasicAlgorithm<TestAlgorithmTraits, Action>;

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

}

template <typename T>
struct gimo::traits<NullableMock<T>>
{
    static constexpr NullableNull null{};
};

static_assert(gimo::nullable<NullableMock<int>>);

TEMPLATE_TEST_CASE(
    "BasicAlgorithm::operator() propagates the nullable as-is.",
    "[algorithm]",
    as_lvalue_ref,
    as_const_lvalue_ref,
    as_rvalue_ref,
    as_const_rvalue_ref)
{
    using matches::_;

    using ActionMock = mimicpp::Mock<void()>;
    TestAlgorithm<ActionMock> algorithm{};
    NullableMock<int> opt{};
    mimicpp::ScopedSequence sequence{};

    using Cast = TestType;
    using ExpectedOptRef = Cast::template type<NullableMock<int>>;

    SECTION("When nullable contains a value.")
    {
        sequence += opt.is_null.expect_call()
                and finally::returns(false);

        SECTION("When algorithm is invoked as mutable lvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_value_<ActionMock&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                    and finally::returns(1337);

            std::optional<int> const result = algorithm(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as const lvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_value_<ActionMock const&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                    and finally::returns(1337);

            std::optional<int> const result = std::as_const(algorithm)(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as mutable rvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_value_<ActionMock&&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                    and finally::returns(1337);

            std::optional<int> const result = std::move(algorithm)(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as const rvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_value_<ActionMock const&&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                    and finally::returns(1337);

            std::optional<int> const result = std::move(std::as_const(algorithm))(Cast::cast(opt));
            CHECK(1337 == result);
        }
    }

    SECTION("When nullable is empty.")
    {
        sequence += opt.is_null.expect_call()
                and finally::returns(true);

        SECTION("When algorithm is invoked as mutable lvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock&>.expect_call(_)
                    and finally::returns(1337);

            std::optional<int> const result = algorithm(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as const lvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock const&>.expect_call(_)
                    and finally::returns(1337);

            std::optional<int> const result = std::as_const(algorithm)(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as mutable rvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock&&>.expect_call(_)
                    and finally::returns(1337);

            std::optional<int> const result = std::move(algorithm)(Cast::cast(opt));
            CHECK(1337 == result);
        }

        SECTION("When algorithm is invoked as const rvalue-ref.")
        {
            sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock const&&>.expect_call(_)
                    and finally::returns(1337);

            std::optional<int> const result = std::move(std::as_const(algorithm))(Cast::cast(opt));
            CHECK(1337 == result);
        }
    }
}

TEMPLATE_TEST_CASE(
    "BasicAlgorithm::on_value propagates the nullable as-is to Traits::on_value.",
    "[algorithm]",
    as_lvalue_ref,
    as_const_lvalue_ref,
    as_rvalue_ref,
    as_const_rvalue_ref)
{
    using matches::_;

    using ActionMock = mimicpp::Mock<void()>;
    TestAlgorithm<ActionMock> algorithm{};
    NullableMock<int> opt{};
    mimicpp::ScopedSequence sequence{};

    using Cast = TestType;
    using ExpectedOptRef = Cast::template type<NullableMock<int>>;

    SECTION("When algorithm is invoked as mutable lvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_value_<ActionMock&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                and finally::returns(1337);

        std::optional<int> const result = algorithm.on_value(Cast::cast(opt));
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as const lvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_value_<ActionMock const&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                and finally::returns(1337);

        std::optional<int> const result = std::as_const(algorithm).on_value(Cast::cast(opt));
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as mutable rvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_value_<ActionMock&&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                and finally::returns(1337);

        std::optional<int> const result = std::move(algorithm).on_value(Cast::cast(opt));
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as const rvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_value_<ActionMock const&&, ExpectedOptRef>.expect_call(_, matches::instance(opt))
                and finally::returns(1337);

        std::optional<int> const result = std::move(std::as_const(algorithm)).on_value(Cast::cast(opt));
        CHECK(1337 == result);
    }
}

TEST_CASE(
    "BasicAlgorithm::on_null calls Traits::on_null accordingly.",
    "[algorithm]")
{
    using matches::_;

    using ActionMock = mimicpp::Mock<void()>;
    TestAlgorithm<ActionMock> algorithm{};
    mimicpp::ScopedSequence sequence{};

    SECTION("When algorithm is invoked as mutable lvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock&>.expect_call(_)
                and finally::returns(1337);

        std::optional<int> const result = algorithm.on_null<NullableMock<int>>();
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as const lvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock const&>.expect_call(_)
                and finally::returns(1337);

        std::optional<int> const result = std::as_const(algorithm).on_null<NullableMock<int>>();
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as mutable rvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock&&>.expect_call(_)
                and finally::returns(1337);

        std::optional<int> const result = std::move(algorithm).on_null<NullableMock<int>>();
        CHECK(1337 == result);
    }

    SECTION("When algorithm is invoked as const rvalue-ref.")
    {
        sequence += TestAlgorithmTraits::on_null_<std::optional<int>, ActionMock const&&>.expect_call(_)
                and finally::returns(1337);

        std::optional<int> const result = std::move(std::as_const(algorithm)).on_null<NullableMock<int>>();
        CHECK(1337 == result);
    }
}
