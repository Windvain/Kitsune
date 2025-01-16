#include <gtest/gtest.h>
#include "Foundation/String/ToString.h"

using namespace Kitsune;

TEST(ToStringTests, ToStringSigned)
{
    EXPECT_STREQ(ToString((signed char)0).Raw(), "0");
    EXPECT_STREQ(ToString((signed char)127).Raw(), "127");
    EXPECT_STREQ(ToString((signed char)-72).Raw(), "-72");
    EXPECT_STREQ(ToString((signed short)0).Raw(), "0");
    EXPECT_STREQ(ToString((signed short)5548).Raw(), "5548");
    EXPECT_STREQ(ToString((signed short)-12359).Raw(), "-12359");
    EXPECT_STREQ(ToString((signed int)0).Raw(), "0");
    EXPECT_STREQ(ToString((signed int)8723293).Raw(), "8723293");
    EXPECT_STREQ(ToString((signed int)-8398288).Raw(), "-8398288");
    EXPECT_STREQ(ToString((signed long)0).Raw(), "0");
    EXPECT_STREQ(ToString((signed long)8398288).Raw(), "8398288");
    EXPECT_STREQ(ToString((signed long)-178288).Raw(), "-178288");
    EXPECT_STREQ(ToString((signed long long)0).Raw(), "0");
    EXPECT_STREQ(ToString((signed long long)1293134).Raw(), "1293134");
    EXPECT_STREQ(ToString((signed long long)-3812390).Raw(), "-3812390");

    EXPECT_STREQ(ToString((signed char)0, (signed char)8).Raw(), "0");
    EXPECT_STREQ(ToString((signed char)127, (signed char)8).Raw(), "177");
    EXPECT_STREQ(ToString((signed short)0, (signed short)16).Raw(), "0");
    EXPECT_STREQ(ToString((signed short)5548, (signed short)16).Raw(), "15AC");
    EXPECT_STREQ(ToString((signed int)0, (signed int)2).Raw(), "0");
    EXPECT_STREQ(ToString((signed int)8723293, (signed int)2).Raw(), "100001010001101101011101");
    EXPECT_STREQ(ToString((signed long)0, (signed long)12).Raw(), "0");
    EXPECT_STREQ(ToString((signed long)8398288, (signed long)12).Raw(), "2990154");
    EXPECT_STREQ(ToString((signed long long)0, (signed long long)3).Raw(), "0");
    EXPECT_STREQ(ToString((signed long long)1293134, (signed long long)3).Raw(), "2102200211212");

    EXPECT_STREQ(ToString((Int8)-72, (Int8)8).Raw(), "270");
    EXPECT_STREQ(ToString((Int16)-12359, (Int16)16).Raw(), "CFB9");
    EXPECT_STREQ(ToString((Int32)-42312359, (Int32)2).Raw(), "11111101011110100101110101011001");
    EXPECT_STREQ(ToString((Int64)-634242312359, (Int64)8).Raw(), "1777777766612415453531");
}

TEST(ToStringTests, ToStringUnsigned)
{
    EXPECT_STREQ(ToString((unsigned char)0).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned char)127).Raw(), "127");
    EXPECT_STREQ(ToString((unsigned short)0).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned short)5548).Raw(), "5548");
    EXPECT_STREQ(ToString((unsigned int)0).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned int)8723293).Raw(), "8723293");
    EXPECT_STREQ(ToString((unsigned long)0).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned long)8398288).Raw(), "8398288");
    EXPECT_STREQ(ToString((unsigned long long)0).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned long long)1293134).Raw(), "1293134");

    EXPECT_STREQ(ToString((unsigned char)0, (unsigned char)8).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned char)127, (unsigned char)8).Raw(), "177");
    EXPECT_STREQ(ToString((unsigned short)0, (unsigned short)16).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned short)5548, (unsigned short)16).Raw(), "15AC");
    EXPECT_STREQ(ToString((unsigned int)0, (unsigned int)2).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned int)8723293, (unsigned int)2).Raw(), "100001010001101101011101");
    EXPECT_STREQ(ToString((unsigned long)0, (unsigned long)12).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned long)8398288, (unsigned long)12).Raw(), "2990154");
    EXPECT_STREQ(ToString((unsigned long long)0, (unsigned long long)3).Raw(), "0");
    EXPECT_STREQ(ToString((unsigned long long)1293134, (unsigned long long)3).Raw(), "2102200211212");
}

TEST(ToStringTests, ToStringFloat)
{
    // TODO: The ToString() implementation for floating-point data types is currently.. a WIP.
    //       b.c. stuff like the decimal seperator have to take locale into account.
    EXPECT_TRUE(true);
}
