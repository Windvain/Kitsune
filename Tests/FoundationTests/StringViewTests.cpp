#include <gtest/gtest.h>
#include "Foundation/String/StringView.h"

using namespace Kitsune;

template<typename T>
class StringViewTests : public ::testing::Test
{
public:
    using CharType = T;

protected:
    StringViewTests() { /* ... */ }
    ~StringViewTests() { /* ... */ }

protected:
    const T* GetCString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Hello there, I am a string!";
        else if constexpr (std::is_same_v<T, wchar_t>)
            return L"I am a wide string! How about you?";
        else if constexpr (std::is_same_v<T, char8_t>)
            return u8"I am a UTF-8 string... The new kid on the block..";
        else if constexpr (std::is_same_v<T, char16_t>)
            return u"Hey UTF-32, look at us old strings...";
        else
            return U"Yeah... we're ooooolllldddd...";
    }

    const T* GetShortCString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Hello World!";
        else if constexpr (std::is_same_v<T, wchar_t>)
            return L"Wiidde";
        else if constexpr (std::is_same_v<T, char8_t>)
            return u8"Hello there..";
        else if constexpr (std::is_same_v<T, char16_t>)
            return u"..uhh..";
        else
            return U"uhh";
    }

    const T* GetFoundString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Mouse";
        else if constexpr (std::is_same_v<T, wchar_t>)
            return L"Mouse";
        else if constexpr (std::is_same_v<T, char8_t>)
            return u8"Mouse";
        else if constexpr (std::is_same_v<T, char16_t>)
            return u"Mouse";
        else
            return U"Mouse";
    }

    const T* GetNotFoundString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Standee";
        else if constexpr (std::is_same_v<T, wchar_t>)
            return L"Lamp";
        else if constexpr (std::is_same_v<T, char8_t>)
            return u8"Cubes";
        else if constexpr (std::is_same_v<T, char16_t>)
            return u"Phone Stand";
        else
            return U"Laptop";
    }

    const T* GetFindString()
    {
        if constexpr (std::is_same_v<T, char>)
            return "Earphones Mouse Pen Eraser Remote";
        else if constexpr (std::is_same_v<T, wchar_t>)
            return L"Earphones Mouse Pen Eraser Remote";
        else if constexpr (std::is_same_v<T, char8_t>)
            return u8"Earphones Mouse Pen Eraser Remote";
        else if constexpr (std::is_same_v<T, char16_t>)
            return u"Earphones Mouse Pen Eraser Remote";
        else
            return U"Earphones Mouse Pen Eraser Remote";
    }
};

using StringViewTestsImpl =
    ::testing::Types<
        char,
        wchar_t,
        char8_t,
        char16_t,
        char32_t>;

TYPED_TEST_SUITE(StringViewTests, StringViewTestsImpl);

// BasicStringView<T>()
TYPED_TEST(StringViewTests, DefaultConstructor)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> stringView{};
    EXPECT_EQ(stringView.Data(), nullptr);
    EXPECT_EQ(stringView.Size(), 0);
}

// BasicStringView<T>(const T* string, Usize size)
TYPED_TEST(StringViewTests, CstringSizeConstructor)
{
    using T = typename TestFixture::CharType;
    const T* cstring = this->GetCString();

    BasicStringView<T> stringView(cstring + 2, cstring + 5);
    EXPECT_EQ(stringView.Data(), cstring + 2);
    EXPECT_EQ(stringView.Size(), 3);
}

// BasicStringView<T>(const T* string)
TYPED_TEST(StringViewTests, CstringConstructor)
{
    using T = typename TestFixture::CharType;

    const T* cstring = this->GetCString();
    Usize expectedSize = std::char_traits<T>::length(cstring);

    BasicStringView<T> stringView(cstring);
    EXPECT_EQ(stringView.Data(), cstring);
    EXPECT_EQ(stringView.Size(), expectedSize);
}

// BasicStringView<T>(It begin, It end)
TYPED_TEST(StringViewTests, RangeConstructor)
{
    using T = typename TestFixture::CharType;

    const T* cstring = this->GetCString();
    Usize expectedSize = std::char_traits<T>::length(cstring);

    BasicStringView<T> stringView(cstring);
    EXPECT_EQ(stringView.Data(), cstring);
    EXPECT_EQ(stringView.Size(), expectedSize);
}

// const T& operator[](Index index) const
TYPED_TEST(StringViewTests, SubscriptOperator)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> stringView = this->GetCString();

    for (Index i = 0; i < stringView.Size(); ++i)
    {
        EXPECT_EQ(stringView[i], stringView.Data()[i]);
    }
}

// const T& Front() const
// const T& Back() const
TYPED_TEST(StringViewTests, FrontAndBackGetters)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> string = this->GetCString();

    EXPECT_EQ(string.Front(), string.Data()[0]);
    EXPECT_EQ(string.Back(), string.Data()[string.Size() - 1]);
}

// ConstIterator GetBegin() const
// ConstIterator GetEnd() const
// ConstIterator GetReverseBegin() const
// ConstIterator GetReverseEnd() const
TYPED_TEST(StringViewTests, IteratorGetters)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> string = this->GetCString();

    EXPECT_EQ(*string.GetBegin(), string.Front());
    EXPECT_EQ(string.GetEnd(), string.GetBegin() + string.Size());

    EXPECT_EQ(*string.GetReverseBegin(), string.Back());
    EXPECT_EQ(string.GetReverseEnd(), string.GetReverseBegin() + static_cast<Ptrdiff>(string.Size()));
}

// void RemovePrefix(Usize offset)
TYPED_TEST(StringViewTests, RemovePrefix)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();
    std::basic_string_view expectedString = this->GetCString();

    string.RemovePrefix(3);
    expectedString.remove_prefix(3);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void RemoveSuffix(Usize offset)
TYPED_TEST(StringViewTests, RemoveSuffix)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();
    std::basic_string_view expectedString = this->GetCString();

    string.RemoveSuffix(3);
    expectedString.remove_suffix(3);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void Swap(BasicStringView<T>& stringView)
TYPED_TEST(StringViewTests, Swap)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();
    BasicStringView<T> string2 = this->GetShortCString();

    const T* rawString = string.Data();
    const T* rawString2 = string2.Data();

    Usize size = string.Size();
    Usize size2 = string2.Size();

    string.Swap(string2);

    EXPECT_EQ(string.Data(), rawString2);
    EXPECT_EQ(string2.Data(), rawString);

    EXPECT_EQ(string.Size(), size2);
    EXPECT_EQ(string2.Size(), size);
}

// bool StartsWith(BasicStringView<T> stringView)
// bool StartsWith(T character)
// bool StartsWith(const T* string)
TYPED_TEST(StringViewTests, StartsWith)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();

    std::basic_string<T> expectedString = string.Data();
    BasicStringView<T> matchingSubstring(expectedString.data(), expectedString.data() + 3);
    BasicStringView<T> differingSubstring(expectedString.data() + 2, expectedString.data() + 3);

    EXPECT_TRUE(string.StartsWith(matchingSubstring));
    EXPECT_FALSE(string.StartsWith(differingSubstring));

    EXPECT_TRUE(string.StartsWith(expectedString[0]));
    EXPECT_FALSE(string.StartsWith(static_cast<T>('@')));

    EXPECT_TRUE(string.StartsWith(expectedString.substr(0, 4).data()));
    EXPECT_FALSE(string.StartsWith(expectedString.substr(2, 4).data()));
}

// bool EndsWith(BasicStringView<T> stringView)
// bool EndsWith(T character)
// bool EndsWith(const T* string)
TYPED_TEST(StringViewTests, EndsWith)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();

    std::basic_string<T> expectedString = string.Data();
    BasicStringView<T> matchingSubstring(expectedString.data() + 3, expectedString.data() + expectedString.size());
    BasicStringView<T> differingSubstring(expectedString.data() + 2, expectedString.data() + 3);

    EXPECT_TRUE(string.EndsWith(matchingSubstring));
    EXPECT_FALSE(string.EndsWith(differingSubstring));

    EXPECT_TRUE(string.EndsWith(expectedString.back()));
    EXPECT_FALSE(string.EndsWith(static_cast<T>('@')));

    EXPECT_TRUE(string.EndsWith(expectedString.substr(expectedString.size() - 3, 3).data()));
    EXPECT_FALSE(string.EndsWith(expectedString.substr(2, 4).data()));
}

// bool Contains(BasicStringView<T> stringView)
// bool Contains(T character)
// bool Contains(const T* string)
TYPED_TEST(StringViewTests, Contains)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();

    std::basic_string<T> expectedString = string.Data();
    BasicStringView<T> matchingSubstring(expectedString.data() + 3, expectedString.data() + expectedString.size());
    BasicStringView<T> matchingSubstring2(expectedString.data() + 2, expectedString.data() + 3);
    BasicStringView<T> differingSubstring(this->GetShortCString());

    EXPECT_TRUE(string.Contains(matchingSubstring));
    EXPECT_TRUE(string.Contains(matchingSubstring2));
    EXPECT_FALSE(string.Contains(differingSubstring));

    EXPECT_TRUE(string.Contains(expectedString.back()));
    EXPECT_FALSE(string.Contains(static_cast<T>('@')));

    EXPECT_TRUE(string.Contains(expectedString.substr(expectedString.size() - 3, 3).data()));
    EXPECT_TRUE(string.Contains(expectedString.substr(2, 4).data()));
    EXPECT_FALSE(string.Contains(differingSubstring.Data()));
}

// ConstIterator Find(BasicStringView<T> string) const
TYPED_TEST(StringViewTests, FindStringView)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(BasicStringView<T>(this->GetFoundString())), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(BasicStringView<T>(this->GetNotFoundString())), string.GetEnd());
}

// ConstIterator Find(const T* string) const
TYPED_TEST(StringViewTests, FindCString)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(this->GetFoundString()), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(this->GetNotFoundString()), string.GetEnd());
}

// ConstIterator Find(T character) const
TYPED_TEST(StringViewTests, FindCharacter)
{
    using T = typename TestFixture::CharType;
    BasicStringView<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(static_cast<T>('M')), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(static_cast<T>('x')), string.GetEnd());
}

// BasicStringView<T> Substring(Index startPos, Usize count)
TYPED_TEST(StringViewTests, Substring)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();
    BasicStringView<T> substring = string.Substring(3, 5);

    std::basic_string<T> stdSubstring = this->GetCString();
    stdSubstring = stdSubstring.substr(3, 5);

    EXPECT_TRUE(std::equal(stdSubstring.begin(), stdSubstring.end(), substring.GetBegin()));
    EXPECT_EQ(substring.Size(), stdSubstring.size());
}

// bool operator==(const BasicString<T, Alloc1>& string1, const BasicString<T, Alloc2>& string2)
// bool operator==(const BasicString<T, Alloc>& string1, const T* string2)
// bool operator==(const T* string1, const BasicString<T, Alloc>& string2)
TYPED_TEST(StringViewTests, EqualOperator)
{
    using T = typename TestFixture::CharType;

    BasicStringView<T> string = this->GetCString();
    BasicStringView<T> sameString = this->GetCString();
    BasicStringView<T> differentString = this->GetShortCString();

    EXPECT_EQ(string, sameString);
    EXPECT_NE(string, differentString);

    EXPECT_EQ(string, sameString.Data());
    EXPECT_NE(string, differentString.Data());

    EXPECT_EQ(string.Data(), sameString);
    EXPECT_NE(string.Data(), differentString);
}
