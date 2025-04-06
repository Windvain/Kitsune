#include <gtest/gtest.h>

#include "CompareStrings.h"
#include "Foundation/Logging/Logger.h"

using namespace Kitsune;

namespace
{
    class DummySink : public ILogSink
    {
    public:
        void Log(const LogMessage&) override { /* ... */ };
    };

    class A : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override { Message = message; }
        void Flush() override { Flushed = true; }

    public:
        LogMessage Message;
        bool Flushed = false;
    };
}

TEST(LoggerTests, NameCtor)
{
    Logger logger = Logger("MyLogger");
    EXPECT_GENERAL_STREQ(logger.GetName().Data(), "MyLogger");
}

TEST(LoggerTests, NameAndSinkCtor)
{
    auto sink = MakeShared<DummySink>();
    Logger logger = Logger("Hello!", sink);

    EXPECT_GENERAL_STREQ(logger.GetName().Data(), "Hello!");
    EXPECT_EQ(logger.GetSinks()[0], sink);
}

TEST(LoggerTests, NameAndRangeCtor)
{
    SharedPtr<ILogSink> sinks[3] = { MakeShared<DummySink>(), MakeShared<DummySink>(), MakeShared<DummySink>() };
    Logger logger = Logger("Hello!", sinks, sinks + 3);

    EXPECT_GENERAL_STREQ(logger.GetName().Data(), "Hello!");
    EXPECT_EQ(logger.GetSinks()[0], sinks[0]);
    EXPECT_EQ(logger.GetSinks()[1], sinks[1]);
    EXPECT_EQ(logger.GetSinks()[2], sinks[2]);
}

TEST(LoggerTests, InitListCtor)
{
    SharedPtr<ILogSink> sinks[3] = { MakeShared<DummySink>(), MakeShared<DummySink>(), MakeShared<DummySink>() };
    Logger logger = Logger("Hello!", { sinks[0], sinks[1], sinks[2] });

    EXPECT_GENERAL_STREQ(logger.GetName().Data(), "Hello!");
    EXPECT_EQ(logger.GetSinks()[0], sinks[0]);
    EXPECT_EQ(logger.GetSinks()[1], sinks[1]);
    EXPECT_EQ(logger.GetSinks()[2], sinks[2]);
}

TEST(LoggerTests, LogComplete)
{
    Logger logger("LOGGER", MakeShared<A>());
    logger.Log(LogSeverity::Error, SourceLocation(), "Hello!");

    A* sink = dynamic_cast<A*>(logger.GetSinks()[0].Get());
    LogMessage message = sink->Message;

    EXPECT_STREQ(message.Message.Data(), "Hello!");
    EXPECT_EQ(message.Location, SourceLocation());
    EXPECT_EQ(message.LoggerName, "LOGGER");
    EXPECT_EQ(message.Severity, LogSeverity::Error);
}

TEST(LoggerTests, LogWithLocation)
{
    SourceLocation loc = SourceLocation::Current();

    Logger logger("LOGGER", MakeShared<A>());
    logger.Log(loc, "Hello!");

    A* sink = dynamic_cast<A*>(logger.GetSinks()[0].Get());
    LogMessage message = sink->Message;

    EXPECT_STREQ(message.Message.Data(), "Hello!");
    EXPECT_EQ(message.Location, loc);
    EXPECT_EQ(message.LoggerName, "LOGGER");
    EXPECT_EQ(message.Severity, logger.GetMinimumSeverity());
}

TEST(LoggerTests, LogWithSeverity)
{
    Logger logger("LOGGER", MakeShared<A>());
    logger.Log(LogSeverity::Error, "Hello!");

    A* sink = dynamic_cast<A*>(logger.GetSinks()[0].Get());
    LogMessage message = sink->Message;

    EXPECT_STREQ(message.Message.Data(), "Hello!");
    EXPECT_EQ(message.Location, SourceLocation());
    EXPECT_EQ(message.LoggerName, "LOGGER");
    EXPECT_EQ(message.Severity, LogSeverity::Error);
}

TEST(LoggerTests, Log)
{
    Logger logger("LOGGER", MakeShared<A>());
    logger.Log("Hello!");

    A* sink = dynamic_cast<A*>(logger.GetSinks()[0].Get());
    LogMessage message = sink->Message;

    EXPECT_STREQ(message.Message.Data(), "Hello!");
    EXPECT_EQ(message.Location, SourceLocation());
    EXPECT_EQ(message.LoggerName, "LOGGER");
    EXPECT_EQ(message.Severity, logger.GetMinimumSeverity());
}

TEST(LoggerTests, LogFlushCheck)
{
    SharedPtr<A> sink = MakeShared<A>();
    Logger logger = Logger("Logger", sink);

    logger.SetFlushSeverity(LogSeverity::Error);
    logger.Log(LogSeverity::Trace, "RaNDOM");

    EXPECT_EQ(sink->Flushed, false);

    logger.Log(LogSeverity::Error, "Random");
    EXPECT_EQ(sink->Flushed, true);
}

TEST(LoggerTests, SetGetMinimumSeverity)
{
    Logger logger;
    logger.SetMinimumSeverity(LogSeverity::Info);

    EXPECT_EQ(logger.GetMinimumSeverity(), LogSeverity::Info);
}

TEST(LoggerTests, SetGetFlushSeverity)
{
    Logger logger;
    logger.SetFlushSeverity(LogSeverity::Info);

    EXPECT_EQ(logger.GetFlushSeverity(), LogSeverity::Info);
}

TEST(LoggerTests, IsLogged)
{
    Logger logger;
    logger.SetMinimumSeverity(LogSeverity::Warning);

    EXPECT_FALSE(logger.IsLogged(LogSeverity::Trace));
    EXPECT_TRUE(logger.IsLogged(LogSeverity::Warning));
    EXPECT_TRUE(logger.IsLogged(LogSeverity::Error));
}

TEST(LoggerTests, IsFlushed)
{
    Logger logger;
    logger.SetFlushSeverity(LogSeverity::Warning);

    EXPECT_FALSE(logger.IsFlushed(LogSeverity::Trace));
    EXPECT_TRUE(logger.IsFlushed(LogSeverity::Warning));
    EXPECT_TRUE(logger.IsFlushed(LogSeverity::Error));
}
