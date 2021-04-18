// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <simple-mips-emu/File.hh>

#include "TestCommon.hh"
#include <sstream>

char const _validCase[] = "0x2\n"
                          "0x4\n"
                          "0x1234\n"
                          "0x23452a45\n"
                          "0x9876\n"
                          "0x3DE1\n"
                          "0x2457B\n"
                          "0x78A5c16f\n";

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