// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Memory.hh>

#include <regex>
#include <string>

bool Address::Parse(char const* begin, char const* end, Address& out) noexcept
{
    std::regex  re { "^0x([0-9a-dA-D]+)$" };
    std::cmatch match;

    if (!std::regex_match(begin, end, match, re))
        return false;

    try
    {
        uint64_t u = std::stoull(match[1].str(), nullptr, 16);
        if (static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) < u)
            return false;

        uint32_t offset = static_cast<uint32_t>(u);
        if (static_cast<uint32_t>(Address::BaseType::Data) <= offset)
            out = Address::MakeData(offset - static_cast<uint32_t>(Address::BaseType::Data));
        else
            out = Address::MakeText(offset - static_cast<uint32_t>(Address::BaseType::Text));

        return true;
    }
    catch (...)
    {
        return false;
    }
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
    _registerFile.back() = Address::MakeText(0);
}

bool Memory::IsTerminated() const
{
    return GetRegister(32) == static_cast<uint32_t>(Address::MakeText(_textSize));
}

void Memory::Load(Address::BaseType base, std::vector<uint8_t> const& data)
{
    auto& segment = GetSegmentByBase(base);
    std::copy_n(data.begin(), std::min(data.size(), segment.size()), segment.begin());
}

uint32_t Memory::GetRegister(uint8_t registerIdx) const
{
    return _registerFile.at(registerIdx);
}

void Memory::SetRegister(uint8_t registerIdx, uint32_t newValue)
{
    if (registerIdx != 0)
        _registerFile.at(registerIdx) = newValue;
}

uint8_t Memory::GetByte(Address address) const
{
    return GetSegmentByBase(address.base).at(address.offset);
}

void Memory::SetByte(Address address, uint8_t byte)
{
    GetSegmentByBase(address.base).at(address.offset) = byte;
}

uint32_t Memory::GetWord(Address address) const
{
    auto& segment = GetSegmentByBase(address.base);
    if (static_cast<size_t>(address.offset) + 3 >= segment.size())
        throw std::out_of_range { "address out of range" };

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
    os << "PC: 0x" << std::hex << GetRegister(32) << '\n';
    os << "Registers:\n";

    for (uint32_t idx = 0; idx < 32; ++idx)
    {
        os << "R" << std::dec << idx << ": 0x" << std::hex << GetRegister(idx) << '\n';
    }

    os.flags(flags);
}

void Memory::DumpMemory(std::ostream& os, Address start, Address end) const
{
    if (start.base != end.base)
        throw std::invalid_argument { "invalid memory range" };

    if (start.offset > end.offset)
        throw std::invalid_argument { "invalid memory range" };

    std::ios_base::fmtflags flags = os.flags();

    os << std::hex;
    os << "Memory content [" << start << ".." << end << "]:\n";
    os << "------------------------------------\n";

    for (uint32_t offset = start.offset; offset < end.offset; offset += 4)
    {
        Address current { start.base, offset };
        os << current << ": 0x" << GetWord(current) << '\n';
    }

    os.flags(flags);
}
