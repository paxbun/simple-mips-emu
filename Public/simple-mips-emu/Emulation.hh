// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#ifndef SIMPLE_MIPS_EMU_EMULATION_HH
#define SIMPLE_MIPS_EMU_EMULATION_HH

#include <simple-mips-emu/Memory.hh>

enum class TickResult
{
    Success = 0,

    /// <summary>
    /// The program is already terminated.
    /// </summary>
    AlreadyTerminated,

    /// <summary>
    /// The emulator cannot recognize the current instruction.
    /// </summary>
    InvalidInstruction,

    /// <summary>
    /// The current instruction references invalid memory.
    /// </summary>
    MemoryOutOfRange,

    /// <summary>
    /// Branch or memory R/W instruction has negative offset whose absolute value is too big.
    /// </summary>
    OffsetIsTooSmall,
};

/// <summary>
/// Runs one instruction and mutate the given memory. If the result is not
/// <c>TickResult::Success</c>, the memory is not mutated.
/// </summary>
TickResult Tick(Memory& memory) noexcept;

#endif