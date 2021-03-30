// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <simple-mips-emu/Lib.hh>

TEST(AddTest, ValidCase)
{
    ASSERT_EQ(Add(1.2, 4.8), 6.0);
    ASSERT_EQ(Add(3.7, 2.4), 6.1);
}

TEST(AddTest, InvalidCase)
{
    ASSERT_NE(Add(3.9, 12.4), 0.0);
}