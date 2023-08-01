#include <unity.h>
#include "InputHelper.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_tryParseHex_should_consumeCorrectValue(void)
{
    char str[] = "0x01fA";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseHex(str, i, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0x1fa, value);
    TEST_ASSERT_EQUAL_INT(6, i);
}

void test_tryParseHex_should_consumeOnlyHexDigits(void)
{
    char str[] = "0x01cd2q";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseHex(str, i, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0x1cd2, value);
    TEST_ASSERT_EQUAL_INT(7, i);
}

void test_tryParseHex_should_failWithout0xPrefix(void)
{
    char str[] = "1cd0";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseHex(str, i, value);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT16(0, value);
    TEST_ASSERT_EQUAL_INT(0, i);
}

void test_tryParseDec_should_consumeCorrectValue(void)
{
    char str[] = "1234";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseDec(str, i, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(1234, value);
    TEST_ASSERT_EQUAL_INT(4, i);
}

void test_tryParseDec_should_failOnIncorrectValue(void)
{
    char str[] = "a111";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseDec(str, i, value);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT16(0, value);
    TEST_ASSERT_EQUAL_INT(0, i);
}


void test_tryConsumeNumber_should_consumeCorrectDecNumber(void)
{
    char str[] = "4321";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseNumber(str, i, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(4321, value);
    TEST_ASSERT_EQUAL_INT(4, i);
}

void test_tryConsumeNumber_should_consumeCorrectHexNumber(void)
{
    char str[] = "0x234";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseNumber(str, i, value);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_UINT16(0x234, value);
    TEST_ASSERT_EQUAL_INT(5, i);
}

void test_tryConsumeNumber_should_failOnIncorrectNumber(void)
{
    char str[] = "foobar";
    int i = 0;
    unsigned short value = 0;
    bool result = tryParseNumber(str, i, value);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL_UINT16(0, value);
    TEST_ASSERT_EQUAL_INT(0, i);
}

void test_tryParseInput_should_parseReadByteCommand(void)
{
    char str[] = "r 0x222";
    unsigned short a = 0;
    unsigned short b = 0;
    CommandType cmd = parseInput(str, a, b);
    TEST_ASSERT_EQUAL(CommandType::ReadByte, cmd);
    TEST_ASSERT_EQUAL_UINT16(0x222, a);
    TEST_ASSERT_EQUAL_UINT16(0, b);
}

void test_tryParseInput_should_parseReadBlockAbsoluteCommand(void)
{
    char str[] = "r 0x0100-0x014f";
    unsigned short a = 0;
    unsigned short b = 0;
    CommandType cmd = parseInput(str, a, b);
    TEST_ASSERT_EQUAL(CommandType::ReadBlockAbsolute, cmd);
    TEST_ASSERT_EQUAL_UINT16(0x100, a);
    TEST_ASSERT_EQUAL_UINT16(0x14f, b);
}

void test_tryParseInput_should_parseReadBlockWithCountCommand(void)
{
    char str[] = "r 0x1000 1024";
    unsigned short a = 0;
    unsigned short b = 0;
    CommandType cmd = parseInput(str, a, b);
    TEST_ASSERT_EQUAL(CommandType::ReadBlockWithCount, cmd);
    TEST_ASSERT_EQUAL_UINT16(0x1000, a);
    TEST_ASSERT_EQUAL_UINT16(1024, b);
}

void test_tryParseInput_should_parseWriteByteCommand(void)
{
    char str[] = "w 0x4000 0xff";
    unsigned short a = 0;
    unsigned short b = 0;
    CommandType cmd = parseInput(str, a, b);
    TEST_ASSERT_EQUAL(CommandType::WriteByte, cmd);
    TEST_ASSERT_EQUAL_UINT16(0x4000, a);
    TEST_ASSERT_EQUAL_UINT16(0xff, b);
}

void test_tryParseInput_should_failForWriteWithOneArguent(void)
{
    char str[] = "w 0x1111";
    unsigned short a = 0;
    unsigned short b = 0;
    CommandType cmd = parseInput(str, a, b);
    TEST_ASSERT_EQUAL(CommandType::Unknown, cmd);
    TEST_ASSERT_EQUAL_UINT16(0, a);
    TEST_ASSERT_EQUAL_UINT16(0, b);
}

int runUnityTests(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tryParseHex_should_consumeCorrectValue);
    RUN_TEST(test_tryParseHex_should_consumeOnlyHexDigits);
    RUN_TEST(test_tryParseHex_should_failWithout0xPrefix);

    RUN_TEST(test_tryParseDec_should_consumeCorrectValue);
    RUN_TEST(test_tryParseDec_should_failOnIncorrectValue);

    RUN_TEST(test_tryConsumeNumber_should_consumeCorrectDecNumber);
    RUN_TEST(test_tryConsumeNumber_should_consumeCorrectHexNumber);
    RUN_TEST(test_tryConsumeNumber_should_failOnIncorrectNumber);

    RUN_TEST(test_tryParseInput_should_parseReadByteCommand);
    RUN_TEST(test_tryParseInput_should_parseReadBlockAbsoluteCommand);
    RUN_TEST(test_tryParseInput_should_parseReadBlockWithCountCommand);
    RUN_TEST(test_tryParseInput_should_parseWriteByteCommand);
    RUN_TEST(test_tryParseInput_should_failForWriteWithOneArguent);

    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}

void setup(void)
{
    delay(2000);
    runUnityTests();
}

void loop(void)
{
}