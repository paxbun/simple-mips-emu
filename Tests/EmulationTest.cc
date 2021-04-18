// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <gtest/gtest.h>
#include <simple-mips-emu/Emulation.hh>
#include <simple-mips-emu/File.hh>

/*
    .data
array:
    .word 0
    .word 1
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
array_end:

    .text
main:
    la     $8,   array
    la     $9,   array_end
    addiu  $9,   $9,   -8
loop:
    lw     $10,  0($8)
    lw     $11,  4($8)
    addu   $10,  $10,  $11
    sw     $10,  8($8)
    addiu  $8,   4
    bne    $8,   $9,   loop
*/

/*
    uint32_t array[10] = { 0, 1 };
    int main() {
        uint32_t* r8 = array;
        uint32_t* r9 = array_end - 2;
        while (r8 != r9) {
            r8[2] = r8[0] + r8[1];
            r8 += 1;
        }
    }
*/

char const _fibonacci[] = R"===(
    0x28
    0x28
    0x3c081000
    0x3c091000
    0x35290028
    0x2529fff8
    0x8d0a0000
    0x8d0b0004
    0x14b5021
    0xad0a0008
    0x25080004
    0x1509fffa
    0x0
    0x1
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
)===";

TEST(EmulationTest, Fibonacci)
{
    std::istringstream iss { _fibonacci };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);
    Memory  memory { std::move(file.text), std::move(file.data) };

    while (!memory.IsTerminated())
    {
        auto result = Tick(memory);
        ASSERT_EQ(result, TickResult::Success);
    }

    {
        // clang-format off
        Address address = Address::MakeData(0);
        std::vector<uint32_t> expected { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };

        for (size_t i = 0; i < 10; ++i) {
            ASSERT_EQ(memory.GetWord(address), expected[i]);
            address.offset += 4;
        }
        // clang-format on
    }
}

/*
    .data
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
stack:
    .text
main:
    la     $29,  stack
    lui    $4,   0x13
    ori    $4,   $4,   0xC02
    lui    $5,   0x5E
    ori    $5,   $5,   0x5E67
    jal    gcd
    j      end
gcd:
    addiu  $29,  $29,  -4
    sw     $31,  0($29)
if:
    bne    $4,   $5,   elif
if_true:
    addu   $2,   $0,   $4
    lw     $31,  0($29)
    addiu  $29,  $29,  4
    jr     $31
elif:
    sltu   $1,   $5,   $4
    beq    $1,   $0,   else
elif_true:
    subu   $4,   $4,   $5
    jal    gcd
    lw     $31,  0($29)
    addiu  $29,  $29,  4
    jr     $31
else:
    subu   $5,   $5,   $4
    jal    gcd
    lw     $31,  0($29)
    addiu  $29,  $29,  4
    jr     $31
end:
*/

/*
    int main() {
        return gcd(6184551, 1248258);
    }

    uint32_t gcd(uint32_t r4, uint32_t r5) {
        if (r4 == r5)
            return r4;
        elif (r4 > r5)
            return gcd(r4 - r5, r5);
        else:
            return gcd(r4, r5 - r4);
    }
*/

char const _gcd[] = R"===(
    0x6c
    0x80
    0x3c1d1000
    0x37bd0080
    0x3c040013
    0x34840c02
    0x3c05005e
    0x34a55e67
    0xc100008
    0x810001b
    0x27bdfffc
    0xafbf0000
    0x14850004
    0x41021
    0x8fbf0000
    0x27bd0004
    0x3e00008
    0xa4082b
    0x10200005
    0x852023
    0xc100008
    0x8fbf0000
    0x27bd0004
    0x3e00008
    0xa42823
    0xc100008
    0x8fbf0000
    0x27bd0004
    0x3e00008
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
    0x0
)===";

TEST(EmulationTest, GCD)
{
    std::istringstream iss { _gcd };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);
    Memory  memory { std::move(file.text), std::move(file.data) };

    while (!memory.IsTerminated())
    {
        auto result = Tick(memory);
        ASSERT_EQ(result, TickResult::Success);
    }

    ASSERT_EQ(memory.GetRegister(2), 56739);
}

/*
    .data
array:
    .word 74
    .word 43
    .word 95
    .word 62
    .word 100
    .word 68
    .word 86
    .word 4
    .word 42
    .word 20
    .text
main:
for_outer_init:
    la     $8,    array
for_outer_cond:
    la     $1,     array
    addiu  $1,     $1,     36
    sltu   $1,     $8,     $1
    beq    $1,     $0,     for_outer_end
for_outer_body:
    lw     $9,     0($8)

for_inner_init:
    addiu  $10,    $8,     4
for_inner_cond:
    la     $1,     array
    addiu  $1,     $1,     40
    sltu   $1,     $10,    $1
    beq    $1,     $0,     for_inner_end
for_inner_body:
    lw     $11,    0($10)

if:
    sltu   $1,     $11,    $9
    beq    $1,     $0,     if_end
if_body:
    addu   $12,    $0,     $9
    addu   $9,     $0,     $11
    addu   $11,    $0,     $12
if_end:

    sw     $11,     0($10)
for_inner_rep:
    addiu  $10,    $10,    4
    j      for_inner_cond
for_inner_end:

    sw     $9,     0($8)
for_outer_rep:
    addiu  $8,     $8,     4
    j      for_outer_cond
for_outer_end:
*/

/*
    uint32_t array[10] = { 74, 43, 95, 62, 100, 68, 86, 4, 42, 20 };
    int main() {
        for (uint32_t r8 = 0; r8 < 9; ++r8) {
            uint32_t r9 = array[r8];
            for (uint32_t r10 = r8 + 1; r10 < 10; ++r10) {
                uint32_t r11 = array[r10];
                if (r9 > r11) {
                    uint32_t r12 = r9;
                    r9 = r11;
                    r11 = r12;
                }
                array[r10] = r11;
            }
            array[r8] = r9;
        }
    }
*/

char const _selectionSort[] = R"===(
    0x5c
    0x28
    0x3c081000
    0x3c011000
    0x24210024
    0x101082b
    0x10200012
    0x8d090000
    0x250a0004
    0x3c011000
    0x24210028
    0x141082b
    0x10200009
    0x8d4b0000
    0x169082b
    0x10200003
    0x96021
    0xb4821
    0xc5821
    0xad4b0000
    0x254a0004
    0x8100007
    0xad090000
    0x25080004
    0x8100001
    0x4a
    0x2b
    0x5f
    0x3e
    0x64
    0x44
    0x56
    0x4
    0x2a
    0x14
)===";

TEST(EmulationTest, SelectionSort)
{
    std::istringstream iss { _selectionSort };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);
    Memory  memory { std::move(file.text), std::move(file.data) };

    while (!memory.IsTerminated())
    {
        auto result = Tick(memory);
        ASSERT_EQ(result, TickResult::Success);
    }

    {
        // clang-format off
        Address address = Address::MakeData(0);
        std::vector<uint32_t> expected { 4, 20, 42, 43, 62, 68, 74, 86, 95, 100 };

        for (size_t i = 0; i < 10; ++i) {
            ASSERT_EQ(memory.GetWord(address), expected[i]);
            address.offset += 4;
        }
        // clang-format on
    }
}

/*
    .text
main:
    addiu   $8,  $0,  5
while_cond:
    sltiu   $1,  $8,  -5
    bne     $1,  $0,  end
while_body:
    addiu   $8,  $8,  -1
    j       while_cond
end:
*/

/*
    int main() {
        int32_t r8 = 5;
        while (r8 >= -5) {
            r8 -= 1;
        }
    }
*/

char const _simpleLoop[] = R"===(
    0x14
    0x0
    0x24080005
    0x2d01fffb
    0x14200002
    0x2508ffff
    0x8100001
)===";

TEST(EmulationTest, SimpleLoop)
{
    std::istringstream iss { _simpleLoop };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);
    Memory  memory { std::move(file.text), std::move(file.data) };

    while (!memory.IsTerminated())
    {
        auto result = Tick(memory);
        ASSERT_EQ(result, TickResult::Success);
    }

    ASSERT_EQ(memory.GetRegister(8), -6);
}

/*
    .data
string:
    .word 0x48656C6C
    .word 0x6F2C2077
    .word 0x6F726C64
    .word 0x21000000
    .text
main:
    la     $8,   string
loop:
    lb     $1,   0($8)
    beq    $0,   $1,   end
    addiu  $8,   $8,   1
    j      loop
end:
    la     $9,   string
    subu   $8,   $8,   $9
*/

/*
    int main() {
        int r8 = strlen("Hello, world!");
    }
*/

char const _strlen[] = R"===(
    0x1c
    0x10
    0x3c081000
    0x81010000
    0x10010002
    0x25080001
    0x8100001
    0x3c091000
    0x1094023
    0x48656c6c
    0x6f2c2077
    0x6f726c64
    0x21000000
)===";

TEST(EmulationTest, Strlen)
{
    std::istringstream iss { _strlen };

    FileReadResult result = ReadFile(iss);
    ASSERT_TRUE(std::holds_alternative<CanRead>(result));

    CanRead file = std::get<CanRead>(result);
    Memory  memory { std::move(file.text), std::move(file.data) };

    while (!memory.IsTerminated())
    {
        auto result = Tick(memory);
        ASSERT_EQ(result, TickResult::Success);
    }

    ASSERT_EQ(memory.GetRegister(8), 13);
}
