#pragma once

#define EXPECT_GENERAL_STREQ(str1, str2) \
    EXPECT_PRED_FORMAT2(::Testing::Internal::StringsEqual, str1, str2)

namespace Testing::Internal
{
    template<typename TChar>
    ::testing::AssertionResult StringsEqual(const char* str1Expr, const char* str2Expr,
                                            const TChar* str1, const TChar* str2)
    {
        using CharTraits_ = std::char_traits<TChar>;
        std::size_t str1Size = CharTraits_::length(str1);
        std::size_t str2Size = CharTraits_::length(str2);

        if ((str1Size == str2Size) &&
            (CharTraits_::compare(str1, str2, str1Size) == 0))
        {
            return ::testing::AssertionSuccess();
        }

        return ::testing::internal::EqFailure(
            str1Expr, str2Expr,
            ::testing::PrintToString(str1), ::testing::PrintToString(str2),
            false);
    }
}
