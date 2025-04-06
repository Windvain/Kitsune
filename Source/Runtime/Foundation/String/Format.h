#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/String/Formatter.h"
#include "Foundation/String/FormatScanner.h"

namespace Kitsune
{
    namespace Internal
    {
        class StringFormatIterator
        {
        public:
            using ValueType = char;
            using DifferenceType = IteratorTraits<String::Iterator>::DifferenceType;

        public:
            StringFormatIterator() = default;
            explicit StringFormatIterator(String& string)
                : m_String(&string)
            {
            }

        public:
            StringFormatIterator& operator=(char value)
            {
                m_String->PushBack(value);
                return *this;
            }

        public:
            StringFormatIterator& operator*() { return *this; }

            StringFormatIterator& operator++()   { return *this; }
            StringFormatIterator operator++(int) { return *this; }

        private:
            String* m_String = nullptr;
        };
    }

    template<WritableIterator<char> OutIt, FormatScanner<OutIt> Scanner, typename... Args>
    void FormatTo(OutIt&& out, Scanner&& scanner, const StringView fmt, Args&&... args)
    {
        FormatArgumentPack<OutIt> argumentPack = MakeFormatArgumentPack<OutIt>(Forward<Args>(args)...);
        StringView formatSpecs;

        StringView::Iterator pointer = fmt.GetBegin();

        while (true)
        {
            scanner.Step();
            formatSpecs = scanner.GetFormatSpecs();

            out = Algorithms::Copy(pointer, formatSpecs.GetBegin(), out);

            if (formatSpecs.Size() == 0)
                break;

            out = Scanner::ParseFormatSpecs(out, formatSpecs, argumentPack);
            pointer = formatSpecs.GetEnd();
        }
    }

    template<typename... Args>
    [[nodiscard]] String Format(const StringView fmt, Args&&... args)
    {
        using namespace Internal;

        String string(fmt.Size());
        FormatTo(StringFormatIterator(string), DefaultFormatScanner(fmt), fmt,
                 Forward<Args>(args)...);

        return string;
    }
}
