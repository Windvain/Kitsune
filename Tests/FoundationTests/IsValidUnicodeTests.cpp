/*#include <gtest/gtest.h>
#include "Foundation/Unicode/IsValidUnicode.h"

using namespace Kitsune;

namespace
{
    template<typename T>
    class MyForwardIterator
    {
    public:
        using ValueType = T;
        using DifferenceType = Ptrdiff;

    public:
        MyForwardIterator(T* pointer = nullptr)
            : m_Pointer(pointer)
        {
        }

        MyForwardIterator(const MyForwardIterator<T>&) = default;
        MyForwardIterator<T>& operator=(const MyForwardIterator<T>&) = default;

    public:
        T& operator*() const { return *m_Pointer; }
        MyForwardIterator<T>& operator++()
        {
            ++m_Pointer;
            return *this;
        }

        MyForwardIterator<T> operator++(int)
        {
            MyForwardIterator<T> copy = *this;
            ++m_Pointer;

            return copy;
        }

    public:
        bool operator==(MyForwardIterator<T> iter) const
        {
            return m_Pointer == iter.m_Pointer;
        }

    private:
        T* m_Pointer;
    };

    static_assert(ForwardIterator<MyForwardIterator<char>>,
                  "MyForwardIterator<T> doesn't satisfy the requirements of a ForwardIterator.");
}

template<typename T>
class IsValidUtf8Tests : public ::testing::Test
{
public:
    using CharType = T;

protected:

protected:
    std::basic_string_view<T> GetAsciiString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Hello there, I am a string!";
        else
            return u8"I am a UTF-8 string... The new kid on the block..";
    }

    std::basic_string_view<T> GetChineseString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "你好，我叫Windvain！";
        else
            return u8"家乡的众多美食中，萝卜饼是最让我怀念的。";
    }

    std::basic_string_view<T> GetValidEmojiString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "😊😊😂😘💕😁❤️";
        else
            return u8"🙌👍👌😢😅🤗🤩😙🥰🤨";
    }

protected:
    std::basic_string_view<T> GetInvalidString()
    {
        // <C0 AF> is invalid, `C0` is not a valid leading byte.
        // <F5 81> is also invalid for the same reason.
        if constexpr (std::is_same_v<T, char>)
            return "\xC0\xAF时钟";
        else
            return reinterpret_cast<const char8_t*>("是风扇\xF5\x81;asdk");
    }

    std::basic_string_view<T> GetInvalidString2()
    {
        // <E0 9F BF> is not valid, E0's second byte must be in the range [A0, BF].
        // <ED AF 93> is invalid for the same reason, but with the range [80, 9F].
        if constexpr (std::is_same_v<T, char>)
            return "笔记本\xE0\x9F\xBF手机";
        else
            return reinterpret_cast<const char8_t*>("镜子\xED\xAF\x93眼镜 耳机");
    }

    std::basic_string_view<T> GetInvalidString3()
    {
        // Same reason as GetInvalidString2(), just with different ranges.
        if constexpr (std::is_same_v<T, char>)
            return "植物\xF0\x85\x80\x80魔方";
        else
            return reinterpret_cast<const char8_t*>("橡皮檫\xF4\x90\xA2\xB3键盘");
    }

    std::basic_string_view<T> GetInvalidString4()
    {
        // Headers are correct, missing a byte or two.
        if constexpr (std::is_same_v<T, char>)
            return "💩🤖🦝🦒🙊\xF4\x80\x83😝🫤😭😲😧🫠😨";
        else
            return reinterpret_cast<const char8_t*>("🙈🙀🐼🐸🐷🐼\xF4\x90\xA2\xB3");
    }

    std::basic_string_view<T> GetInvalidString5()
    {
        // Headers are correct, missing a byte or two.
        if constexpr (std::is_same_v<T, char>)
            return "💩🤖🦝🦒🙊\xED\xA0\xA5😝🫤😭😲😧🫠😨";
        else
            return reinterpret_cast<const char8_t*>("↕️😈💩👾🐼🦐\xED\xB0\xA7😡🤧🤬🤕🙂‍🐦‍🔥");
    }
};

template<typename T>
class IsValidUtf16Tests : public ::testing::Test
{
public:
    using CharType = T;

protected:

protected:
    std::basic_string_view<T> GetAsciiString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"Hello there, I am a string!";
        else
            return u"I am a UTF-16 string... The new kid on the block..";
    }

    std::basic_string_view<T> GetChineseString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"你好，我叫Windvain！";
        else
            return u"家乡的众多美食中，萝卜饼是最让我怀念的。";
    }

    std::basic_string_view<T> GetValidEmojiString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"😊😊😂😘💕😁❤️";
        else
            return u"🙌👍👌😢😅🤗🤩😙🥰🤨";
    }

protected:
    std::basic_string_view<T> GetInvalidString()
    {
        // Hanging high surrogates.
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"\xD800时钟";
        else
            return reinterpret_cast<const char16_t*>(L"是风扇\xD9C2");
    }

    std::basic_string_view<T> GetInvalidString2()
    {
        // Hanging low surrogates.
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"笔记本\xDD32手机";
        else
            return reinterpret_cast<const char16_t*>(L"镜子\xDEAF 耳机");
    }
};

template<typename T>
class IsValidUtf32Tests : public ::testing::Test
{
public:
    using CharType = T;

protected:

protected:
    std::basic_string_view<T> GetAsciiString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"Hello there, I am a string!";
        else
            return U"I am a UTF-32 string... The new kid on the block..";
    }

    std::basic_string_view<T> GetChineseString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"你好，我叫Windvain！";
        else
            return U"家乡的众多美食中，萝卜饼是最让我怀念的。";
    }

    std::basic_string_view<T> GetValidEmojiString()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"😊😊😂😘💕😁❤️";
        else
            return U"🙌👍👌😢😅🤗🤩😙🥰🤨";
    }

protected:
    std::basic_string_view<T> GetInvalidString()
    {
        // Hanging high surrogates.
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"\xFF\xFF\xFF\xFF时钟";
        else
            return reinterpret_cast<const char32_t*>(L"是风扇\x20\xFF\xF2\xA1");
    }

    std::basic_string_view<T> GetInvalidString2()
    {
        // Hanging low surrogates.
        if constexpr (std::is_same_v<T, wchar_t>)
            return L"笔记本\xD8\x00\xDD\x32手机";
        else
            return reinterpret_cast<const char32_t*>(L"镜子\xD9\xC2\xDE\xAF 耳机");
    }
};

using IsValidUtf8TestsImplementations = ::testing::Types<char, char8_t>;
using IsValidUtf16TestsImplementations = ::testing::Types<
    char16_t
#if (WCHAR_MAX == 0xFFFF) || (WCHAR_MAX == 0x7FFF)
    , wchar_t
#endif
>;

using IsValidUtf32TestsImplementations = ::testing::Types<
    char32_t
#if (WCHAR_MAX == 0xFFFFFFFF) || (WCHAR_MAX == 0x7FFFFFFF)
    , wchar_t
#endif
>;

TYPED_TEST_SUITE(IsValidUtf8Tests, IsValidUtf8TestsImplementations);
TYPED_TEST_SUITE(IsValidUtf16Tests, IsValidUtf16TestsImplementations);
TYPED_TEST_SUITE(IsValidUtf32Tests, IsValidUtf32TestsImplementations);

TYPED_TEST(IsValidUtf8Tests, ActuallyValid)
{
    using T = typename TestFixture::CharType;

    auto asciiString = this->GetAsciiString();
    MyForwardIterator<const T> asciiBegin(asciiString.data());
    MyForwardIterator<const T> asciiEnd(asciiString.data() + asciiString.size());

    auto emojiString = this->GetValidEmojiString();
    MyForwardIterator<const T> emojiBegin(emojiString.data());
    MyForwardIterator<const T> emojiEnd(emojiString.data() + emojiString.size());

    auto chineseString = this->GetChineseString();
    MyForwardIterator<const T> chineseBegin(chineseString.data());
    MyForwardIterator<const T> chineseEnd(chineseString.data() + chineseString.size());

    EXPECT_TRUE(IsValidUtf8(asciiBegin, asciiEnd));
    EXPECT_TRUE(IsValidUtf8(emojiBegin, emojiEnd));
    EXPECT_TRUE(IsValidUtf8(chineseBegin, chineseEnd));
}

TYPED_TEST(IsValidUtf8Tests, InvalidBytes)
{
    using T = typename TestFixture::CharType;

    auto invalidString1 = this->GetInvalidString();
    MyForwardIterator<const T> string1Begin(invalidString1.data());
    MyForwardIterator<const T> string1End(invalidString1.data() + invalidString1.size());

    auto invalidString2 = this->GetInvalidString2();
    MyForwardIterator<const T> string2Begin(invalidString2.data());
    MyForwardIterator<const T> string2End(invalidString2.data() + invalidString2.size());

    auto invalidString3 = this->GetInvalidString3();
    MyForwardIterator<const T> string3Begin(invalidString3.data());
    MyForwardIterator<const T> string3End(invalidString3.data() + invalidString3.size());

    EXPECT_FALSE(IsValidUtf8(string1Begin, string1End));
    EXPECT_FALSE(IsValidUtf8(string2Begin, string2End));
    EXPECT_FALSE(IsValidUtf8(string3Begin, string3End));
}

TYPED_TEST(IsValidUtf8Tests, InvalidLength)
{
    using T = typename TestFixture::CharType;
    auto invalidString4 = this->GetInvalidString4();

    MyForwardIterator<const T> string4Begin(invalidString4.data());
    MyForwardIterator<const T> string4End(invalidString4.data() + invalidString4.size());

    EXPECT_FALSE(IsValidUtf8(string4Begin, string4End));
}

TYPED_TEST(IsValidUtf8Tests, SurrogateCheck)
{
    using T = typename TestFixture::CharType;
    auto invalidString5 = this->GetInvalidString5();

    MyForwardIterator<const T> string5Begin(invalidString5.data());
    MyForwardIterator<const T> string5End(invalidString5.data() + invalidString5.size());

    EXPECT_FALSE(IsValidUtf8(string5Begin, string5End));
}

TYPED_TEST(IsValidUtf16Tests, ActuallyValid)
{
    using T = typename TestFixture::CharType;

    auto asciiString = this->GetAsciiString();
    MyForwardIterator<const T> asciiBegin(asciiString.data());
    MyForwardIterator<const T> asciiEnd(asciiString.data() + asciiString.size());

    auto emojiString = this->GetValidEmojiString();
    MyForwardIterator<const T> emojiBegin(emojiString.data());
    MyForwardIterator<const T> emojiEnd(emojiString.data() + emojiString.size());

    auto chineseString = this->GetChineseString();
    MyForwardIterator<const T> chineseBegin(chineseString.data());
    MyForwardIterator<const T> chineseEnd(chineseString.data() + chineseString.size());

    EXPECT_TRUE(IsValidUtf16(asciiBegin, asciiEnd));
    EXPECT_TRUE(IsValidUtf16(emojiBegin, emojiEnd));
    EXPECT_TRUE(IsValidUtf16(chineseBegin, chineseEnd));
}

TYPED_TEST(IsValidUtf16Tests, HangingSurrogates)
{
    using T = typename TestFixture::CharType;

    auto invalidString1 = this->GetInvalidString();
    MyForwardIterator<const T> string1Begin(invalidString1.data());
    MyForwardIterator<const T> string1End(invalidString1.data() + invalidString1.size());

    auto invalidString2 = this->GetInvalidString2();
    MyForwardIterator<const T> string2Begin(invalidString2.data());
    MyForwardIterator<const T> string2End(invalidString2.data() + invalidString2.size());

    EXPECT_FALSE(IsValidUtf16(string1Begin, string1End));
    EXPECT_FALSE(IsValidUtf16(string2Begin, string2End));
}

TYPED_TEST(IsValidUtf32Tests, ActuallyValid)
{
    using T = typename TestFixture::CharType;

    auto asciiString = this->GetAsciiString();
    MyForwardIterator<const T> asciiBegin(asciiString.data());
    MyForwardIterator<const T> asciiEnd(asciiString.data() + asciiString.size());

    auto emojiString = this->GetValidEmojiString();
    MyForwardIterator<const T> emojiBegin(emojiString.data());
    MyForwardIterator<const T> emojiEnd(emojiString.data() + emojiString.size());

    auto chineseString = this->GetChineseString();
    MyForwardIterator<const T> chineseBegin(chineseString.data());
    MyForwardIterator<const T> chineseEnd(chineseString.data() + chineseString.size());

    EXPECT_TRUE(IsValidUtf32(asciiBegin, asciiEnd));
    EXPECT_TRUE(IsValidUtf32(emojiBegin, emojiEnd));
    EXPECT_TRUE(IsValidUtf32(chineseBegin, chineseEnd));
}

TYPED_TEST(IsValidUtf32Tests, PlainInvalid)
{
    using T = typename TestFixture::CharType;

    auto invalidString1 = this->GetInvalidString();
    MyForwardIterator<const T> string1Begin(invalidString1.data());
    MyForwardIterator<const T> string1End(invalidString1.data() + invalidString1.size());

    auto invalidString2 = this->GetInvalidString2();
    MyForwardIterator<const T> string2Begin(invalidString2.data());
    MyForwardIterator<const T> string2End(invalidString2.data() + invalidString2.size());

    EXPECT_FALSE(IsValidUtf32(string1Begin, string1End));
    EXPECT_FALSE(IsValidUtf32(string2Begin, string2End));
}*/
