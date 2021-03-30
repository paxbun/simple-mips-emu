// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <simple-mips-emu/Memory.hh>

TEST(MemoryTest, Init)
{
    Memory memory { 7, 9 };

    for (uint8_t i = 0; i < 31; ++i) ASSERT_EQ(memory.GetRegister(i), 0);
    ASSERT_EQ(memory.GetRegister(32), Address::MakeText(0));

    ASSERT_EQ(memory.GetTextSize(), 7);
    for (uint32_t i = 0; i < 7; ++i) ASSERT_EQ(memory.GetByte(Address::MakeText(i)), 0);
    EXPECT_THROW(memory.GetByte(Address::MakeText(7)), std::out_of_range);
    EXPECT_THROW(memory.GetWord(Address::MakeText(4)), std::out_of_range);

    ASSERT_EQ(memory.GetDataSize(), 9);
    for (uint32_t i = 0; i < 9; ++i) ASSERT_EQ(memory.GetByte(Address::MakeData(i)), 0);
    EXPECT_THROW(memory.GetByte(Address::MakeData(9)), std::out_of_range);
    EXPECT_THROW(memory.GetWord(Address::MakeText(6)), std::out_of_range);
}

TEST(MemoryTest, Load)
{
    Memory memory { 5, 10 };

    std::vector<uint8_t> text { 1, 2, 3, 4, 5 };
    std::vector<uint8_t> data { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    memory.Load(Address::BaseType::Text, text);
    memory.Load(Address::BaseType::Data, data);

    ASSERT_EQ(memory.GetWord(Address::MakeText(1)), 0x02030405);
    ASSERT_EQ(memory.GetWord(Address::MakeData(2)), 0x08070605);
}

TEST(MemoryTest, Register)
{
    Memory memory { 0, 0 };

    EXPECT_THROW(memory.GetRegister(33), std::out_of_range);

    memory.SetRegister(18, 0x1234);
    ASSERT_EQ(memory.GetRegister(18), 0x1234);
}