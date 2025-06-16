#pragma once

#include "Foundation/String/String.h"

#if defined(_WIN32)
    #define MAKE_UTF16(str) L ## str
    #define UTF16_STR WideString
    #define UTF16_STRV WideStringView
#else
    #define MAKE_UTF16(str) u ## str
    #define UTF16_STR U16String
    #define UTF16_STRV U16StringView
#endif

