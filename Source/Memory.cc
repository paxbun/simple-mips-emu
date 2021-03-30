// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Memory.hh>

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
    if (static_cast<size_t>(address.offset) + 3 > segment.size())
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
