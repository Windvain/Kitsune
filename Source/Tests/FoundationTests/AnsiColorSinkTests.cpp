#include <gtest/gtest.h>
#include "Foundation/Logging/AnsiColorSink.h"

#include <format>
#include <source_location>

using namespace Kitsune;

namespace
{
    class FakeStream : public ConsoleOutputStream
    {
    public:
        FakeStream() : ConsoleOutputStream() { /* ... */ }
        ~FakeStream() { /* ... */ }

    public:
        void Write(const char* ptr, Usize count) override
        {
            Output += std::string_view(ptr, count);
        }

        void Flush() override { Flushed = true; }

    public:
        std::string Output;
        bool Flushed;
    };
}

TEST(AnsiColorSinkTests, LogFullData)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    SourceLocation loc = SourceLocation::Current();
    LogMessage message("Hello!", "LOGGER", Move(loc), LogSeverity::Error);

    constexpr std::string_view fmt = "\x1B[31;1m[LOGGER]: Hello! [In function {0}, {1}:{2}]\x1B[0m\n";
    std::string expected = std::format(fmt, loc.FunctionName(), loc.FileName(), loc.Line());

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.c_str());
}

TEST(AnsiColorSinkTests, LogWithoutLocation)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    LogMessage message("Hello!", "MY_LOGGER", SourceLocation(), LogSeverity::Error);

    constexpr std::string_view expected = "\x1B[31;1m[MY_LOGGER]: Hello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.data());
}

TEST(AnsiColorSinkTests, LogWithoutName)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    LogMessage message("Hello!", "", SourceLocation(), LogSeverity::Error);

    constexpr std::string_view expected = "\x1B[31;1mHello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.data());

}

TEST(AnsiColorSinkTests, Flush)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    sink.Flush();
    EXPECT_TRUE(stream->Flushed);
}
