// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Common.hh>
#include <simple-mips-emu/Memory.hh>

#include <algorithm>

bool Address::Parse(char const* begin, char const* end, Address& out) noexcept
{
    uint32_t word;
    if (!ParseWord(begin, end, word))
        return false;

    out = MakeFromWord(word);
    return true;
}

std::vector<uint8_t>& Memory::GetSegmentByBase(Address::BaseType base)
{
    if (base == Address::BaseType::Text)
        return _text;
    else if (base == Address::BaseType::Data)
        return _data;
    else
        throw std::invalid_argument { "Invalid address" };
}

std::vector<uint8_t> const& Memory::GetSegmentByBase(Address::BaseType base) const
{
    if (base == Address::BaseType::Text)
        return _text;
    else if (base == Address::BaseType::Data)
        return _data;
    else
        throw std::invalid_argument { "Invalid address" };
}

Memory::Memory(uint32_t textSize, uint32_t dataSize) :
    _registerFile {},
    _text(static_cast<size_t>(textSize), 0),
    _data(static_cast<size_t>(dataSize), 0),
    _textSize { textSize },
    _dataSize { dataSize }
{
    std::fill(std::begin(_registerFile), std::end(_registerFile), 0);
    _registerFile[PC] = Address::MakeText(0);
}

Memory::Memory(std::vector<uint8_t>&& text, std::vector<uint8_t>&& data) noexcept :
    _registerFile {},
    _text(std::move(text)),
    _data(std::move(data)),
    _textSize { static_cast<uint32_t>(_text.size()) },
    _dataSize { static_cast<uint32_t>(_data.size()) }
{
    std::fill(std::begin(_registerFile), std::end(_registerFile), 0);
    _registerFile[PC] = Address::MakeText(0);
}

bool Memory::IsTerminated() const noexcept
{
    return GetRegister(PC) >= static_cast<uint32_t>(Address::MakeText(_textSize));
}

void Memory::AdvancePC() noexcept
{
    SetRegister(PC, GetRegister(PC) + 4);
}

void Memory::Load(Address::BaseType base, std::vector<uint8_t> const& data) noexcept
{
    auto& segment = GetSegmentByBase(base);
    std::copy_n(data.begin(), std::min(data.size(), segment.size()), segment.begin());
}

uint32_t Memory::GetRegister(uint32_t registerIdx) const
{
    return _registerFile.at(registerIdx);
}

void Memory::SetRegister(uint32_t registerIdx, uint32_t newValue)
{
    if (registerIdx != 0)
        _registerFile.at(registerIdx) = newValue;
}

uint8_t Memory::GetByte(Address address) const noexcept
{
    auto& segment = GetSegmentByBase(address.base);
    if (address.offset < segment.size())
        return segment[address.offset];
    else
        return 0;
}

void Memory::SetByte(Address address, uint8_t byte)
{
    GetSegmentByBase(address.base).at(address.offset) = byte;
}

uint32_t Memory::GetWord(Address address) const noexcept
{
    auto& segment = GetSegmentByBase(address.base);
    if (static_cast<size_t>(address.offset) + 3 >= segment.size())
        return 0;

    uint32_t       rtn = 0;
    uint8_t const* ptr = std::addressof(segment[address.offset]);

    rtn |= static_cast<uint32_t>(ptr[0]) << 24;
    rtn |= static_cast<uint32_t>(ptr[1]) << 16;
    rtn |= static_cast<uint32_t>(ptr[2]) << 8;
    rtn |= static_cast<uint32_t>(ptr[3]) << 0;

    return rtn;
}

void Memory::SetWord(Address address, uint32_t word)
{
    auto& segment = GetSegmentByBase(address.base);
    if (static_cast<size_t>(address.offset) + 3 > segment.size())
        throw std::out_of_range { "address out of range" };

    uint8_t* ptr = std::addressof(segment[address.offset]);

    ptr[0] = static_cast<uint8_t>(word >> 24 & 0xFF);
    ptr[1] = static_cast<uint8_t>(word >> 16 & 0xFF);
    ptr[2] = static_cast<uint8_t>(word >> 8 & 0xFF);
    ptr[3] = static_cast<uint8_t>(word >> 0 & 0xFF);
}

void Memory::DumpRegisters(std::ostream& os) const
{
    std::ios_base::fmtflags flags = os.flags();

    os << "Current register values:\n";
    os << "------------------------------------\n";
    os << "PC: 0x" << std::hex << GetRegister(PC) << '\n';
    os << "Registers:\n";

    for (uint32_t idx = 0; idx < NumRegisters; ++idx)
    {
        os << "R" << std::dec << idx << ": 0x" << std::hex << GetRegister(idx) << '\n';
    }

    os.flags(flags);
}

void Memory::DumpMemory(std::ostream& os, Address start, Address end) const
{
    if (static_cast<uint32_t>(start) > static_cast<uint32_t>(end))
        throw std::invalid_argument { "invalid memory range" };

    std::ios_base::fmtflags flags = os.flags();

    os << std::hex;
    os << "Memory content [" << start << ".." << end << "]:\n";
    os << "------------------------------------\n";

    for (uint32_t current = start; current <= end; current += 4)
    {
        Address address = Address::MakeFromWord(current);
        os << address << ": 0x" << GetWord(address) << '\n';
    }

    os.flags(flags);
}
