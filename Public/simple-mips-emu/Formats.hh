// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#ifndef SIMPLE_MIPS_EMU_CONSTANTS_HH
#define SIMPLE_MIPS_EMU_CONSTANTS_HH

#include <cstdint>

enum class RFormatFn : uint32_t
{
    ADDU = 0x21,
    SUBU = 0x23,
    AND  = 0x24,
    OR   = 0x25,
    NOR  = 0x27,
    SLTU = 0x2B,
};

enum class JRFormatFn : uint32_t
{
    JR = 0x08,
};

enum class SRFormatFn : uint32_t
{
    SLL = 0x00,
    SRL = 0x02,
};

enum class IFormatOp : uint32_t
{
    ADDIU = 0x09,
    ANDI  = 0x0C,
    ORI   = 0x0D,
    SLTIU = 0x0B,
};

enum class BIFormatOp : uint32_t
{
    BEQ = 0x04,
    BNE = 0x05,
};

enum class IIFormatOp : uint32_t
{
    LUI = 0x0F,
};

enum class OIFormatOp : uint32_t
{
    LB = 0x20,
    LW = 0x23,
    SB = 0x28,
    SW = 0x2B,
};

enum class JFormatOp : uint32_t
{
    J   = 0x02,
    JAL = 0x03,
};

#endif