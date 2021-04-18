// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Emulation.hh>
#include <simple-mips-emu/Formats.hh>

namespace
{

template <typename T, typename... Ts>
inline bool IsOneOf(T value, Ts... ts) noexcept
{
    return ((static_cast<T>(ts) == value) || ...);
}

constexpr uint32_t SignExtend(uint32_t value, uint32_t numBits) noexcept
{
    // 0  if value >= 0
    // -1 otherwise
    uint32_t const mask = ~(value >> (numBits - 1)) + 1;
    return value | (mask << numBits);
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
    uint32_t const operation = (current >> 26) & 0b111111;
    if (IsOneOf(operation, IFormatOp::ADDIU, IFormatOp::ANDI, IFormatOp::ORI, IFormatOp::SLTIU))
    {
        // instruction is I format
        uint32_t const source      = (current >> 21) & 0b11111;
        uint32_t const destination = (current >> 16) & 0b11111;
        uint32_t const immediate   = (current >> 0) & 0xFFFF;

        uint32_t const sourceValue = memory.GetRegister(source);

        uint32_t destinationValue;
        switch (static_cast<IFormatOp>(operation))
        {
            case IFormatOp::ADDIU:
            {
                destinationValue = sourceValue + SignExtend(immediate, 16);
                break;
            }
            case IFormatOp::ANDI:
            {
                destinationValue = sourceValue & immediate;
                break;
            }
            case IFormatOp::ORI:
            {
                destinationValue = sourceValue | immediate;
                break;
            }
            case IFormatOp::SLTIU:
            {
                destinationValue
                    = static_cast<uint32_t>(static_cast<int32_t>(sourceValue)
                                            < static_cast<int32_t>(SignExtend(immediate, 16)));
                break;
            }
            default: return TickResult::InvalidInstruction;
        }
        memory.SetRegister(destination, destinationValue);
        memory.AdvancePC();

        return TickResult::Success;
    }
    else if (IsOneOf(operation, BIFormatOp::BEQ, BIFormatOp::BNE))
    {
        // instruction is BI format
        uint32_t const pc = memory.GetRegister(Memory::PC);

        uint32_t const source1 = (current >> 21) & 0b11111;
        uint32_t const source2 = (current >> 16) & 0b11111;
        uint32_t const offset  = SignExtend((current >> 0) & 0xFFFF, 16);

        uint32_t const source1Value = memory.GetRegister(source1);
        uint32_t const source2Value = memory.GetRegister(source2);

        if ((source1Value == source2Value)
            == (static_cast<BIFormatOp>(operation) == BIFormatOp::BEQ))
        {
            uint32_t const pcValue = memory.GetRegister(Memory::PC);
            // PC is not advanced yet
            uint32_t const newPcValue = pcValue + 4 + offset * 4;

            memory.SetRegister(Memory::PC, newPcValue);
        }
        else
        {
            memory.AdvancePC();
        }

        return TickResult::Success;
    }
    else if (IsOneOf(operation, IIFormatOp::LUI))
    {
        // instruction is II format

        uint32_t const destination = (current >> 16) & 0b11111;
        uint32_t const immediate   = (current >> 0) & 0xFFFF;

        memory.SetRegister(destination, immediate << 16);
        memory.AdvancePC();

        return TickResult::Success;
    }
    else if (IsOneOf(operation, OIFormatOp::LB, OIFormatOp::LW, OIFormatOp::SB, OIFormatOp::SW))
    {
        // instruction is OI format

        uint32_t const operand1 = (current >> 21) & 0b11111;
        uint32_t const operand2 = (current >> 16) & 0b11111;
        uint32_t const offset   = SignExtend((current >> 0) & 0xFFFF, 16);

        uint32_t const operand1Value = memory.GetRegister(operand1);
        Address const  address       = Address::MakeFromWord(operand1Value + offset);

        switch (static_cast<OIFormatOp>(operation))
        {
            case OIFormatOp::LB:
            {
                uint32_t const value = SignExtend(memory.GetByte(address), 8);
                memory.SetRegister(operand2, value);
                break;
            }
            case OIFormatOp::LW:
            {
                uint32_t const value = memory.GetWord(address);
                memory.SetRegister(operand2, value);
                break;
            }
            case OIFormatOp::SB:
            {
                uint32_t const value = memory.GetRegister(operand2);
                memory.SetByte(address, static_cast<uint8_t>(value & 0xFF));
                break;
            }
            case OIFormatOp::SW:
            {
                uint32_t const value = memory.GetRegister(operand2);
                memory.SetWord(address, value);
                break;
            }
            default: return TickResult::InvalidInstruction;
        }

        memory.AdvancePC();
        return TickResult::Success;
    }

    return TickResult::InvalidInstruction;
}

TickResult TickHandleJ(Memory& memory, uint32_t current)
{
    uint32_t const operation = (current >> 26) & 0b111111;
    uint32_t const pcValue   = memory.GetRegister(Memory::PC);
    uint32_t const target    = ((current & 0x03FFFFFF) << 2) | (pcValue & 0xF0000000);

    if (operation == static_cast<uint32_t>(JFormatOp::J))
    {
        memory.SetRegister(Memory::PC, target);
        return TickResult::Success;
    }
    else if (operation == static_cast<uint32_t>(JFormatOp::JAL))
    {
        memory.SetRegister(Memory::RA, pcValue + 4);
        memory.SetRegister(Memory::PC, target);
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
