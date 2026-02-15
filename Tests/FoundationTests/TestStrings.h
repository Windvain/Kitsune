#pragma once

#include <gtest/gtest.h>

#define EXPECT_GENERAL_STREQ(string1, string2) \
    EXPECT_PRED_FORMAT2(::Kitsune::Details::StringsEqual, string1, string2)

namespace Kitsune::Details
{
    template<typename Char>
    ::testing::AssertionResult StringsEqual(
        const char* expr1, const char* expr2,
        const Char* string1, const Char* string2)
    {
        using CharTraits = std::char_traits<Char>;
        std::size_t string1Size = CharTraits::length(string1);
        std::size_t string2Size = CharTraits::length(string2);

        if ((string1Size == string2Size) &&
            (CharTraits::compare(string1, string2, string1Size) == 0))
        {
            return ::testing::AssertionSuccess();
        }

        return ::testing::internal::EqFailure(
            expr1, expr2,
            ::testing::PrintToString(string1),
            ::testing::PrintToString(string2),
            false);
    }
}
