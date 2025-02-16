#pragma once

// TODO: Remove after implementing unicode encoding conversions.

#include <Windows.h>

#include "Foundation/String/String.h"
#include "Foundation/String/InvalidUnicodeException.h"

namespace Kitsune::Internal
{
    inline WideString WindowsConvertToUtf16(const StringView string)
    {
        int strSize = static_cast<int>(string.Size());         // Avoid truncating conversion warnings.
        int wideSize = ::MultiByteToWideChar(CP_UTF8, 0, string.Data(), strSize, nullptr, 0);

        if (wideSize == ERROR_NO_UNICODE_TRANSLATION)
            throw InvalidUnicodeException();

        WideString wideString(wideSize, '\0');
        ::MultiByteToWideChar(CP_UTF8, 0, string.Data(), strSize, wideString.Data(), wideSize);

        return wideString;
    }

    inline String WindowsConvertToUtf8(const WideStringView string)
    {
        int wideSize = static_cast<int>(string.Size());         // Avoid truncating conversion warnings.
        int size = ::WideCharToMultiByte(CP_UTF8, 0, string.Data(), wideSize,
            nullptr, 0, nullptr, nullptr);

        if (size == ERROR_NO_UNICODE_TRANSLATION)
            throw InvalidUnicodeException();

        String utf8string(size, '\0');
        ::WideCharToMultiByte(CP_UTF8, 0, string.Data(), wideSize, utf8string.Data(), size,
            nullptr, nullptr);

        return utf8string;
    }
}
