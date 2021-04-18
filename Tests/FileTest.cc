// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <simple-mips-emu/File.hh>

#include "TestCommon.hh"
#include <sstream>

char const _validCase[] = R"===(
    0x2
    0x4

    0x1234
0x23452a45

    0x9876
    0x3DE1
    0x2457B
    0x78A5c16f
)===";

TEST(FileTest, ValidCase)
{
    std::istringstream iss { _validCase };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);

    {
        // clang-format off
        std::vector<uint8_t> expected {
            0x00, 0x00, 0x12, 0x34,
            0x23, 0x45, 0x2A, 0x45
        };
        // clang-format on

        ASSERT_EQ_VECTOR(file.text, expected, *lit, *rit);
    }

    {
        // clang-format off
        std::vector<uint8_t> expected{
            0x00, 0x00, 0x98, 0x76,
            0x00, 0x00, 0x3D, 0xE1,
            0x00, 0x02, 0x45, 0x7B,
            0x78, 0xA5, 0xC1, 0x6F
        };
        // clang-format on

        ASSERT_EQ_VECTOR(file.data, expected, *lit, *rit);
    }
}

char const _invalidFormat[] = R"===(
    alksjd
    Hello, world!
    0x12345678
)===";

TEST(FileTest, InvalidFormat)
{
    std::istringstream iss { _invalidFormat };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CannotRead>(result));

    CannotRead error = std::get<CannotRead>(result);
    ASSERT_EQ(error.error.type, FileReadError::Type::InvalidFormat);
}

char const _invalidSectionSize[] = R"===(
    0x2
    0x4
    0x1236
)===";

TEST(FileTest, InvalidSectionSize)
{
    std::istringstream iss { _invalidSectionSize };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CannotRead>(result));

    CannotRead error = std::get<CannotRead>(result);
    ASSERT_EQ(error.error.type, FileReadError::Type::SectionSizeDoesNotMatch);
}