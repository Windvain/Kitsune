#include <gtest/gtest.h>
#include "Foundation/String/StringView.h"

namespace
{
    using namespace Kitsune;

    template<typename T>
    class StringViewTest : public ::testing::Test
    {
    protected:
        using CharType = T;

        [[nodiscard]]
        inline std::basic_string<T> GetEncodedString(
            const char* string,
            std::size_t size)
        {
            std::basic_string<T> convString;
            for (std::size_t index = 0; index < size; ++index)
            {
                assert(string[index] <= 127);
                convString.push_back(static_cast<T>(string[index]));
            }

            return convString;
        }

        [[nodiscard]]
        inline std::basic_string<T> GetEncodedString(const char* string)
        {
            return GetEncodedString(string, std::strlen(string));
        }
    };

    using StringViewTestTypes =
        ::testing::Types<char, wchar_t, char8_t, char16_t, char32_t>;

    TYPED_TEST_SUITE(StringViewTest, StringViewTestTypes);

    // BasicStringView<T>()
    TYPED_TEST(StringViewTest, DefaultConstructor)
    {
        using T = typename TestFixture::CharType;

        BasicStringView<T> stringView{};
        EXPECT_EQ(stringView.Data(), nullptr);
        EXPECT_EQ(stringView.Size(), 0);
    }

    // BasicStringView<T>(const T*, Usize)
    TYPED_TEST(StringViewTest, CstringSizeConstructor)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> string = this->GetEncodedString("Hello, World!");

        BasicStringView<T> stringView(string.data() + 2, string.data() + 5);
        EXPECT_EQ(stringView.Data(), string.data() + 2);
        EXPECT_EQ(stringView.Size(), 3);
    }

    // BasicStringView<T>(const T*)
    TYPED_TEST(StringViewTest, CstringConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView(string.data());

        EXPECT_EQ(stringView.Data(), string.data());
        EXPECT_EQ(stringView.Size(), string.size());
    }

    // BasicStringView<T>(Iter, Iter)
    TYPED_TEST(StringViewTest, RangeConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView(string.data(), string.data() + string.size());

        EXPECT_EQ(stringView.Data(), string.data());
        EXPECT_EQ(stringView.Size(), string.size());
    }

    // BasicStringView<T>::operator[](Index) const
    TYPED_TEST(StringViewTest, SubscriptOperator)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView(string.data());

        for (Index index = 0; index < stringView.Size(); ++index)
            EXPECT_EQ(&stringView[index], stringView.Data() + index);
    }

    // BasicStringView<T>::Front() const
    // BasicStringView<T>::Back() const
    TYPED_TEST(StringViewTest, FrontAndBack)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView(string.data());

        EXPECT_EQ(&stringView.Front(), string.data());
        EXPECT_EQ(&stringView.Back(), string.data() + string.size() - 1);

        BasicStringView <T> empty;
        EXPECT_THROW((void)(empty.Front()), OutOfRangeException);
        EXPECT_THROW((void)(empty.Back()), OutOfRangeException);
    }

    // BasicStringView<T>::GetBegin()
    // BasicStringView<T>::GetEnd()
    // BasicStringView<T>::GetReverseBegin()
    // BasicStringView<T>::GetReverseEnd()
    TYPED_TEST(StringViewTest, IteratorGetters)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView(string.data());

        EXPECT_EQ(ToAddress(stringView.GetBegin()), &stringView.Front());
        EXPECT_EQ(stringView.GetEnd(), stringView.GetBegin() + stringView.Size());

        EXPECT_EQ(&*stringView.GetReverseBegin(), &stringView.Back());
        EXPECT_EQ(
            stringView.GetReverseEnd(),
            stringView.GetReverseBegin() + static_cast<Ptrdiff>(stringView.Size()));
    }

    // BasicStringView<T>::RemovePrefix(Usize)
    TYPED_TEST(StringViewTest, RemovePrefix)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> string = this->GetEncodedString("Hello, World!");

        BasicStringView<T> stringView = string.data();
        std::basic_string_view expectedString = string.data();

        stringView.RemovePrefix(3);
        expectedString.remove_prefix(3);

        EXPECT_EQ(stringView.Size(), expectedString.size());

        for (int index = 0; index < expectedString.size(); ++index)
            EXPECT_EQ(stringView.Data()[index], expectedString[index]);
    }

    // BasicStringView<T>::RemoveSuffix(Usize)
    TYPED_TEST(StringViewTest, RemoveSuffix)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> string = this->GetEncodedString("Hello, World!");

        BasicStringView<T> stringView = string.data();
        std::basic_string_view expectedString = string.data();

        stringView.RemoveSuffix(3);
        expectedString.remove_suffix(3);

        EXPECT_EQ(stringView.Size(), expectedString.size());
        for (int index = 0; index < expectedString.size(); ++index)
            EXPECT_EQ(stringView.Data()[index], expectedString[index]);
    }

    // BasicStringView<T>::Swap(BasicStringView<T>&)
    TYPED_TEST(StringViewTest, Swap)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        std::basic_string<T> string2 = this->GetEncodedString("Not Hello, World!");

        BasicStringView<T> sv = string.data();
        BasicStringView<T> sv2 = string2.data();

        const T* rawString = sv.Data();
        const T* rawString2 = sv2.Data();

        Usize size = sv.Size();
        Usize size2 = sv2.Size();

        sv.Swap(sv2);

        EXPECT_EQ(sv.Data(), rawString2);
        EXPECT_EQ(sv2.Data(), rawString);

        EXPECT_EQ(sv.Size(), size2);
        EXPECT_EQ(sv2.Size(), size);
    }

    // BasicStringView<T>::StartsWith(BasicStringView<T>)
    // BasicStringView<T>::StartsWith(T)
    // BasicStringView<T>::StartsWith(const T*)
    TYPED_TEST(StringViewTest, StartsWith)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView = string.data();

        std::basic_string<T> expected = stringView.Data();
        BasicStringView<T> sameSubstring(expected.data(), expected.data() + 3);
        BasicStringView<T> diffSubstring(expected.data() + 2, expected.data() + 3);

        EXPECT_TRUE(stringView.StartsWith(sameSubstring));
        EXPECT_FALSE(stringView.StartsWith(diffSubstring));

        EXPECT_TRUE(stringView.StartsWith(expected[0]));
        EXPECT_FALSE(stringView.StartsWith(static_cast<T>('@')));

        EXPECT_TRUE(stringView.StartsWith(expected.substr(0, 4).data()));
        EXPECT_FALSE(stringView.StartsWith(expected.substr(2, 4).data()));
    }

    // BasicStringView<T>::EndsWith(BasicStringView<T> stringView)
    // BasicStringView<T>::EndsWith(T character)
    // BasicStringView<T>::EndsWith(const T* string)
    TYPED_TEST(StringViewTest, EndsWith)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        BasicStringView<T> stringView = string.data();

        std::basic_string<T> expected = stringView.Data();
        BasicStringView<T> matching(
            expected.data() + 3,
            expected.data() + expected.size());

        BasicStringView<T> diff(expected.data() + 2, expected.data() + 3);

        EXPECT_TRUE(stringView.EndsWith(matching));
        EXPECT_FALSE(stringView.EndsWith(diff));

        EXPECT_TRUE(stringView.EndsWith(expected.back()));
        EXPECT_FALSE(stringView.EndsWith(static_cast<T>('@')));

        EXPECT_TRUE(stringView.EndsWith(expected.substr(expected.size() - 3, 3).data()));
        EXPECT_FALSE(stringView.EndsWith(expected.substr(2, 4).data()));
    }

    // BasicStringView<T>::Contains(BasicStringView<T> stringView)
    // BasicStringView<T>::Contains(T character)
    // BasicStringView<T>::Contains(const T* string)
    TYPED_TEST(StringViewTest, Contains)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        std::basic_string<T> diffString = this->GetEncodedString("Nope.");

        BasicStringView<T> stringView = string.data();

        std::basic_string<T> expected = stringView.Data();
        BasicStringView<T> matching(
            expected.data() + 3,
            expected.data() + expected.size());

        BasicStringView<T> matching2(
            expected.data() + 2,
            expected.data() + 3);

        BasicStringView<T> diff = diffString.data();

        EXPECT_TRUE(stringView.Contains(matching));
        EXPECT_TRUE(stringView.Contains(matching2));
        EXPECT_FALSE(stringView.Contains(diff));

        EXPECT_TRUE(stringView.Contains(expected.back()));
        EXPECT_FALSE(stringView.Contains(static_cast<T>('@')));

        EXPECT_TRUE(stringView.Contains(expected.substr(expected.size() - 3, 3).data()));
        EXPECT_TRUE(stringView.Contains(expected.substr(2, 4).data()));
        EXPECT_FALSE(stringView.Contains(diff.Data()));
    }

    // BasicStringView<T>::Find(BasicStringView<T> string) const
    TYPED_TEST(StringViewTest, FindStringView)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        std::basic_string<T> falseString = this->GetEncodedString("Hello.");
        std::basic_string<T> trueString = this->GetEncodedString("o, Wo");

        BasicStringView<T> stringView = string.data();
        BasicStringView<T> falseSubstr = falseString.data();
        BasicStringView<T> trueSubstr = trueString.data();

        EXPECT_EQ(stringView.Find(trueSubstr), stringView.GetBegin() + 4);
        EXPECT_EQ(stringView.Find(falseSubstr), stringView.GetEnd());
    }

    // BasicStringView<T>::Find(const T* string) const
    TYPED_TEST(StringViewTest, FindCString)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        std::basic_string<T> falseString = this->GetEncodedString("Hello.");
        std::basic_string<T> trueString = this->GetEncodedString("o, Wo");

        BasicStringView<T> stringView = string.data();

        EXPECT_EQ(stringView.Find(trueString.data()), stringView.GetBegin() + 4);
        EXPECT_EQ(stringView.Find(falseString.data()), stringView.GetEnd());
    }

    // BasicStringView<T>::Find(T character) const
    TYPED_TEST(StringViewTest, FindCharacter)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        T found = static_cast<T>(' ');
        T notFound = static_cast<T>('h');

        BasicStringView<T> stringView = string.data();

        EXPECT_EQ(stringView.Find(found), stringView.GetBegin() + 6);
        EXPECT_EQ(stringView.Find(notFound), stringView.GetEnd());
    }

    // BasicStringView<T>::Substring(Index startPos, Usize count)
    TYPED_TEST(StringViewTest, Substring)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> string = this->GetEncodedString("Hello, World!");

        BasicStringView<T> stringView = string.data();
        BasicStringView<T> substring = stringView.Substring(3, 5);

        std::basic_string<T> stdSubstring = string.data();
        stdSubstring = stdSubstring.substr(3, 5);

        EXPECT_EQ(substring.Size(), stdSubstring.size());
        for (int index = 0; index < stdSubstring.size(); ++index)
            EXPECT_EQ(substring.Data()[index], stdSubstring[index]);
    }

    // operator==(const BasicStringView<T>&, const BasicStringView<T>&)
    // operator==(const BasicStringView<T>&, const T*)
    // operator==(const T*, const BasicStringView<T>&)
    TYPED_TEST(StringViewTest, EqualOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> string = this->GetEncodedString("Hello, World!");
        std::basic_string<T> diffString = this->GetEncodedString("Hello, Worrd!");

        BasicStringView<T> stringView = string.data();
        BasicStringView<T> sameStringView = string.data();
        BasicStringView<T> diffStringView = diffString.data();

        EXPECT_EQ(stringView, sameStringView);
        EXPECT_NE(stringView, diffStringView);

        EXPECT_EQ(stringView, sameStringView.Data());
        EXPECT_NE(stringView, diffStringView.Data());

        EXPECT_EQ(stringView.Data(), sameStringView);
        EXPECT_NE(stringView.Data(), diffStringView);
    }
}
