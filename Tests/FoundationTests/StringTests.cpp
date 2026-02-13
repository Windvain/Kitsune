#include <gtest/gtest.h>
#include "Foundation/String/String.h"

using namespace Kitsune;

namespace
{
    // Basically a GlobalAllocator with an integer for identification.
    class IdentifiableAllocator
    {
    public:
        IdentifiableAllocator()
            : m_Id(0)
        {
        }

        IdentifiableAllocator(Uint64 id)
            : m_Id(id)
        {
        }

        IdentifiableAllocator(const IdentifiableAllocator& allocator)
            : m_Id(allocator.m_Id)
        {
        }

        IdentifiableAllocator(IdentifiableAllocator&& allocator)
            : m_Id(std::exchange(allocator.m_Id, 0))
        {
        }

    public:
        IdentifiableAllocator& operator=(const IdentifiableAllocator& allocator)
        {
            m_Id = allocator.m_Id;
            return *this;
        }

        IdentifiableAllocator& operator=(IdentifiableAllocator&& allocator)
        {
            m_Id = std::exchange(allocator.m_Id, 0);
            return *this;
        }

    public:
        void* Allocate(Usize bytes)
        {
            return m_Allocator.Allocate(bytes);
        }

        void* Allocate(Usize bytes, Usize alignment)
        {
            return m_Allocator.Allocate(bytes, alignment);
        }

        void Free(void* pointer, Usize bytes)
        {
            m_Allocator.Free(pointer, bytes);
        }

    public:
        bool operator==(const IdentifiableAllocator& allocator) const
        {
            return m_Id == allocator.m_Id;
        }

    private:
        Uint64 m_Id;
        GlobalAllocator m_Allocator;
    };

    static_assert(Allocator<IdentifiableAllocator>,
                  "IdentifiableAllocator does not meet the requirements of "
                  "Allocator<T>.");

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
class StringTests : public ::testing::Test
{
public:
    using CharType = T;

protected:
    StringTests() { /* ... */ }
    ~StringTests() { /* ... */ }

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

using StringTestsImpl =
    ::testing::Types<
        char,
        wchar_t,
        char8_t,
        char16_t,
        char32_t>;

TYPED_TEST_SUITE(StringTests, StringTestsImpl);

// BasicString<T, Alloc>()
TYPED_TEST(StringTests, DefaultConstructor)
{
    using T = typename TestFixture::CharType;
    BasicString<T, IdentifiableAllocator> string;

    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());

    IdentifiableAllocator allocator{ /* ... */ };
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_TRUE(string.IsStorageLocal());
}

// BasicString<T, Alloc>(const Alloc& allocator)
TYPED_TEST(StringTests, AllocatorConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 72;
    BasicString<T, IdentifiableAllocator> string{IdentifiableAllocator(allocatorId)};

    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_TRUE(string.IsStorageLocal());
}

// BasicString<T, Alloc>(Usize capacity, const Alloc& allocator)
TYPED_TEST(StringTests, OptimizedCapacityConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 642;
    BasicString<T, IdentifiableAllocator> string(
        BasicString<T>::GetLocalCapacity(),
        IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Capacity(), string.GetLocalCapacity());
    EXPECT_TRUE(string.IsStorageLocal());
}

TYPED_TEST(StringTests, LargeCapacityConstructor)
{
    using T = typename TestFixture::CharType;

    const Usize capacity = 100;
    const Uint64 allocatorId = 2;

    BasicString<T, IdentifiableAllocator> string(
        capacity, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_GE(string.Capacity(), capacity);
    EXPECT_FALSE(string.IsStorageLocal());
}

// BasicString<T, Alloc>(Usize count, T character, const Alloc& allocator)
TYPED_TEST(StringTests, FillConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 6512;
    const Usize fillCount = 12;

    const auto fillChar = static_cast<typename TestFixture::CharType>('f');

    BasicString<T, IdentifiableAllocator> string(
        fillCount, fillChar, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), fillCount);
    EXPECT_GE(string.Capacity(), fillCount);

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    ASSERT_NE(fillCount, 0);
    EXPECT_EQ(string.Data()[fillCount], T());

    for (Index i = 0; i < fillCount - 1; ++i)
    {
        EXPECT_EQ(string.Data()[i], fillChar);
    }
}

// BasicString<T, Alloc>(const T* string, Usize size, const Alloc& allocator)
TYPED_TEST(StringTests, CstringSizeConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 124;
    const T* cstring = this->GetCString();
    const Usize stringSize = 5;

    ASSERT_LE(stringSize, std::char_traits<T>::length(cstring));
    BasicString<T, IdentifiableAllocator> string(
        cstring, stringSize, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), stringSize);
    EXPECT_GE(string.Capacity(), stringSize);

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Data()[stringSize], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        cstring, string.Data(), stringSize), 0);
}

// BasicString<T, Alloc>(const T* string, const Alloc& allocator)
TYPED_TEST(StringTests, CstringConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 85;
    const T* cstring = this->GetCString();

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    Usize length = std::char_traits<T>::length(cstring);

    EXPECT_EQ(string.Size(), length);
    EXPECT_GE(string.Capacity(), length);

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Data()[length], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        cstring, string.Data(), length), 0);
}

// BasicString<T, Alloc>(It begin, It end, const Alloc& allocator)
TYPED_TEST(StringTests, RangeConstructor)
{
    using T = typename TestFixture::CharType;
    const Uint64 allocatorId = 65;

    const T* cstring = this->GetCString();
    Usize length = std::char_traits<T>::length(cstring);

    MyForwardIterator<const T> beginIter(cstring);
    MyForwardIterator<const T> endIter(cstring + length);

    BasicString<T, IdentifiableAllocator> string(
        beginIter, endIter, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), length);
    EXPECT_GE(string.Capacity(), length);

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Data()[length], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        cstring, string.Data(), length), 0);
}

// BasicString<T, Alloc>(const BasicString<T, Alloc>& string)
TYPED_TEST(StringTests, OptimizedStringCopyConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 102;
    const T* cstring = this->GetShortCString();

    ASSERT_LE(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    BasicString<T, IdentifiableAllocator> copy(string);

    EXPECT_EQ(copy.Size(), string.Size());
    EXPECT_GE(copy.Capacity(), copy.Size());

    EXPECT_EQ(copy.Capacity(), copy.GetLocalCapacity());
    EXPECT_TRUE(copy.IsStorageLocal());

    EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
    EXPECT_EQ(std::char_traits<T>::compare(
        copy.Data(), string.Data(), copy.Size() + 1), 0);
}

TYPED_TEST(StringTests, LargeStringCopyConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 542;
    const T* cstring = this->GetCString();

    ASSERT_GT(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    BasicString<T, IdentifiableAllocator> copy(string);

    EXPECT_EQ(copy.Size(), string.Size());
    EXPECT_GE(copy.Capacity(), copy.Size());

    EXPECT_FALSE(copy.IsStorageLocal());

    EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
    EXPECT_EQ(std::char_traits<T>::compare(
        copy.Data(), string.Data(), copy.Size() + 1), 0);
}

// BasicString<T, Alloc>(BasicString<T, Alloc>&& string)
TYPED_TEST(StringTests, OptimizedStringMoveConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 17;
    const T* cstring = this->GetShortCString();

    ASSERT_LE(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    Usize size = string.Size();

    BasicString<T, IdentifiableAllocator> move(std::move(string));

    EXPECT_EQ(move.Size(), size);
    EXPECT_GE(move.Capacity(), size);

    EXPECT_EQ(move.Capacity(), move.GetLocalCapacity());
    EXPECT_TRUE(move.IsStorageLocal());

    EXPECT_EQ(move.GetAllocator(), IdentifiableAllocator(allocatorId));
    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator());

    EXPECT_EQ(move.Data()[move.Size()], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        move.Data(), cstring, move.Size()), 0);
}

TYPED_TEST(StringTests, LargeStringMoveConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 17;
    const T* cstring = this->GetCString();

    ASSERT_GT(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    Usize size = string.Size();
    BasicString<T, IdentifiableAllocator> move(std::move(string));

    EXPECT_EQ(move.Size(), size);
    EXPECT_GE(move.Capacity(), size);

    EXPECT_FALSE(move.IsStorageLocal());

    EXPECT_EQ(move.GetAllocator(), IdentifiableAllocator(allocatorId));
    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator());

    EXPECT_EQ(move.Data()[move.Size()], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        move.Data(), cstring, move.Size()), 0);
}

// BasicString<T, Alloc>(std::initializer_list<T> initList, const Alloc& allocator)
TYPED_TEST(StringTests, InitListConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 65;
    std::initializer_list<T> initList{1, 2, 3, 4, 65, 122, 44 };

    BasicString<T, IdentifiableAllocator> string(
        initList, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), initList.size());
    EXPECT_GE(string.Capacity(), initList.size());

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Data()[initList.size()], T());
    EXPECT_TRUE(std::equal(initList.begin(), initList.end(), string.Data()));
}

// BasicString<T, Alloc>(BasicStringView<T> stringView, const Alloc& allocator)
TYPED_TEST(StringTests, StringViewConstructor)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 85;
    const BasicStringView<T> stringView = this->GetCString();

    BasicString<T, IdentifiableAllocator> string(
        stringView, IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Size(), stringView.Size());
    EXPECT_GE(string.Capacity(), stringView.Size());

    IdentifiableAllocator allocator(allocatorId);
    EXPECT_EQ(string.GetAllocator(), allocator);

    EXPECT_EQ(string.Data()[stringView.Size()], T());
    EXPECT_EQ(string.Raw(), stringView);
}

// BasicString<T, Alloc>& operator=(const BasicString<T, Alloc>& string)
TYPED_TEST(StringTests, CopyAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 542;
    const T* cstring = this->GetCString();

    ASSERT_GT(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    BasicString<T, IdentifiableAllocator> copy(
        this->GetShortCString(), IdentifiableAllocator(234234));

    copy = string;

    EXPECT_EQ(copy.Size(), string.Size());
    EXPECT_GE(copy.Capacity(), copy.Size());

    EXPECT_FALSE(copy.IsStorageLocal());

    EXPECT_EQ(copy.GetAllocator(), string.GetAllocator());
    EXPECT_EQ(std::char_traits<T>::compare(
        copy.Data(), string.Data(), copy.Size() + 1), 0);
}

// BasicString<T, Alloc>& operator=(BasicString<T, Alloc>&& string)
TYPED_TEST(StringTests, MoveAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 17;
    const T* cstring = this->GetCString();

    ASSERT_GT(std::char_traits<T>::length(cstring),
              BasicString<T>().GetLocalCapacity());

    BasicString<T, IdentifiableAllocator> string(
        cstring, IdentifiableAllocator(allocatorId));

    Usize size = string.Size();
    BasicString<T, IdentifiableAllocator> move(
        this->GetShortCString(), IdentifiableAllocator(112));

    move = std::move(string);

    EXPECT_EQ(move.Size(), size);
    EXPECT_GE(move.Capacity(), size);

    EXPECT_FALSE(move.IsStorageLocal());

    EXPECT_EQ(move.GetAllocator(), IdentifiableAllocator(allocatorId));
    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator());

    EXPECT_EQ(move.Data()[move.Size()], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        move.Data(), cstring, move.Size()), 0);
}

// BasicString<T, Alloc>& operator=(const T* string)
TYPED_TEST(StringTests, CstringAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 1327;
    BasicString<T, IdentifiableAllocator> string(
        this->GetCString(), IdentifiableAllocator(allocatorId));

    Usize size = std::char_traits<T>::length(this->GetShortCString());
    string = this->GetShortCString();

    EXPECT_EQ(string.Size(), size);
    EXPECT_GE(string.Capacity(), size);

    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Data()[string.Size()], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        string.Data(), this->GetShortCString(), string.Size()), 0);
}

// BasicString<T, Alloc>& operator=(T character)
TYPED_TEST(StringTests, CharacterAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 132132;
    BasicString<T, IdentifiableAllocator> string(
        this->GetCString(), IdentifiableAllocator(allocatorId));

    auto character = static_cast<T>('t');
    string = character;

    EXPECT_EQ(string.Size(), 1);
    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Data()[0], character);
    EXPECT_EQ(string.Data()[1], T());
}

// BasicString<T, Alloc>& operator=(std::initializer_list<T> initList)
TYPED_TEST(StringTests, InitListAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 1327123;
    BasicString<T, IdentifiableAllocator> string(
        this->GetCString(), IdentifiableAllocator(allocatorId));

    std::initializer_list<T> initList{ 32, 5, 12, 66, 123, 22 };
    string = initList;

    EXPECT_EQ(string.Size(), initList.size());
    EXPECT_GE(string.Capacity(), initList.size());

    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Data()[initList.size()], T());
    EXPECT_TRUE(std::equal(initList.begin(), initList.end(), string.Data()));
}

// BasicString<T, Alloc>& operator=(BasicStringView<T> string)
TYPED_TEST(StringTests, StringViewAssignmentOperator)
{
    using T = typename TestFixture::CharType;

    const Uint64 allocatorId = 132347;
    BasicString<T, IdentifiableAllocator> string(
        this->GetCString(), IdentifiableAllocator(allocatorId));

    Usize size = std::char_traits<T>::length(this->GetShortCString());
    string = BasicStringView<T>(this->GetShortCString());

    EXPECT_EQ(string.Size(), size);
    EXPECT_GE(string.Capacity(), size);

    EXPECT_EQ(string.GetAllocator(), IdentifiableAllocator(allocatorId));

    EXPECT_EQ(string.Data()[string.Size()], T());
    EXPECT_EQ(std::char_traits<T>::compare(
        string.Data(), this->GetShortCString(), string.Size()), 0);
}

// [const]T& operator[](Index index) [const]
TYPED_TEST(StringTests, SubscriptOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string;
    for (Index i = 0; i < string.Size(); ++i)
    {
        EXPECT_EQ(string.Data()[i], string[i]);
    }
}

// operator BasicStringView<T>() const
TYPED_TEST(StringTests, BasicStringViewCastOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    auto stringView = (BasicStringView<T>)string;

    EXPECT_EQ(stringView.Data(), string.Raw());
    EXPECT_EQ(stringView.Size(), string.Size());
}

// BasicString<T, Alloc>& operator+=(const BasicString<T, Alloc>& string)
TYPED_TEST(StringTests, BasicStringAppendOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicString<T> appendedString = this->GetShortCString();

    std::basic_string<T> expectedOutput = string.Data();
    expectedOutput += appendedString.Data();

    Usize expectedSize = string.Size() + appendedString.Size();
    string += appendedString;

    EXPECT_EQ(string.Size(), expectedSize);
    EXPECT_GE(string.Capacity(), expectedSize);

    EXPECT_TRUE(std::equal(expectedOutput.begin(), expectedOutput.end(), string.Data()));
}

// BasicString<T, Alloc>& operator+=(T character)
TYPED_TEST(StringTests, CharacterAppendOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    T character = static_cast<T>('h');

    std::basic_string<T> expectedOutput = string.Data();
    expectedOutput += character;

    Usize expectedSize = string.Size() + 1;
    string += character;

    EXPECT_EQ(string.Size(), expectedSize);
    EXPECT_GE(string.Capacity(), expectedSize);

    EXPECT_TRUE(std::equal(expectedOutput.begin(), expectedOutput.end(), string.Data()));
}

// BasicString<T, Alloc>& operator+=(const T* string)
TYPED_TEST(StringTests, CstringAppendOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    const T* appendedString = this->GetShortCString();

    Usize appendedStringSize = std::char_traits<T>::length(appendedString);

    std::basic_string<T> expectedOutput = string.Data();
    expectedOutput += appendedString;

    Usize expectedSize = string.Size() + appendedStringSize;
    string += appendedString;

    EXPECT_EQ(string.Size(), expectedSize);
    EXPECT_GE(string.Capacity(), expectedSize);

    EXPECT_TRUE(std::equal(expectedOutput.begin(), expectedOutput.end(), string.Data()));
}

// BasicString<T, Alloc>& operator+=(std::initializer_list initList)
TYPED_TEST(StringTests, InitListAppendOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    std::initializer_list<T> initList{ 2, 3, 4, 5, 121, 2 };

    std::basic_string<T> expectedOutput = string.Data();
    expectedOutput += initList;

    Usize expectedSize = string.Size() + initList.size();
    string += initList;

    EXPECT_EQ(string.Size(), expectedSize);
    EXPECT_GE(string.Capacity(), expectedSize);

    EXPECT_TRUE(std::equal(expectedOutput.begin(), expectedOutput.end(), string.Data()));
}

// BasicString<T, Alloc>& operator+=(BasicStringView<T> stringView)
TYPED_TEST(StringTests, BasicStringViewAppendOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicStringView<T> appendedString = this->GetShortCString();

    std::basic_string<T> expectedOutput = string.Data();
    expectedOutput += appendedString.Data();

    Usize expectedSize = string.Size() + appendedString.Size();
    string += appendedString;

    EXPECT_EQ(string.Size(), expectedSize);
    EXPECT_GE(string.Capacity(), expectedSize);

    EXPECT_TRUE(std::equal(expectedOutput.begin(), expectedOutput.end(), string.Data()));
}

/* operator+ is basically just the same as operator+=, skipping.. */

// [const] T& Front() [const]
// [const] T& Back() [const]
TYPED_TEST(StringTests, FrontAndBackGetters)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string = this->GetCString();

    EXPECT_EQ(string.Front(), string.Data()[0]);
    EXPECT_EQ(string.Back(), string.Data()[string.Size() - 1]);
}

// [Iterator/ConstIterator] GetBegin() [const]
// [Iterator/ConstIterator] GetEnd() [const]
// [Iterator/ConstIterator] GetReverseBegin() [const]
// [Iterator/ConstIterator] GetReverseEnd() [const]
TYPED_TEST(StringTests, IteratorGetters)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string = this->GetCString();

    EXPECT_EQ(*string.GetBegin(), string.Front());
    EXPECT_EQ(string.GetEnd(), string.GetBegin() + string.Size());

    EXPECT_EQ(*string.GetReverseBegin(), string.Back());
    EXPECT_EQ(string.GetReverseEnd(), string.GetReverseBegin() + static_cast<Ptrdiff>(string.Size()));
}

// void Assign(Usize count, T character)
TYPED_TEST(StringTests, FillAssign)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string(this->GetCString());

    const T character = static_cast<T>('a');
    string.Assign(56, character);

    EXPECT_EQ(string.Size(), 56);
    EXPECT_GE(string.Capacity(), 56);

    EXPECT_EQ(string.Data()[56], T());
    for (Index i = 0; i < 56; ++i)
    {
        EXPECT_EQ(string.Data()[i], character);
    }
}

// void Assign(It begin, It end)
TYPED_TEST(StringTests, RangeAssign)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string(this->GetCString());

    const typename TestFixture::CharType* cstring = this->GetShortCString();
    Usize length = std::char_traits<typename TestFixture::CharType>::length(cstring);

    MyForwardIterator<const typename TestFixture::CharType> beginIter(cstring);
    MyForwardIterator<const typename TestFixture::CharType> endIter(cstring + length);

    string.Assign(beginIter, endIter);

    EXPECT_EQ(string.Size(), length);
    EXPECT_GE(string.Capacity(), length);

    EXPECT_EQ(string.Data()[length], T());
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), cstring));
}

// void Clear()
TYPED_TEST(StringTests, Clear)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string(this->GetCString());

    string.Clear();
    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());
}

// void Reset()
TYPED_TEST(StringTests, Reset)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string(this->GetCString());

    string.Reset();
    EXPECT_EQ(string.Capacity(), string.GetLocalCapacity());

    EXPECT_EQ(string.Size(), 0);
    EXPECT_EQ(string.Data()[0], T());
}

/* Insert(index, const T* string) and Insert(index, const T* string,
 * Usize size) are both one-liners, skipping.
 * */

// void Insert(Index index, BasicStringView<T> stringView)
TYPED_TEST(StringTests, InsertBasicStringView)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicStringView<T> insertedString = this->GetShortCString();

    string.Insert(5, insertedString);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.insert(5, insertedString.Data());

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

/* Insert(Index index, T character) is a one-liner, skipping.. */

// void Insert(Index index, Usize count, T character)
TYPED_TEST(StringTests, InsertFill)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    T character = static_cast<T>('g');

    string.Insert(string.Size(), 35, character);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString += std::basic_string<T>(35, character);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

/* Insert(Index index, T character) is a one-liner, skipping.. */

// void Insert(Index index, It begin, It end)
TYPED_TEST(StringTests, InsertRange)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicStringView<T> insertedString = this->GetShortCString();

    string.Insert(string.Size(), insertedString.GetBegin(), insertedString.GetEnd());

    std::basic_string<T> expectedString = this->GetCString();
    expectedString += insertedString.Data();

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

/* Insert(Index index, std::initializer_list initList) is a one-liner, skipping... */

// void Remove(Index index)
TYPED_TEST(StringTests, Remove)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    string.Remove(2);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.erase(2, 1);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

/* Remove(Index index) is just Remove(index, 1), skipping.. */

// void Remove(Index beginPos, Usize count)
TYPED_TEST(StringTests, RemoveRange)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    string.Remove(2, 3);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.erase(2, 3);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void PushBack(T character)
TYPED_TEST(StringTests, PushBack)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    T character = static_cast<T>('p');

    string.PushBack(character);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.push_back(character);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void PopBack()
TYPED_TEST(StringTests, PopBack)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    string.PopBack();

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.pop_back();

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void Append(Usize count, T character)
TYPED_TEST(StringTests, AppendFill)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    T character = static_cast<T>('q');

    string.Append(5, character);

    std::basic_string<T> expectedString = this->GetCString();
    expectedString.append(5, character);

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void Append(const T* string, Usize size)
TYPED_TEST(StringTests, AppendSizedString)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    std::basic_string_view<T> stringView = this->GetShortCString();

    string.Append(stringView.data(), stringView.size());

    std::basic_string<T> expectedString = this->GetCString();
    expectedString += stringView;

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// void Append(It begin, It end)
TYPED_TEST(StringTests, AppendRange)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicStringView<T> stringView = this->GetShortCString();

    string.Append(stringView.GetBegin(), stringView.GetEnd());

    std::basic_string<T> expectedString = this->GetCString();
    expectedString += stringView.Data();

    EXPECT_EQ(string.Size(), expectedString.size());
    EXPECT_GE(string.Capacity(), expectedString.size());

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), expectedString.begin()));
}

// bool StartsWith(BasicStringView<T> stringView)
// bool StartsWith(T character)
// bool StartsWith(const T* string)
TYPED_TEST(StringTests, StartsWith)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();

    std::basic_string<T> expectedString = string.Data();
    BasicStringView<T> matchingSubstring(expectedString.data(), expectedString.data() + 3);
    BasicStringView<T> differingSubstring(expectedString.data() + 2, expectedString.data() + 3);

    EXPECT_TRUE(string.StartsWith(matchingSubstring));
    EXPECT_FALSE(string.StartsWith(differingSubstring));

    EXPECT_TRUE(string.StartsWith(expectedString[0]));
    EXPECT_FALSE(string.StartsWith('@'));

    EXPECT_TRUE(string.StartsWith(expectedString.substr(0, 4).data()));
    EXPECT_FALSE(string.StartsWith(expectedString.substr(2, 4).data()));
}

// bool EndsWith(BasicStringView<T> stringView)
// bool EndsWith(T character)
// bool EndsWith(const T* string)
TYPED_TEST(StringTests, EndsWith)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();

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
TYPED_TEST(StringTests, Contains)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();

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

// [Const]Iterator Find(BasicStringView<T> string) [const]
TYPED_TEST(StringTests, FindStringView)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(BasicStringView<T>(this->GetFoundString())), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(BasicStringView<T>(this->GetNotFoundString())), string.GetEnd());
}

// [Const]Iterator Find(const T* string) [const]
TYPED_TEST(StringTests, FindCString)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(this->GetFoundString()), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(this->GetNotFoundString()), string.GetEnd());
}

// [Const]Iterator Find(T character) [const]
TYPED_TEST(StringTests, FindCharacter)
{
    using T = typename TestFixture::CharType;
    BasicString<T> string = this->GetFindString();

    EXPECT_EQ(string.Find(static_cast<T>('M')), string.GetBegin() + 10);
    EXPECT_EQ(string.Find(static_cast<T>('x')), string.GetEnd());
}

// BasicStringView<T> Substring(Index startPos, Usize count)
TYPED_TEST(StringTests, Substring)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicStringView<T> substring = string.Substring(3, 5);

    std::basic_string<T> stdSubstring = this->GetCString();
    stdSubstring = stdSubstring.substr(3, 5);

    EXPECT_TRUE(std::equal(stdSubstring.begin(), stdSubstring.end(), substring.GetBegin()));
    EXPECT_EQ(substring.Size(), stdSubstring.size());
}

// void Swap(BasicString<T, Alloc>& string)
TYPED_TEST(StringTests, Swap)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetShortCString();
    BasicString<T> string2 = this->GetCString();

    std::basic_string<T> shortString = this->GetShortCString();
    std::basic_string<T> longString = this->GetCString();

    string.Swap(string2);

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), longString.begin()));
    EXPECT_TRUE(std::equal(string2.GetBegin(), string2.GetEnd(), shortString.begin()));

    EXPECT_EQ(string.Size(), longString.size());
    EXPECT_EQ(string2.Size(), shortString.size());
}

// void Reserve(Usize capacity)
TYPED_TEST(StringTests, Reserve)
{
    using T = typename TestFixture::CharType;
    const Usize reserveCount = 124;

    BasicString<T> string = this->GetCString();
    Usize previousSize = string.Size();

    ASSERT_GT(reserveCount, string.Capacity());
    string.Reserve(reserveCount);

    EXPECT_GE(string.Capacity(), reserveCount);
    EXPECT_EQ(string.Size(), previousSize);

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetCString()));

    const Usize smallReserveCount = 83;
    /* 83 is definitely smaller than 124.. */

    string.Reserve(smallReserveCount);

    EXPECT_GE(string.Capacity(), reserveCount);
    EXPECT_EQ(string.Size(), previousSize);

    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetCString()));
}

// void Shrink()
TYPED_TEST(StringTests, ShrinkLargeStrings)
{
    using T = typename TestFixture::CharType;
    const Usize reserveCount = 124;

    BasicString<T> string = this->GetCString();
    Usize previousSize = string.Size();

    ASSERT_GT(reserveCount, string.Capacity());
    string.Reserve(reserveCount);

    string.Shrink(reserveCount - 5);

    EXPECT_GE(string.Capacity(), reserveCount - 5);
    EXPECT_EQ(string.Size(), previousSize);
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetCString()));

    const Usize smallShrinkCount = 3;
    /* 83 is definitely smaller than 124.. */

    string.Shrink(smallShrinkCount);

    EXPECT_GE(string.Capacity(), reserveCount - 5);
    EXPECT_EQ(string.Size(), previousSize);
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetCString()));
}

TYPED_TEST(StringTests, ShrinkSmallStrings)
{
    using T = typename TestFixture::CharType;
    const Usize reserveCount = 124;

    BasicString<T> string = this->GetShortCString();
    Usize previousSize = string.Size();

    ASSERT_GT(reserveCount, string.Capacity());
    string.Reserve(reserveCount);

    string.Shrink(string.GetLocalCapacity());

    EXPECT_EQ(string.Capacity(), string.GetLocalCapacity());
    EXPECT_TRUE(string.IsStorageLocal());

    EXPECT_EQ(string.Size(), previousSize);
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetShortCString()));

    const Usize largeShrinkCount = 3234;
    /* 83 is definitely smaller than 124.. */

    string.Shrink(largeShrinkCount);

    EXPECT_EQ(string.Capacity(), string.GetLocalCapacity());
    EXPECT_TRUE(string.IsStorageLocal());

    EXPECT_EQ(string.Size(), previousSize);
    EXPECT_TRUE(std::equal(string.GetBegin(), string.GetEnd(), this->GetShortCString()));
}

/* ShrinkToFit() is basically Shrink(Size()), skipping... */

// bool operator==(const BasicString<T, Alloc1>& string1, const BasicString<T, Alloc2>& string2)
// bool operator==(const BasicString<T, Alloc>& string1, const T* string2)
// bool operator==(const T* string1, const BasicString<T, Alloc>& string2)
TYPED_TEST(StringTests, EqualOperator)
{
    using T = typename TestFixture::CharType;

    BasicString<T> string = this->GetCString();
    BasicString<T> sameString = this->GetCString();
    BasicString<T> differentString = this->GetShortCString();

    EXPECT_EQ(string, sameString);
    EXPECT_NE(string, differentString);

    EXPECT_EQ(string, sameString.Data());
    EXPECT_NE(string, differentString.Data());

    EXPECT_EQ(string.Data(), sameString);
    EXPECT_NE(string.Data(), differentString);
}
