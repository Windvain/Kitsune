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
        FakeStream() : ConsoleOutputStream(false) { /* ... */ }
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
    FakeStream stream;
    AnsiColorSink sink(stream, stream);

    SourceLocation loc = SourceLocation::Current();
    LogMessage message("Hello!", "LOGGER", Move(loc), LogSeverity::Error);

    constexpr std::string_view fmt = "\x1B[31;1m[LOGGER]: Hello! [In function {0}, {1}:{2}]\x1B[0m\n";
    std::string expected = std::format(fmt, loc.FunctionName(), loc.FileName(), loc.Line());

    sink.Log(message);
    EXPECT_STREQ(stream.Output.c_str(), expected.c_str());
}

TEST(AnsiColorSinkTests, LogWithoutLocation)
{
    FakeStream stream;
    AnsiColorSink sink(stream, stream);

    LogMessage message("Hello!", "MY_LOGGER", SourceLocation(), LogSeverity::Error);

    constexpr std::string_view expected = "\x1B[31;1m[MY_LOGGER]: Hello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream.Output.c_str(), expected.data());
}

TEST(AnsiColorSinkTests, LogWithoutName)
{
    FakeStream stream;
    AnsiColorSink sink(stream, stream);

    LogMessage message("Hello!", "", SourceLocation(), LogSeverity::Error);

    constexpr std::string_view expected = "\x1B[31;1mHello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream.Output.c_str(), expected.data());

}

TEST(AnsiColorSinkTests, StreamUsage)
{
    FakeStream regularStream;
    FakeStream errorStream;

    AnsiColorSink sink(regularStream, errorStream);
    LogMessage regularMessage("i am a regular message", "", SourceLocation(), LogSeverity::Trace);
    LogMessage errMessage("oh no no no no", "", SourceLocation(), LogSeverity::Error);

    sink.Log(regularMessage);
    sink.Log(errMessage);

    EXPECT_NE(regularStream.Output.find("i am a regular message"), std::string::npos);
    EXPECT_NE(errorStream.Output.find("oh no no no no"), std::string::npos);
}

TEST(AnsiColorSinkTests, Flush)
{
    FakeStream regularStream;
    FakeStream errorStream;

    AnsiColorSink sink(regularStream, errorStream);
    sink.Flush();

    EXPECT_TRUE(regularStream.Flushed);
    EXPECT_TRUE(errorStream.Flushed);
}
