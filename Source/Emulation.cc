// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Emulation.hh>
#include <simple-mips-emu/Formats.hh>

namespace
{

template <typename T, typename... Ts>
inline bool IsOneOf(T value, Ts... ts)
{
    return ((static_cast<T>(ts) == value) || ...);
}

TickResult TickHandleR(Memory& memory, uint32_t current)
{
    uint32_t const operation = (current >> 26) & 0b111111;
    if (operation != 0)
        return TickResult::InvalidInstruction;

    uint32_t const function = (current >> 0) & 0b111111;
    if (IsOneOf(function,
                RFormatFn::ADDU,
                RFormatFn::SUBU,
                RFormatFn::AND,
                RFormatFn::NOR,
                RFormatFn::OR,
                RFormatFn::SLTU))
    {
        // instruction is R format
        uint32_t const source1     = (current >> 21) & 0b11111;
        uint32_t const source2     = (current >> 16) & 0b11111;
        uint32_t const destination = (current >> 11) & 0b11111;

        uint32_t const source1Value = memory.GetRegister(source1);
        uint32_t const source2Value = memory.GetRegister(source2);

        uint32_t destinationValue;
        switch (static_cast<RFormatFn>(function))
        {
            case RFormatFn::ADDU: destinationValue = source1Value + source2Value; break;
            case RFormatFn::SUBU: destinationValue = source1Value - source2Value; break;
            case RFormatFn::AND: destinationValue = source1Value & source2Value; break;
            case RFormatFn::NOR: destinationValue = ~(source1Value | source2Value); break;
            case RFormatFn::OR: destinationValue = source1Value | source2Value; break;
            case RFormatFn::SLTU: destinationValue = source1Value < source2Value; break;
            default: return TickResult::InvalidInstruction;
        }
        memory.SetRegister(destination, destinationValue);
        memory.AdvancePC();

        return TickResult::Success;
    }
    else if (IsOneOf(function, JRFormatFn::JR))
    {
        // instruction is JR format
        uint32_t const source = (current >> 21) & 0b11111;

        memory.SetRegister(Memory::PC, memory.GetRegister(source));
        memory.AdvancePC();

        return TickResult::Success;
    }
    else if (IsOneOf(function, SRFormatFn::SLL, SRFormatFn::SRL))
    {
        // instruction is SR format
        uint32_t const source      = (current >> 16) & 0b11111;
        uint32_t const destination = (current >> 11) & 0b11111;
        uint32_t const shiftAmount = (current >> 6) & 0b11111;

        uint32_t const sourceValue = memory.GetRegister(source);

        uint32_t destinationValue;
        switch (static_cast<SRFormatFn>(function))
        {
            case SRFormatFn::SLL: destinationValue = sourceValue << shiftAmount; break;
            case SRFormatFn::SRL: destinationValue = sourceValue >> shiftAmount; break;
            default: return TickResult::InvalidInstruction;
        }
        memory.SetRegister(destination, destinationValue);
        memory.AdvancePC();

        return TickResult::Success;
    }

    return TickResult::InvalidInstruction;
}

TickResult TickHandleI(Memory& memory, uint32_t current)
{
    // TODO
    return TickResult::InvalidInstruction;
}

TickResult TickHandleJ(Memory& memory, uint32_t current)
{
    uint32_t const operation = (current >> 26) & 0b111111;
    uint32_t const target    = current & 0x03FFFFFF;
    if (operation == static_cast<uint32_t>(JFormatOp::J))
    {
        memory.SetRegister(Memory::PC, target * 4);
        return TickResult::Success;
    }
    else if (operation == static_cast<uint32_t>(JFormatOp::JAL))
    {
        memory.SetRegister(Memory::RA, memory.GetRegister(Memory::PC) + 4);
        memory.SetRegister(Memory::PC, target * 4);
        return TickResult::Success;
    }

    return TickResult::InvalidInstruction;
}

using TickHandler = TickResult (*)(Memory& memory, uint32_t current);

TickHandler _handlers[] = {
    TickHandleR,
    TickHandleI,
    TickHandleJ,
};

}

TickResult Tick(Memory& memory) noexcept
{
    if (memory.IsTerminated())
        return TickResult::AlreadyTerminated;

    try
    {
        uint32_t current = memory.GetWord(Address::MakeFromWord(memory.GetRegister(Memory::PC)));
        for (auto handler : _handlers)
        {
            if (auto res = handler(memory, current); res != TickResult::InvalidInstruction)
                return res;
        }

        return TickResult::InvalidInstruction;
    }
    catch (std::out_of_range const&)
    {
        return TickResult::MemoryOutOfRange;
    }
}
