#include <gtest/gtest.h>

#include "CompareStrings.h"
#include "Foundation/Logging/Logger.h"

using namespace Kitsune;

namespace LoggerTesting
{
    class DummySink : public ILoggerSink
    {
    public:
        void Log(const LogMessage&) override { /* ... */ };
    };

    class A : public ILoggerSink
    {
    public:
        void Log(const LogMessage& message) override { Message = message; }
        void Flush() override { Flushed = true; }
    public:
        LogMessage Message;
        bool Flushed = false;
    };
}

using namespace LoggerTesting;

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
    SharedPtr<ILoggerSink> sinks[3] = { MakeShared<DummySink>(), MakeShared<DummySink>(), MakeShared<DummySink>() };
    Logger logger = Logger("Hello!", sinks, sinks + 3);

    EXPECT_GENERAL_STREQ(logger.GetName().Data(), "Hello!");
    EXPECT_EQ(logger.GetSinks()[0], sinks[0]);
    EXPECT_EQ(logger.GetSinks()[1], sinks[1]);
    EXPECT_EQ(logger.GetSinks()[2], sinks[2]);
}

TEST(LoggerTests, LogWithSeverity)
{
    SharedPtr<A> sinks[2] = { MakeShared<A>(), MakeShared<A>() };
    Logger logger = Logger("Logger", sinks, sinks + 2);

    logger.Log(LogSeverity::Error, "Yep.");

    LogMessage message1 = sinks[0]->Message;
    LogMessage message2 = sinks[1]->Message;

    EXPECT_GENERAL_STREQ(message1.LoggerName.Data(), "Logger");
    EXPECT_GENERAL_STREQ(message2.LoggerName.Data(), "Logger");

    EXPECT_GENERAL_STREQ(message1.Message.Data(), "Yep.");
    EXPECT_GENERAL_STREQ(message2.Message.Data(), "Yep.");

    EXPECT_EQ(message1.Severity, LogSeverity::Error);
    EXPECT_EQ(message2.Severity, LogSeverity::Error);
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

TEST(LoggerTests, LogWithoutSeverity)
{
    SharedPtr<A> sinks[2] = { MakeShared<A>(), MakeShared<A>() };
    Logger logger = Logger("Logger", sinks, sinks + 2);

    logger.Log("Yep.");

    LogMessage message1 = sinks[0]->Message;
    LogMessage message2 = sinks[1]->Message;

    EXPECT_GENERAL_STREQ(message1.LoggerName.Data(), "Logger");
    EXPECT_GENERAL_STREQ(message2.LoggerName.Data(), "Logger");

    EXPECT_GENERAL_STREQ(message1.Message.Data(), "Yep.");
    EXPECT_GENERAL_STREQ(message2.Message.Data(), "Yep.");

    EXPECT_EQ(message1.Severity, logger.GetMinimumSeverity());
    EXPECT_EQ(message2.Severity, logger.GetMinimumSeverity());
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
