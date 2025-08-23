#include <gtest/gtest.h>
#include "Foundation/Logging/AnsiColorSink.h"

#include <format>
#include <source_location>

using namespace Kitsune;

namespace
{
    class FakeStream : public IWriteStream<char>
    {
    public:
        FakeStream()  { /* ... */ }
        ~FakeStream() { /* ... */ }

    public:
        void Write(const char* ptr, Usize count) override
        {
            Output += std::string_view(ptr, count);
        }

        void Flush() override
        {
            Output.clear();
        }

    public:
        std::string Output;
    };

    template<typename... Args>
    std::string RuntimeFormat(std::string_view str, Args&&... args)
    {
        return std::vformat(str, std::make_format_args(args...));
    }
}

TEST(AnsiColorSinkTests, LogSeverity)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    LogMessage message("", "", SourceLocation(), LogSeverity::Trace);

    sink.Log(message);
    EXPECT_EQ(std::memcmp(stream->Output.c_str(), AnsiColorSink::TraceColor, std::strlen(AnsiColorSink::TraceColor)), 0);

    stream->Flush();

    message.Severity = LogSeverity::Info;
    sink.Log(message);

    EXPECT_EQ(std::memcmp(stream->Output.c_str(), AnsiColorSink::InfoColor, std::strlen(AnsiColorSink::InfoColor)), 0);

    stream->Flush();

    message.Severity = LogSeverity::Warning;
    sink.Log(message);

    EXPECT_EQ(std::memcmp(stream->Output.c_str(), AnsiColorSink::WarningColor, std::strlen(AnsiColorSink::WarningColor)), 0);

    stream->Flush();

    message.Severity = LogSeverity::Error;
    sink.Log(message);

    EXPECT_EQ(std::memcmp(stream->Output.c_str(), AnsiColorSink::ErrorColor, std::strlen(AnsiColorSink::ErrorColor)), 0);

    stream->Flush();

    message.Severity = LogSeverity::Fatal;
    sink.Log(message);

    EXPECT_EQ(std::memcmp(stream->Output.c_str(), AnsiColorSink::FatalColor, std::strlen(AnsiColorSink::FatalColor)), 0);
}

TEST(AnsiColorSinkTests, LogFullData)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    SourceLocation loc = SourceLocation::Current();
    LogMessage message("Hello!", "LOGGER", Move(loc), LogSeverity::Error);

    std::string fmt = std::string(AnsiColorSink::ErrorColor) + "[LOGGER]: Hello! [In function {0}, {1}:{2}]\x1B[0m\n";
    std::string expected = RuntimeFormat(fmt, loc.FunctionName(), loc.FileName(), loc.Line());

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.c_str());
}

TEST(AnsiColorSinkTests, LogWithoutLocation)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    LogMessage message("Hello!", "MY_LOGGER", SourceLocation(), LogSeverity::Info);

    std::string expected = std::string(AnsiColorSink::InfoColor) + "[MY_LOGGER]: Hello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.data());
}

TEST(AnsiColorSinkTests, LogWithoutName)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    LogMessage message("Hello!", "", SourceLocation(), LogSeverity::Warning);

    std::string expected = std::string(AnsiColorSink::WarningColor) + "Hello!\x1B[0m\n";

    sink.Log(message);
    EXPECT_STREQ(stream->Output.c_str(), expected.data());
}

TEST(AnsiColorSinkTests, Flush)
{
    auto stream = MakeShared<FakeStream>();
    AnsiColorSink sink(stream);

    sink.Log(LogMessage("Hello!", "", SourceLocation(), LogSeverity::Error));
    ASSERT_STRNE(stream->Output.c_str(), "");

    sink.Flush();
    EXPECT_EQ(stream->Output.size(), 0);
}
