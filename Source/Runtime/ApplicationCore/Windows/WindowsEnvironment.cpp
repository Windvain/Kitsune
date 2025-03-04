#include "ApplicationCore/Environment.h"

#include <cstdlib>
#include <Windows.h>

#include "Foundation/Windows/StringConversions.h"

namespace Kitsune
{
    // Ignore argc and argv on Windows, only needed on UNIX platforms.
    // ...except OSX.
    void Environment::Initialize(int argc, char** argv)
    {
        KITSUNE_UNUSED(argc);
        KITSUNE_UNUSED(argv);
    }

    void Environment::Shutdown()
    {
    }

    CommandLineArguments Environment::GetCommandLineArguments()
    {
        LPWSTR commandLine = ::GetCommandLineW();

        int argc;
        LPWSTR* wargv = ::CommandLineToArgvW(commandLine, &argc);

        KITSUNE_ASSERT(wargv != nullptr, "::CommandLineToArgvW() should not have failed..");

        Array<String> argv(argc);
        for (int i = 0; i < argc; ++i)
        {
            argv.PushBack(Internal::WindowsConvertToUtf8(wargv[i]));
        }

       ::LocalFree(wargv);
        return CommandLineArguments(argv);
    }

    Path Environment::GetCurrentWorkingDirectory()
    {
        DWORD len = ::GetCurrentDirectoryW(0, nullptr);
        if (len == 0) return Path();

        WideString str(len - 1, L'\0');
        ::GetCurrentDirectoryW(len, str.Data());

        str.Remove(str.GetBegin() + len, str.GetEnd());
        return Path(Move(str));
    }

    void Environment::SetCurrentWorkingDirectory(const Path& path)
    {
        // Failed? Dont't care.
        KITSUNE_UNUSED(::SetCurrentDirectoryW(path.Native().Raw()));
    }

    Path Environment::GetExecutablePath()
    {
        constexpr Usize PathStartingSize = 128;

        WideString exePath(PathStartingSize, L'\0');
        DWORD pathLen = 0;

        do
        {
            pathLen = ::GetModuleFileNameW(nullptr, exePath.Data(), exePath.Size() + 1);
            if (pathLen == 0)
                return Path(L"");

            // Yes, we are using global errors to control the flow of
            // the program. Who the f*ck thought that this would be a
            // good idea?
            if (!(pathLen == exePath.Size() + 1) ||
                !(::GetLastError() == ERROR_INSUFFICIENT_BUFFER))
            {
                break;
            }
            else
            {
                exePath = WideString(exePath.Size() * 2, L'\0');
            }
        }
        while (true);

        exePath.Remove(exePath.GetBegin() + pathLen, exePath.GetEnd());
        return exePath;
    }
}
