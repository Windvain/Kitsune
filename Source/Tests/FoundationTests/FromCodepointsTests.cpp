#include <gtest/gtest.h>

#include "CompareStrings.h"
#include "TestContainer2.h"

#include "Foundation/String/String.h"
#include "Foundation/String/FromCodepoints.h"

#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

namespace
{
    template<typename T>
    class PushBackableContainer
    {
    public:
        using ValueType = T;

        using Iterator = T*;
        using ConstIterator = const T*;

    public:
        PushBackableContainer() = default;

        inline void PushBack(const T& element)
        {
            m_Data.push_back(element);
        }

        inline T* Data() { return m_Data.data(); }
        inline const T* Data() const { return m_Data.data(); }

    public:
        // Spoof unused functions to satisfy Container concept.
        T* GetBegin() { return nullptr; }
        const T* GetBegin() const { return nullptr; }

        T* GetEnd() { return nullptr; }
        const T* GetEnd() const { return nullptr; }

        void Swap(PushBackableContainer<T>& cont)
        {
            m_Data.swap(cont.m_Data);
        }

        bool operator==(const PushBackableContainer<T>&) const { return true; }

    private:
        std::vector<T> m_Data;
    };
}

template<std::size_t N>
using CodepointTestContainer = Testing::ForwardTestContainer<Unicode::Codepoint, N>;

TEST(FromCodepointsTests, FromCodepoointsChar)
{
    PushBackableContainer<char> str;
    CodepointTestContainer<10> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6, 0x0 };

    Unicode::FromCodepoints<char>(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<decltype(str)>(str));
    EXPECT_GENERAL_STREQ(str.Data(), "»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, FromCodepointsChar8)
{
    PushBackableContainer<char8_t> str;
    CodepointTestContainer<10> codepoints = { 0x00BB, 0x005F, 0x00FC, 0x00FF, 0x2194, 0x2563, 0x1F923, 0x1F495, 0x1F3B6, 0x0 };

    Unicode::FromCodepoints<char8_t>(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<PushBackableContainer<char8_t>>(str));
    EXPECT_GENERAL_STREQ(str.Data(), u8"»_üÿ↔╣🤣💕🎶");
}

TEST(FromCodepointsTests, FromCodepointsChar16)
{
    PushBackableContainer<char16_t> str;
    CodepointTestContainer<6> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518, 0x0 };

    Unicode::FromCodepoints<char16_t>(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<PushBackableContainer<char16_t>>(str));
    EXPECT_GENERAL_STREQ(str.Data(), u"┬ÿ¼↔┘");
}

TEST(FromCodepointsTests, FromCodepointsChar32)
{
    PushBackableContainer<char32_t> str;
    CodepointTestContainer<6> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518, 0x0 };

    Unicode::FromCodepoints<char32_t>(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<PushBackableContainer<char32_t>>(str));
    EXPECT_GENERAL_STREQ(str.Data(), U"┬ÿ¼↔┘");
}

#if defined(_WIN32)
    static_assert(sizeof(wchar_t) == 2, "sizeof(wchar_t) should be equal to 2.");
#else
    static_assert(sizeof(wchar_t) == 4, "sizeof(wchar_t) should be equal to 4.");
#endif

TEST(FromCodepointsTests, FromCodepointsWideChar)
{

    PushBackableContainer<wchar_t> str;
    CodepointTestContainer<6> codepoints = { 0x252C, 0x00FF, 0x00BC, 0x2194, 0x2518, 0x0 };

    Unicode::FromCodepoints<wchar_t>(codepoints.GetBegin(), codepoints.GetEnd(), BackInsertIterator<PushBackableContainer<wchar_t>>(str));
    EXPECT_GENERAL_STREQ(str.Data(), L"┬ÿ¼↔┘");
}
