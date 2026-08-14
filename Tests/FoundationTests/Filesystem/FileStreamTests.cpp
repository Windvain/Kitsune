#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>
#include "TrackingAllocator.h"
#include "StatefulAllocator.h"

#include "Foundation/Streams/FileStream.h"

namespace
{
    using namespace Kitsune;
    using Testing::TrackingAllocator, Testing::StatefulAllocator;

    class FileStreamTest : public ::testing::Test
    {
    protected:
        inline void SetUp() override
        {
            if (std::filesystem::exists("example.txt"))
            {
                throw std::runtime_error(
                    "example.txt already exists in the current directory. Delete it "
                    "before running this test executable. This should not happen.");
            }

            m_FileStream.open("./example.txt", std::ios::out);
            if (!m_FileStream.is_open())
                throw std::runtime_error("Failed to open the example file.");

            m_FileStream << "Hello, World!";
            m_FileStream.close();
        }

        inline void TearDown() override
        {
            std::filesystem::remove("./example.txt");
        }

        inline ~FileStreamTest()
        {
            // Remove the file again, just in case something happened in which the
            // TearDown() function was not called properly.
            TearDown();
        }

    private:
        std::ofstream m_FileStream;
    };

    // FileStream<BufSize, Alloc>::FileStream()
    TEST_F(FileStreamTest, AllocatorConstructor)
    {
        BasicFileStream<4096, StatefulAllocator> stream(StatefulAllocator(42));
        EXPECT_FALSE(stream.IsOpen());
        EXPECT_EQ(stream.GetAllocator().GetId(), 42);
    }

    // FileStream<BufSize, Alloc>::FileStream(StringView, FileAccessMode, FileOpenMode)
    TEST_F(FileStreamTest, FileConstructor)
    {
        // This will contain just the basic file operations specific to the constructor.
        // The details will be tested in the Open() test.
        FileStream stream("./example.txt", FileAccessMode::ReadWrite);
        EXPECT_TRUE(stream.IsOpen());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::FileStream(StringView, FileAccessMode, FileOpenMode)
    TEST_F(FileStreamTest, FileConstructorThrows)
    {
        // If opening the file fails in the constructor, there is no way of directly
        // signaling this to the user. Just throw.
        EXPECT_THROW(
            FileStream(
                "./example.txt", FileAccessMode::Read,
                FileOpenMode::CreateNew),
            SystemException);
    }

    // FileStream<BufSize, Alloc>::FileStream(FileStream&&)
    TEST_F(FileStreamTest, MoveConstructor)
    {
        FileStream stream("./example.txt", FileAccessMode::ReadWrite);
        FileStream move = Move(stream);

        EXPECT_FALSE(stream.IsOpen());

        EXPECT_TRUE(move.IsOpen());
        EXPECT_EQ(move.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::~FileStream()
    TEST_F(FileStreamTest, Destructor)
    {
        {
            FileStream stream("./example.txt", FileAccessMode::Read);
            KITSUNE_UNUSED(stream);
        }

        // The stream should close the file on destruction, therefore any streams
        // which attempts to open the file should succeed.
        FileStream stream;
        EXPECT_TRUE(stream.Open("./example.txt", FileAccessMode::Read));
    }

    // FileStream<BufSize, Alloc>::operator=(FileStream&&)
    TEST_F(FileStreamTest, MoveAssign)
    {
        FileStream stream("./example.txt", FileAccessMode::ReadWrite);
        FileStream move(
            "./example2.txt", FileAccessMode::Read,
            FileOpenMode::OpenOrCreate);

        move = Move(stream);

        EXPECT_FALSE(stream.IsOpen());

        EXPECT_TRUE(move.IsOpen());
        EXPECT_EQ(move.GetPath().GetFilename(), "example.txt");

        std::filesystem::remove("./example2.txt");
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode::Read)
    TEST_F(FileStreamTest, OpenForReading)
    {
        FileStream readStream;

        EXPECT_TRUE(readStream.Open("./example.txt", FileAccessMode::Read));
        EXPECT_EQ(readStream.GetPath().GetFilename(), "example.txt");

        EXPECT_TRUE(readStream.IsReadable());
        EXPECT_TRUE(readStream.IsSeekable());
        EXPECT_FALSE(readStream.IsWritable());
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode::Write)
    TEST_F(FileStreamTest, OpenForWriting)
    {
        FileStream writeStream;

        EXPECT_TRUE(writeStream.Open("./example.txt", FileAccessMode::Write));
        EXPECT_EQ(writeStream.GetPath().GetFilename(), "example.txt");

        EXPECT_TRUE(writeStream.IsWritable());
        EXPECT_TRUE(writeStream.IsSeekable());
        EXPECT_FALSE(writeStream.IsReadable());
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode::ReadWrite)
    TEST_F(FileStreamTest, OpenForReadingAndWriting)
    {
        FileStream rwStream;

        EXPECT_TRUE(rwStream.Open("./example.txt", FileAccessMode::ReadWrite));
        EXPECT_EQ(rwStream.GetPath().GetFilename(), "example.txt");

        EXPECT_TRUE(rwStream.IsReadable());
        EXPECT_TRUE(rwStream.IsWritable());
        EXPECT_TRUE(rwStream.IsSeekable());
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode, FileOpenMode::Open)
    TEST_F(FileStreamTest, OpenIfExists)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Open));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_TRUE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode, FileOpenMode::Open)
    TEST_F(FileStreamTest, OpenIfExistsFailCase)
    {
        FileStream stream;
        EXPECT_FALSE(stream.Open(
            "./example2.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Open));

        EXPECT_FALSE(stream.IsOpen());
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::CreateNew)
    TEST_F(FileStreamTest, OpenCreateNew)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example2.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::CreateNew));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_TRUE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example2.txt");

        stream.Close();
        std::filesystem::remove("./example2.txt");
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::CreateNew)
    TEST_F(FileStreamTest, OpenCreateNewFailCase)
    {
        FileStream stream;
        EXPECT_FALSE(stream.Open(
            "./example.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::CreateNew));

        EXPECT_FALSE(stream.IsOpen());
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::OpenOrCreate)
    TEST_F(FileStreamTest, OpenCreateOrOpen)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example2.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::OpenOrCreate));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_TRUE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example2.txt");

        stream.Close();
        std::filesystem::remove("./example2.txt");
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::OpenOrCreate)
    TEST_F(FileStreamTest, OpenCreateOrOpen2)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::OpenOrCreate));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_TRUE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::Truncate)
    TEST_F(FileStreamTest, OpenTruncate)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Truncate));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_TRUE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example.txt");

        EXPECT_TRUE(std::filesystem::is_empty("example.txt"));
    }

    // FileStream<BufSize, Alloc>::Open(
    //     StringView, FileAccessMode, FileOpenMode::Truncate)
    TEST_F(FileStreamTest, OpenTruncateFailCase)
    {
        FileStream stream;
        EXPECT_FALSE(stream.Open(
            "./example2.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Truncate));

        EXPECT_FALSE(stream.IsOpen());
        EXPECT_FALSE(stream.Open(
            "./example.txt",
            FileAccessMode::Read,
            FileOpenMode::Truncate));

        EXPECT_FALSE(stream.IsOpen());
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode, FileOpenMode::Append)
    TEST_F(FileStreamTest, OpenAppend)
    {
        FileStream stream;
        EXPECT_TRUE(stream.Open(
            "./example.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Append));

        EXPECT_TRUE(stream.IsOpen());
        EXPECT_FALSE(stream.IsSeekable());
        EXPECT_EQ(stream.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::Open(StringView, FileAccessMode, FileOpenMode::Append)
    TEST_F(FileStreamTest, OpenAppendFailCase)
    {
        FileStream stream;
        EXPECT_FALSE(stream.Open(
            "./example2.txt",
            FileAccessMode::ReadWrite,
            FileOpenMode::Append));

        EXPECT_FALSE(stream.IsOpen());
        EXPECT_FALSE(stream.Open(
            "./example.txt",
            FileAccessMode::Read,
            FileOpenMode::Append));

        EXPECT_FALSE(stream.IsOpen());
    }

    // FileStream<BufSize, Alloc>::Close()
    TEST_F(FileStreamTest, Close)
    {
        FileStream stream("./example.txt", FileAccessMode::Read);
        ASSERT_TRUE(stream.IsOpen());

        stream.Close();
        EXPECT_FALSE(stream.IsOpen());
    }

    // FileStream<BufSize, Alloc>::Write(const Byte*, Usize)
    TEST_F(FileStreamTest, Write)
    {
        FileStream stream("./example.txt", FileAccessMode::Write, FileOpenMode::Truncate);
        const char* data = "Hello, there!";

        stream.Write(reinterpret_cast<const Byte*>(data), 13);
        EXPECT_EQ(stream.GetPosition(), 13);

        stream.Close();

        std::ifstream file("./example.txt", std::ios::in | std::ios::binary);
        std::vector<char> buffer(std::filesystem::file_size("example.txt"));

        file.read(
            buffer.data(),
            std::streamsize(buffer.size()));

        EXPECT_EQ(std::string_view(buffer.data(), buffer.size()), data);
    }

    // FileStream<BufSize, Alloc>::Write(const Byte*, Usize)
    TEST_F(FileStreamTest, WriteWithoutPerms)
    {
        FileStream stream("./example.txt", FileAccessMode::Read);
        const char data[5] = { 0x73, 0x68, 0x69, 0x74, 0x65 };

        EXPECT_THROW(
            stream.Write(reinterpret_cast<const Byte*>(data), 5),
            LogicException);
    }

    // FileStream<BufSize, Alloc>::Read(Byte*, Usize)
    TEST_F(FileStreamTest, Read)
    {
        FileStream stream("./example.txt", FileAccessMode::Read);
        std::vector<Byte> buffer(8, Byte());

        EXPECT_EQ(stream.Read(buffer.data(), 8), 8);
        EXPECT_EQ(stream.GetPosition(), 8);

        EXPECT_EQ(
            std::string_view(reinterpret_cast<const char*>(buffer.data()), 8),
            "Hello, W");
    }

    // FileStream<BufSize, Alloc>::Read(Byte*, Usize)
    TEST_F(FileStreamTest, ReadWithoutPerms)
    {
        FileStream stream("./example.txt", FileAccessMode::Write);
        Byte buffer[5];

        EXPECT_THROW(stream.Read(buffer, 5), LogicException);
    }

    // FileStream<BufSize, Alloc>::Seek(Ptrdiff, SeekOrigin)
    TEST_F(FileStreamTest, Seek)
    {
        FileStream stream("./example.txt", FileAccessMode::ReadWrite);
        EXPECT_EQ(stream.Seek(5, SeekOrigin::Begin), 5);

        Byte buffer[3];
        EXPECT_EQ(stream.GetPosition(), 5);
        EXPECT_EQ(stream.Read(buffer, 3), 3);
        EXPECT_EQ(std::string_view((const char*)buffer, 3), ", W");

        ASSERT_EQ(stream.GetPosition(), 8);

        EXPECT_EQ(stream.Seek(-2, SeekOrigin::Current), 6);

        EXPECT_EQ(stream.GetPosition(), 6);
        EXPECT_EQ(stream.Read(buffer, 3), 3);
        EXPECT_EQ(std::string_view((const char*)buffer, 3), " Wo");

        ASSERT_EQ(stream.GetPosition(), 9);

        EXPECT_EQ(stream.Seek(-2, SeekOrigin::End), 11);

        EXPECT_EQ(stream.GetPosition(), 11);
        EXPECT_EQ(stream.Read(buffer, 2), 2);
        EXPECT_EQ(std::string_view((const char*)buffer, 2), "d!");
    }

    // FileStream<BufSize, Alloc>::Length()
    TEST_F(FileStreamTest, Length)
    {
        FileStream stream("./example.txt", FileAccessMode::Read);
        EXPECT_EQ(stream.Length(), 13);
    }

    /* IsReadable(), IsWritable(), IsSeekable(), GetAllocator(), GetPosition(),
     * and GetPath() is assumed to always work.
     */

    // FileStream<BufSize, Alloc>::Swap(FileStream&)
    TEST_F(FileStreamTest, Swap)
    {
        BasicFileStream<4096, StatefulAllocator> stream(StatefulAllocator(65));
        BasicFileStream<4096, StatefulAllocator> stream2(StatefulAllocator(19));

        ASSERT_TRUE(stream.Open("./example.txt", FileAccessMode::ReadWrite));
        stream.Swap(stream2);

        EXPECT_FALSE(stream.IsOpen());
        EXPECT_TRUE(stream2.IsOpen());

        EXPECT_EQ(stream.GetAllocator().GetId(), 19);
        EXPECT_EQ(stream2.GetAllocator().GetId(), 65);

        EXPECT_EQ(stream2.GetPath().GetFilename(), "example.txt");
    }

    // FileStream<BufSize, Alloc>::GetAllocator()
    TEST_F(FileStreamTest, StreamUsesAllocator)
    {
        BasicFileStream<4096, TrackingAllocator> stream(
            "./example.txt",
            FileAccessMode::Read);

        EXPECT_EQ(stream.GetAllocator().AllocationCount(), 1);
        EXPECT_EQ(stream.GetAllocator().AllocationSize(), 4096);
    }
}
