#include <gtest/gtest.h>
#include "TestContainer.h"

#include "StringEqualAssert.h"

#include "StatefulAllocator.h"
#include "TrackingAllocator.h"

#include "Foundation/String/String.h"
#include "Foundation/Concepts/Container.h"

namespace
{
    using namespace Kitsune;
    using Testing::StatefulAllocator, Testing::TrackingAllocator,
          Testing::ForwardNonOwningTestContainer;

    template<typename T>
    class StringTest : public ::testing::Test
    {
    protected:
        using CharType = T;
        using StringType = BasicString<T>;

        static_assert(
            Container<BasicString<T>>,
            "BasicString<T> does not satisfy the requirements of the Container "
            "concept.");

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

    using StringTestTypes =
        ::testing::Types<char, wchar_t, char8_t, char16_t, char32_t>;

    TYPED_TEST_SUITE(StringTest, StringTestTypes);

    // BasicString<T, Alloc>::BasicString()
    TYPED_TEST(StringTest, DefaultConstructor)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, StatefulAllocator> string;

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Data()[0], T());

        EXPECT_EQ(string.GetAllocator().GetId(), 0);
        EXPECT_TRUE(string.IsStorageLocal());
    }

    // BasicString<T, Alloc>::BasicString(const Alloc&)
    TYPED_TEST(StringTest, AllocatorConstructor)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, StatefulAllocator> string{ StatefulAllocator(72) };

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Data()[0], T());

        EXPECT_EQ(string.GetAllocator().GetId(), 72);
        EXPECT_TRUE(string.IsStorageLocal());
    }

    // BasicString<T, Alloc>::BasicString(Usize, const Alloc&)
    TYPED_TEST(StringTest, OptimizedCapacityConstructor)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, StatefulAllocator> string(
            BasicString<T>::GetLocalCapacity(),
            StatefulAllocator(642));

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Data()[0], T());

        EXPECT_EQ(string.GetAllocator().GetId(), 642);

        EXPECT_EQ(string.Capacity(), string.GetLocalCapacity());
        EXPECT_TRUE(string.IsStorageLocal());
    }

    // BasicString<T, Alloc>::BasicString(Usize, const Alloc&)
    TYPED_TEST(StringTest, OptimizedCapacityConstructorDoesNotAllocate)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, TrackingAllocator> string(BasicString<T>::GetLocalCapacity());

        ASSERT_TRUE(string.IsStorageLocal());
        EXPECT_EQ(string.GetAllocator().AllocationCount(), 0);
    }

    // BasicString<T, Alloc>::BasicString(Usize, const Alloc&)
    TYPED_TEST(StringTest, LargeCapacityConstructor)
    {
        using T = typename TestFixture::CharType;

        BasicString<T, StatefulAllocator> string(100, StatefulAllocator(2));
        ASSERT_GT(100, BasicString<T>().GetLocalCapacity());

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Data()[0], T());

        EXPECT_EQ(string.GetAllocator().GetId(), 2);

        EXPECT_GE(string.Capacity(), 100);
        EXPECT_FALSE(string.IsStorageLocal());
    }

    // BasicString<T, Alloc>::BasicString(Usize, const Alloc&)
    TYPED_TEST(StringTest, LargeCapacityConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, TrackingAllocator> string(100);

        ASSERT_GT(100, BasicString<T>().GetLocalCapacity());
        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(Usize, T, const Alloc&)
    TYPED_TEST(StringTest, FillConstructor)
    {
        using T = typename TestFixture::CharType;

        const auto fillChar = static_cast<T>('f');
        BasicString<T, StatefulAllocator> string(
            12,
            fillChar,
            StatefulAllocator(6512));

        EXPECT_EQ(string.Size(), 12);
        EXPECT_GE(string.Capacity(), 12);
        EXPECT_EQ(string.GetAllocator().GetId(), 6512);

        EXPECT_EQ(string.Data()[12], T());

        for (Index index = 0; index < 11; ++index)
            EXPECT_EQ(string.Data()[index], fillChar);
    }

    // BasicString<T, Alloc>::BasicString(Usize, T, const Alloc&)
    TYPED_TEST(StringTest, FillConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;

        const auto fillChar = static_cast<T>('f');
        BasicString<T, TrackingAllocator> string(17, fillChar);

        ASSERT_GT(17, BasicString<T>().GetLocalCapacity());

        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(const T*, Usize, const Alloc&)
    TYPED_TEST(StringTest, CstringSizeConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!mmmmm");
        BasicString<T, StatefulAllocator> string(
            source.c_str(), 13, StatefulAllocator(124));

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), 13);
        EXPECT_EQ(string.GetAllocator().GetId(), 124);

        EXPECT_EQ(string.Data()[13], T());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Data(), expected.data());
    }

    // BasicString<T, Alloc>::BasicString(const T*, Usize, const Alloc&)
    TYPED_TEST(StringTest, CstringSizeConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string.mmmmm");

        BasicString<T, TrackingAllocator> string(source.c_str(), 33);
        ASSERT_GT(source.size(), BasicString<T>().GetLocalCapacity());

        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(const T*, const Alloc&)
    TYPED_TEST(StringTest, CstringConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(124));

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), 13);
        EXPECT_EQ(string.GetAllocator().GetId(), 124);

        EXPECT_EQ(string.Data()[13], T());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Data(), expected.data());
    }

    // BasicString<T, Alloc>::BasicString(const T*, const Alloc&)
    TYPED_TEST(StringTest, CstringConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string.mmmmm");

        ASSERT_GT(source.size(), BasicString<T>().GetLocalCapacity());

        BasicString<T, TrackingAllocator> string(source.c_str());

        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(Iter, Iter, const Alloc&)
    TYPED_TEST(StringTest, RangeConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        ForwardNonOwningTestContainer<T, 13> container(source.data());

        BasicString<T, StatefulAllocator> string(
            container.GetBegin(), container.GetEnd(),
            StatefulAllocator(65));

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), 13);
        EXPECT_EQ(string.GetAllocator().GetId(), 65);

        EXPECT_EQ(string.Data()[13], T());

        for (int index = 0; index < 13; ++index)
            EXPECT_EQ(string.Data()[index], source[index]);
    }

    // BasicString<T, Alloc>::BasicString(Iter, Iter, const Alloc&)
    TYPED_TEST(StringTest, RangeConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!mmmmmmmm");
        ForwardNonOwningTestContainer<T, 21> container(source.data());

        ASSERT_GT(source.size(), BasicString<T>().GetLocalCapacity());

        BasicString<T, TrackingAllocator> string(
            container.GetBegin(), container.GetEnd());

        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, OptimizedStringCopyConstructor)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hi");
        BasicString<T, StatefulAllocator> string(source.data(), StatefulAllocator(102));

        ASSERT_TRUE(string.IsStorageLocal());

        BasicString<T, StatefulAllocator> copy = string;
        EXPECT_EQ(copy.Size(), string.Size());
        EXPECT_EQ(copy.Size(), 2);

        EXPECT_GE(copy.Capacity(), copy.Size());
        EXPECT_EQ(copy.Capacity(), copy.GetLocalCapacity());

        EXPECT_TRUE(copy.IsStorageLocal());
        EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
        EXPECT_EQ(copy.GetAllocator().GetId(), 102);

        std::basic_string<T> expected = this->GetEncodedString("Hi");
        EXPECT_GENERAL_STREQ(string.Data(), expected.data());
        EXPECT_GENERAL_STREQ(copy.Data(), expected.data());
    }

    // BasicString<T, Alloc>::BasicString(const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, LargeStringCopyConstructor)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string.");

        ASSERT_GT(source.size(), BasicString<T>().GetLocalCapacity());

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(542));
        BasicString<T, StatefulAllocator> copy = string;

        EXPECT_EQ(copy.Size(), string.Size());
        EXPECT_EQ(copy.Size(), source.size());

        EXPECT_GE(copy.Capacity(), copy.Size());
        EXPECT_FALSE(copy.IsStorageLocal());

        EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
        EXPECT_EQ(copy.GetAllocator().GetId(), 542);

        EXPECT_GENERAL_STREQ(string.Data(), source.data());
        EXPECT_GENERAL_STREQ(copy.Data(), source.data());
    }

    // BasicString<T, Alloc>::BasicString(const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, CopyConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World! Weeeee!");

        BasicString<T, TrackingAllocator> string(source.c_str());
        BasicString<T, TrackingAllocator> copy = string;

        EXPECT_EQ(
            copy.GetAllocator().AllocationSize(copy.Data()),
            (copy.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(BasicString<T, Alloc>&&)
    TYPED_TEST(StringTest, OptimizedStringMoveConstructor)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hi");

        ASSERT_LE(source.size(), BasicString<T>().GetLocalCapacity());

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        BasicString<T, StatefulAllocator> move(std::move(string));

        EXPECT_EQ(move.Size(), source.size());
        EXPECT_EQ(string.Size(), 0);

        EXPECT_EQ(move.Capacity(), move.GetLocalCapacity());
        EXPECT_TRUE(move.IsStorageLocal());

        EXPECT_EQ(move.GetAllocator().GetId(), 17);
        EXPECT_EQ(string.GetAllocator().GetId(), 0);

        EXPECT_EQ(move.Data()[move.Size()], T());
        EXPECT_GENERAL_STREQ(move.Data(), source.data());
    }

    // BasicString<T, Alloc>::BasicString(BasicString<T, Alloc>&&)
    TYPED_TEST(StringTest, LargeStringMoveConstructor)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello I am a long string!");

        ASSERT_GT(source.size(), BasicString<T>().GetLocalCapacity());

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        BasicString<T, StatefulAllocator> move(std::move(string));

        EXPECT_EQ(move.Size(), source.size());
        EXPECT_EQ(string.Size(), 0);

        EXPECT_GE(move.Capacity(), move.Size());
        EXPECT_FALSE(move.IsStorageLocal());

        EXPECT_EQ(move.GetAllocator().GetId(), 17);
        EXPECT_EQ(string.GetAllocator().GetId(), 0);

        EXPECT_EQ(move.Data()[move.Size()], T());
        EXPECT_GENERAL_STREQ(move.Data(), source.data());
    }

    // BasicString<T, Alloc>::BasicString(std::initializer_list<T>, const Alloc&)
    TYPED_TEST(StringTest, InitializerListConstructor)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, StatefulAllocator> string(
            { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!' },
            StatefulAllocator(65));

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), 13);
        EXPECT_EQ(string.GetAllocator().GetId(), 65);

        EXPECT_EQ(string.Data()[13], T());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        for (int index = 0; index < 13; ++index)
            EXPECT_EQ(string.Data()[index], expected[index]);
    }

    // BasicString<T, Alloc>::BasicString(std::initializer_list<T>, const Alloc&)
    TYPED_TEST(StringTest, InitializerListConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;
        BasicString<T, TrackingAllocator> string({
            'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!',
            'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm'
        });

        ASSERT_FALSE(string.IsStorageLocal());
        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::BasicString(BasicStringView<T>, const Alloc&)
    TYPED_TEST(StringTest, StringViewConstructor)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicStringView<T> stringView(source.data(), 5);
        BasicString<T, StatefulAllocator> string(stringView, StatefulAllocator(65));

        EXPECT_EQ(string.Size(), 5);
        EXPECT_GE(string.Capacity(), 5);
        EXPECT_EQ(string.GetAllocator().GetId(), 65);

        EXPECT_EQ(string.Data()[5], T());

        std::basic_string<T> expected = this->GetEncodedString("Hello");
        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(string.Data()[index], expected[index]);
    }

    // BasicString<T, Alloc>::BasicString(BasicStringView<T>, const Alloc&)
    TYPED_TEST(StringTest, StringViewConstructorAllocatesMemory)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string!");

        BasicStringView<T> stringView(source.data(), 16);
        BasicString<T, TrackingAllocator> string(stringView);

        ASSERT_FALSE(string.IsStorageLocal());
        EXPECT_EQ(
            string.GetAllocator().AllocationSize(string.Data()),
            (string.Capacity() + 1) * sizeof(T));
    }

    // BasicString<T, Alloc>::operator=(const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, CopyAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("Cow");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(542));
        BasicString<T, StatefulAllocator> copy(source2.c_str(), StatefulAllocator(32));

        copy = string;

        EXPECT_EQ(copy.Size(), string.Size());
        EXPECT_EQ(copy.Size(), source.size());
        EXPECT_GE(copy.Capacity(), copy.Size());

        EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
        EXPECT_EQ(copy.GetAllocator().GetId(), 542);

        EXPECT_GENERAL_STREQ(string.Data(), source.data());
        EXPECT_GENERAL_STREQ(copy.Data(), source.data());
    }

    // BasicString<T, Alloc>::operator=(BasicString<T, Alloc>&&)
    TYPED_TEST(StringTest, MoveAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("Rad!");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        BasicString<T, StatefulAllocator> move(source2.c_str(), StatefulAllocator(123));

        move = std::move(string);

        EXPECT_EQ(move.Size(), source.size());
        EXPECT_EQ(string.Size(), 0);
        EXPECT_GE(move.Capacity(), move.Size());

        EXPECT_EQ(move.GetAllocator().GetId(), 17);
        EXPECT_EQ(string.GetAllocator().GetId(), 0);

        EXPECT_EQ(move.Data()[move.Size()], T());
        EXPECT_GENERAL_STREQ(move.Data(), source.data());
    }

    // BasicString<T, Alloc>::operator=(const T*)
    TYPED_TEST(StringTest, CstringAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("Rad!");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        string = source2.c_str();

        EXPECT_EQ(string.Size(), source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 17);

        EXPECT_EQ(string.Data()[string.Size()], T());
        EXPECT_GENERAL_STREQ(string.Data(), source2.data());
    }

    // BasicString<T, Alloc>::operator=(T)
    TYPED_TEST(StringTest, CharacterAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("R");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        string = source2.front();

        EXPECT_EQ(string.Size(), source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 17);

        EXPECT_EQ(string.Data()[0], source2.front());
        EXPECT_EQ(string.Data()[1], T());
    }

    // BasicString<T, Alloc>::operator=(std::initializer_list<T>)
    TYPED_TEST(StringTest, InitializerListAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Weeee!");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        string = {
            'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!',
            'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm', 'm'
        };

        EXPECT_EQ(string.Size(), 22);
        EXPECT_GE(string.Capacity(), 22);

        EXPECT_EQ(string.GetAllocator().GetId(), 17);

        EXPECT_EQ(string.Data()[string.Size()], T());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!mmmmmmmmm");
        EXPECT_GENERAL_STREQ(string.Data(), expected.data());
    }

    // BasicString<T, Alloc>::operator=(BasicStringView<T>)
    TYPED_TEST(StringTest, StringViewAssign)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("Rad! Can't wait!");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));
        string = BasicStringView<T>(source2.c_str(), 4);

        EXPECT_EQ(string.Size(), 4);
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 17);

        EXPECT_EQ(string.Data()[string.Size()], T());

        std::basic_string<T> expected = this->GetEncodedString("Rad!");
        EXPECT_GENERAL_STREQ(string.Data(), expected.data());
    }

    // BasicString<T, Alloc>::operator[](Index)
    TYPED_TEST(StringTest, Subscript)
    {
        using T = typename TestFixture::CharType;

        BasicString<T> string;
        const BasicString<T> constString;

        for (Index i = 0; i < string.Size(); ++i)
        {
            EXPECT_EQ(string.Data() + i, &string[i]);
            EXPECT_EQ(constString.Data() + i, &constString[i]);
        }
    }

    // BasicString<T, Alloc>::operator BasicStringView<T>()
    TYPED_TEST(StringTest, BasicStringViewCastOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        BasicStringView<T> stringView(string);

        EXPECT_EQ(stringView.Data(), string.Raw());
        EXPECT_EQ(stringView.Size(), string.Size());
    }

    // BasicString<T, Alloc>::operator+=(const BasicString<T, Alloc>&)
    // BasicString<T, Alloc>::operator+(const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, StringAppendOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Far far away, ");
        std::basic_string<T> source2 = this->GetEncodedString(
            "behind the word mountains, ");

        std::basic_string<T> source3 = this->GetEncodedString(
            "there lived the blind texts.");

        BasicString<T> string = source.c_str();
        BasicString<T> appendedString = source2.c_str();
        BasicString<T> appendedString2 = source3.c_str();

        string += appendedString + appendedString2;

        EXPECT_EQ(string.Size(), source.size() + source2.size() + source3.size());
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expectedOutput = this->GetEncodedString(
            "Far far away, behind the word mountains, there lived the blind texts.");

        EXPECT_GENERAL_STREQ(string.Raw(), expectedOutput.c_str());
    }

    // BasicString<T, Alloc>::operator+=(T)
    // BasicString<T, Alloc>::operator+(T)
    TYPED_TEST(StringTest, CharacterAppendOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, Worl");

        BasicString<T> string = source.c_str();
        string += 'd';

        EXPECT_EQ(string.Size(), 12);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expectedOutput = this->GetEncodedString("Hello, World");
        EXPECT_GENERAL_STREQ(string.Raw(), expectedOutput.c_str());

        BasicString<T> result = string + '!';

        EXPECT_EQ(result.Size(), 13);
        EXPECT_GE(result.Capacity(), string.Size());

        std::basic_string<T> expectedOutput2 = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(result.Raw(), expectedOutput2.c_str());
    }

    // BasicString<T, Alloc>::operator+=(const T*)
    // BasicString<T, Alloc>::operator+(const T*)
    TYPED_TEST(StringTest, CstringAppendOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Far far away, ");
        std::basic_string<T> source2 = this->GetEncodedString(
            "behind the word mountains, ");

        std::basic_string<T> source3 = this->GetEncodedString(
            "there lived the blind texts.");

        BasicString<T> string = source.c_str();
        string += source2.c_str();

        EXPECT_EQ(string.Size(), source.size() + source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString(
            "Far far away, behind the word mountains, ");

        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());

        BasicString<T> result = string + source3.c_str();

        EXPECT_EQ(result.Size(), source.size() + source2.size() + source3.size());
        EXPECT_GE(result.Capacity(), string.Size());

        std::basic_string<T> expected2 = this->GetEncodedString(
            "Far far away, behind the word mountains, there lived the blind texts.");

        EXPECT_GENERAL_STREQ(result.Raw(), expected2.c_str());
    }

    // BasicString<T, Alloc>::operator+=(std::initializer_list<T>)
    TYPED_TEST(StringTest, InitializerListAppendOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Far far away, ");
        std::basic_string<T> source3 = this->GetEncodedString(
            "there lived the blind texts.");

        BasicString<T> string = source.c_str();
        string += {
            'b', 'e', 'h', 'i', 'n', 'd', ' ', 't', 'h', 'e', ' ', 'w', 'o', 'r', 'd',
            ' ', 'm', 'o', 'u', 'n', 't', 'a', 'i', 'n', 's', ',', ' '
        };

        EXPECT_EQ(string.Size(), source.size() + 27);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString(
            "Far far away, behind the word mountains, ");

        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());

        /* operator+(std::initializer_list<T>) doesn't exist. */
    }

    // BasicString<T, Alloc>::operator+=(const BasicStringView<T>)
    // BasicString<T, Alloc>::operator+(const BasicStringView<T>)
    TYPED_TEST(StringTest, StringViewAppendOperator)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Far far away, ");
        std::basic_string<T> source2 = this->GetEncodedString(
            "behind the word mountains, mmmmm");

        std::basic_string<T> source3 = this->GetEncodedString(
            "there lived the blind texts.ddd");

        BasicString<T> string = source.c_str();
        BasicStringView<T> appendedString(source2.c_str(), 27);
        BasicStringView<T> appendedString2(source3.c_str(), 28);

        string += appendedString;
        BasicString<T> result = string + appendedString2;

        EXPECT_EQ(string.Size(), source.size() + 27);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString(
            "Far far away, behind the word mountains, ");

        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());

        EXPECT_EQ(result.Size(), source.size() + 55);
        EXPECT_GE(result.Capacity(), result.Size());

        std::basic_string<T> expected2 = this->GetEncodedString(
            "Far far away, behind the word mountains, there lived the blind texts.");

        EXPECT_GENERAL_STREQ(result.Raw(), expected2.c_str());
    }

    // BasicString<T, Alloc>::Front()
    // BasicString<T, Alloc>::Back()
    TYPED_TEST(StringTest, FrontAndBackGetters)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        BasicString<T> string = source.c_str();

        EXPECT_EQ(&string.Front(), string.Data());
        EXPECT_EQ(&string.Back(), string.Data() + string.Size() - 1);
    }

    // BasicString<T, Alloc>::GetBegin()
    // BasicString<T, Alloc>::GetEnd()
    // BasicString<T, Alloc>::GetReverseBegin()
    // BasicString<T, Alloc>::GetReverseEnd()
    TYPED_TEST(StringTest, Iterators)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        const BasicString<T> constString = source.c_str();

        EXPECT_EQ(ToAddress(string.GetBegin()), string.Data());
        EXPECT_EQ(ToAddress(string.GetReverseBegin()), &string.Back());
        EXPECT_EQ(ToAddress(constString.GetBegin()), constString.Data());
        EXPECT_EQ(ToAddress(constString.GetReverseBegin()), &constString.Back());

        EXPECT_EQ(string.GetEnd(), string.GetBegin() + string.Size());
        EXPECT_EQ(string.GetReverseEnd(), string.GetReverseBegin() + string.Size());

        EXPECT_EQ(constString.GetEnd(), constString.GetBegin() + constString.Size());
        EXPECT_EQ(
            constString.GetReverseEnd(),
            constString.GetReverseBegin() + constString.Size());
    }

    /* BasicString<T, Alloc>::Raw(), BasicString<T, Alloc>::Data(),
     * BasicString<T, Alloc>::Size(), BasicString<T, Alloc>::Capacity(),
     * BasicString<T, Alloc>::GetLocalCapacity(), BasicString<T, Alloc>::IsStorageLocal(),
     * and BasicString<T, Alloc>::GetAllocator() are assumed to work. No tests.
     */

    // String<T, Alloc>::IsEmpty()
    TYPED_TEST(StringTest, IsEmpty)
    {
        using T = typename TestFixture::CharType;

        BasicString<T> array(3, static_cast<T>('f'));
        BasicString<T> empty(12);

        ASSERT_EQ(array.Size(), 3);
        ASSERT_EQ(empty.Size(), 0);

        EXPECT_FALSE(array.IsEmpty());
        EXPECT_TRUE(empty.IsEmpty());
    }

    // BasicString<T, Alloc>::Assign(Usize, T)
    TYPED_TEST(StringTest, AssignFill)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));

        string.Assign(20, static_cast<T>('f'));

        EXPECT_EQ(string.Size(), 20);
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 17);
        EXPECT_EQ(string.Data()[string.Size()], T());

        for (int index = 0; index < 20; ++index)
            EXPECT_EQ(string.Data()[index], static_cast<T>('f'));
    }

    // BasicString<T, Alloc>::Assign(Iter, Iter)
    TYPED_TEST(StringTest, AssignRange)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString("I am a long string!");

        ForwardNonOwningTestContainer<T, 19> container(source2.data());
        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(17));

        string.Assign(container.GetBegin(), container.GetEnd());

        EXPECT_EQ(string.Size(), source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 17);

        EXPECT_EQ(string.Data()[string.Size()], T());
        EXPECT_GENERAL_STREQ(string.Data(), source2.data());
    }

    // BasicString<T, Alloc>::Clear()
    TYPED_TEST(StringTest, Clear)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string!");

        BasicString<T> string = source.c_str();
        Usize capacity = string.Capacity();

        string.Clear();

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Capacity(), capacity);
        EXPECT_EQ(string.Data()[0], T());
    }

    // BasicString<T, Alloc>::Reset()
    TYPED_TEST(StringTest, Reset)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString(
            "Hello, World! I am a long string!");

        BasicString<T> string = source.c_str();
        string.Reset();

        EXPECT_EQ(string.Size(), 0);
        EXPECT_EQ(string.Capacity(), BasicString<T>::GetLocalCapacity());
        EXPECT_EQ(string.Data()[0], T());
    }

    // BasicString<T, Alloc>::Insert(Index, const T*)
    TYPED_TEST(StringTest, InsertCstring)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        std::basic_string<T> source2 = this->GetEncodedString("llo, W");

        BasicString<T> string = source.c_str();
        string.Insert(2, source2.c_str());

        EXPECT_EQ(string.Size(), source.size() + source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Insert(Index, const T*, Usize)
    TYPED_TEST(StringTest, InsertCstringSize)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        std::basic_string<T> source2 = this->GetEncodedString("llo, World!");

        BasicString<T> string = source.c_str();
        string.Insert(2, source2.c_str(), 6);

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Insert(Index, BasicStringView<T>)
    TYPED_TEST(StringTest, InsertStringView)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        std::basic_string<T> source2 = this->GetEncodedString("llo, World!");

        BasicString<T> string = source.c_str();
        string.Insert(2, BasicStringView<T>(source2.c_str(), 6));

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Insert(Index, const BasicString<T>&)
    TYPED_TEST(StringTest, InsertString)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        std::basic_string<T> source2 = this->GetEncodedString("llo, W");

        BasicString<T, StatefulAllocator> string(source.c_str(), StatefulAllocator(13));
        BasicString<T, StatefulAllocator> string2 = source2.c_str();

        string.Insert(2, string2);

        EXPECT_EQ(string.Size(), source.size() + source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        EXPECT_EQ(string.GetAllocator().GetId(), 13);

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Insert(Index, Iter, Iter)
    TYPED_TEST(StringTest, InsertRange)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        std::basic_string<T> source2 = this->GetEncodedString("llo, W");

        ForwardNonOwningTestContainer<T, 6> container(source2.data());
        BasicString<T> string(source.c_str(), StatefulAllocator(13));

        string.Insert(2, container.GetBegin(), container.GetEnd());

        EXPECT_EQ(string.Size(), source.size() + source2.size());
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Insert(Index, std::initializer_list<T>)
    TYPED_TEST(StringTest, InsertInitializerList)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Heorld!");
        BasicString<T> string(source.c_str(), StatefulAllocator(13));

        string.Insert(2, { 'l', 'l', 'o', ',', ' ', 'W' });

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Remove(Index)
    TYPED_TEST(StringTest, RemoveCharacter)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, mWorld!");

        BasicString<T> string = source.c_str();
        string.Remove(7);

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, Alloc>::Remove(Index, Usize)
    TYPED_TEST(StringTest, RemoveRange)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, mfaWorld!");

        BasicString<T> string = source.c_str();
        string.Remove(7, 3);

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(string.Raw(), expected.c_str());
    }

    // BasicString<T, ALloc>::PushBack(T)
    TYPED_TEST(StringTest, PushBack)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        string.PushBack(static_cast<T>('p'));

        EXPECT_EQ(string.Size(), source.size() + 1);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!p");
        EXPECT_GENERAL_STREQ(expected.c_str(), string.Data());
    }

    // BasicString<T, Alloc>::PopBack()
    TYPED_TEST(StringTest, PopBack)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!!");

        BasicString<T> string = source.c_str();
        string.PopBack();

        EXPECT_EQ(string.Size(), 13);
        EXPECT_GE(string.Capacity(), string.Size());

        std::basic_string<T> expected = this->GetEncodedString("Hello, World!");
        EXPECT_GENERAL_STREQ(expected.c_str(), string.Data());
    }

    /* BasicString<T, Alloc>::Append(Usize, T),
     * BasicString<T, Alloc>::Append(const T*, Usize), and
     * BasicString<T, Alloc>::Append(Iter, Iter) were tested previously in the
     * operator+= and operator+ tests. These will be skipped.
     */

    /* BasicString<T, Alloc>::StartsWith(), BasicString<T, Alloc>::EndsWith(),
     * BasicString<T, Alloc>::Contains(), and BasicString<T, Alloc>::Find() are
     * are helper functions to the equivalent functions in BasicStringView<T>. No tests.
     */

    // BasicStringView<T> Substring(Index, Usize)
    TYPED_TEST(StringTest, Substring)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        BasicStringView<T> substring = string.Substring(3, 5);

        EXPECT_EQ(substring.Size(), 5);

        std::basic_string<T> expected = this->GetEncodedString("lo, W");
        EXPECT_EQ(
            std::basic_string<T>(substring.Data(), substring.Size()),
            expected.c_str());
    }

    // BasicString<T, Alloc>::Swap(BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, Swap)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString(
            "Hello, World! I am a long string.");

        BasicString<T> string = source.c_str();
        BasicString<T> string2 = source2.c_str();

        string.Swap(string2);

        EXPECT_EQ(string.Size(), source2.size());
        EXPECT_GE(string.Capacity(), string.Capacity());
        EXPECT_GENERAL_STREQ(string.Raw(), source2.c_str());

        EXPECT_EQ(string2.Size(), source.size());
        EXPECT_GE(string2.Capacity(), string2.Capacity());
        EXPECT_GENERAL_STREQ(string2.Raw(), source.c_str());
    }

    // BasicString<T, Alloc>::Reserve(Usize)
    TYPED_TEST(StringTest, Reserve)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        ASSERT_GT(124, string.Capacity());

        string.Reserve(124);

        EXPECT_GE(string.Capacity(), 124);
        EXPECT_EQ(string.Size(), source.size());
        EXPECT_GENERAL_STREQ(string.Data(), source.c_str());

        // 83 < 124, Reserve() should do nothing.
        string.Reserve(83);

        EXPECT_GE(string.Capacity(), 83);
        EXPECT_EQ(string.Size(), source.size());
        EXPECT_GENERAL_STREQ(string.Data(), source.c_str());
    }

    // BasicString<T, Alloc>::ShrinkToFit()
    TYPED_TEST(StringTest, ShrinkToFit)
    {
        using T = typename TestFixture::CharType;
        std::basic_string<T> source = this->GetEncodedString("Hello, World!");

        BasicString<T> string = source.c_str();
        ASSERT_GT(string.Capacity(), string.Size());

        string.ShrinkToFit();

        EXPECT_EQ(
            string.Capacity(),
            std::max(string.Size(), BasicString<T>::GetLocalCapacity()));

        EXPECT_EQ(string.Size(), source.size());
        EXPECT_GENERAL_STREQ(string.Data(), source.c_str());

        // The string's size is equal to its capacity, ShrinkToFit() should do nothing.
        string.ShrinkToFit();

        EXPECT_EQ(
            string.Capacity(),
            std::max(string.Size(), BasicString<T>::GetLocalCapacity()));

        EXPECT_EQ(string.Size(), source.size());
        EXPECT_GENERAL_STREQ(string.Data(), source.c_str());
    }

    // BasicString<T, Alloc>::begin()
    // BasicString<T, Alloc>::end()
    TYPED_TEST(StringTest, RangedForLoop)
    {
        using T = typename TestFixture::CharType;
        BasicString<T> string(5, static_cast<T>('f'));

        int index = 0;
        for (const T& element : string)
        {
            EXPECT_EQ(&element, string.Raw() + index);
            ++index;
        }
    }

    // operator==(const BasicString<T, Alloc1>&, const BasicString<T, Alloc2>&)
    // operator==(const BasicString<T, Alloc>&, const T*)
    // operator==(const T*, const BasicString<T, Alloc>&)
    TYPED_TEST(StringTest, EqualOperator)
    {
        using T = typename TestFixture::CharType;

        std::basic_string<T> source = this->GetEncodedString("Hello, World!");
        std::basic_string<T> source2 = this->GetEncodedString(
            "Hello, World! I am a long string.");

        BasicString<T> string = source.c_str();
        BasicString<T> sameString = source.c_str();
        BasicString<T> differentString = source2.c_str();

        EXPECT_EQ(string, sameString);
        EXPECT_NE(string, differentString);

        EXPECT_EQ(string, sameString.Data());
        EXPECT_NE(string, differentString.Data());

        EXPECT_EQ(string.Data(), sameString);
        EXPECT_NE(string.Data(), differentString);
    }
}
