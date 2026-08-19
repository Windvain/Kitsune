#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>
#include "StatefulAllocator.h"

#include "Foundation/Streams/FileWriter.h"
#include "Foundation/String/TranscodePresets.h"

namespace
{
    using namespace Kitsune;
    using Testing::StatefulAllocator;

    template<TextEncoding Encoding>
    class FileWriterTest : public ::testing::Test
    {
    public:
        using ValueType = typename Encoding::CodeunitType;
        using EncodingType = Encoding;

        using WriterType = BasicFileWriter<Encoding, 4096, GlobalAllocator>;

    public:
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

            m_FileStream.close();
        }

        inline void TearDown() override
        {
            std::filesystem::remove("./example.txt");
        }

        inline ~FileWriterTest() override
        {
            // Remove the file again, just in case something happened in which the
            // TearDown() function was not called properly.
            TearDown();
        }

    private:
        std::ofstream m_FileStream;
    };


    using FileWriterTestTypes = ::testing::Types<
        UTF8Encoding<char>,
        UTF8Encoding<char8_t>,
        UTF16Encoding<char16_t>,
        UTF32Encoding<char32_t>>;

    TYPED_TEST_SUITE(FileWriterTest, FileWriterTestTypes);

    TYPED_TEST(FileWriterTest, DefaultConstructor)
    {
        using Writer = typename TestFixture::WriterType;

        Writer writer;
        EXPECT_FALSE(writer.IsOpen());
    }

    TYPED_TEST(FileWriterTest, AllocatorConstructor)
    {
        using Encoding = typename TestFixture::EncodingType;

        StatefulAllocator allocator(20);
        BasicFileWriter<Encoding, 4096, StatefulAllocator> writer(allocator);

        EXPECT_FALSE(writer.IsOpen());
        EXPECT_EQ(writer.GetAllocator(), StatefulAllocator(20));
    }

    TYPED_TEST(FileWriterTest, MoveStreamConstructor)
    {
        using Writer = typename TestFixture::WriterType;

        FileStream stream("./example.txt", FileAccessMode::Write);
        Writer writer(Move(stream));

        EXPECT_TRUE(writer.IsOpen());
        EXPECT_EQ(writer.GetPath().GetFilename(), "example.txt");
    }

    TYPED_TEST(FileWriterTest, StreamConstructorThrows)
    {
        using Writer = typename TestFixture::WriterType;

        FileStream stream("./example.txt", FileAccessMode::Read);
        EXPECT_THROW((Writer(Move(stream))), InvalidArgumentException);
    }

    TYPED_TEST(FileWriterTest, FileConstructor)
    {
        using Writer = typename TestFixture::WriterType;

        Writer writer("./example.txt", FileAccessMode::ReadWrite);

        EXPECT_TRUE(writer.IsOpen());
        EXPECT_EQ(writer.GetPath().GetFilename(), "example.txt");

        EXPECT_TRUE(writer.GetStream().IsReadable());
        EXPECT_TRUE(writer.GetStream().IsWritable());
    }

    TYPED_TEST(FileWriterTest, FileConstructorThrows)
    {
        using Writer = typename TestFixture::WriterType;
        EXPECT_THROW(
            (Writer("./example.txt", FileAccessMode::Read)),
            InvalidArgumentException);
    }

    /* BasicFileWriter::operator=(BasicFileWriter&&) and
     * BasicFileWriter::BasicFileWriter(BasicFileWriter&&) are defaulted. They use the
     * move constructor & assign operators from BasicFileStream.
     * */

    /* BasicFileWriter::GetPath(), BasicFileWriter::GetAllocator(), and
     * BasicFileWriter::GetStream() are expected to work.
     * */

    // BasicFileWriter::Open(...) uses the member function from BasicFileStream, but
    // FileAccessMode cannot be set to Read.
    TYPED_TEST(FileWriterTest, OpenThrows)
    {
        using Writer = typename TestFixture::WriterType;
        EXPECT_THROW(
            (Writer("./example.txt", FileAccessMode::Read)),
            InvalidArgumentException);
    }

    /* BasicFileWriter::Close() and BasicFileWriter::IsOpen() uses BasicFileStream's
     * member functions. They are tested in the FileStreamTests.cpp.
     * */

    // HACK: Used engine code here, no native way of converting between UTF-8
    // and other encodings in C++ without libraries.
    TYPED_TEST(FileWriterTest, Write)
    {
        using Writer = typename TestFixture::WriterType;
        using Char = typename TestFixture::ValueType;

        using Encoding = typename TestFixture::EncodingType;

        const auto translate = [](StringView string) -> BasicString<Char>
        {
            return TranscodeString<UTF8Encoding<char>, Encoding>(string.Data());
        };

        Writer writer("./example.txt");
        writer.Write(translate("Hello, World!").Raw(), 5);      // Overload (1)

        writer.Close();
        writer.Open("./example.txt", FileOpenMode::Append);

        writer.Write(translate(", World!").Raw());              // Overload (2)
        writer.Close();

        std::ifstream file("./example.txt", std::ios::binary | std::ios::ate);
        ASSERT_TRUE(file.is_open());

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size, char{});
        ASSERT_TRUE(file.read(buffer.data(), size));

        BasicString<Char> expected(Encoding::GetPreamble());
        expected += translate("Hello, World!");

        EXPECT_EQ(
            expected,
            BasicStringView<Char>(
                reinterpret_cast<const Char*>(buffer.data()),
                buffer.size() / sizeof(Char)));
    }

    /* BasicFileWriter::Flush() calls BasicFileStream's Flush() function. No tests.
     * */
}
