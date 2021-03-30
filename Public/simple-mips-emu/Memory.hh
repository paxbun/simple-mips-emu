// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#ifndef SIMPLE_MIPS_EMU_MEMORY_HH
#define SIMPLE_MIPS_EMU_MEMORY_HH

#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

/// <summary>
/// Number of registers except for PC
/// </summary>
constexpr size_t NumRegisters = 32;

/// <summary>
/// Represents an address in the memory.
/// </summary>
struct Address
{
    enum class BaseType : uint32_t
    {
        Text = 0x400000,
        Data = 0x10000000
    };

    constexpr static Address MakeText(uint32_t offset) noexcept
    {
        return Address { Address::BaseType::Text, offset };
    }

    constexpr static Address MakeData(uint32_t offset) noexcept
    {
        return Address { Address::BaseType::Data, offset };
    }

    BaseType base;
    uint32_t offset;

    constexpr void MoveToNext() noexcept
    {
        offset += 4;
    }

    constexpr operator uint32_t() const noexcept
    {
        return static_cast<uint32_t>(base) + offset;
    }
};

/// <summary>
/// Memory represents a state of the device at the specific time point.
/// </summary>
class Memory
{
  private:
    std::array<uint32_t, NumRegisters + 1> _registerFile;
    std::vector<uint8_t>                   _text;
    std::vector<uint8_t>                   _data;
    size_t                                 _textSize, _dataSize;

  public:
    size_t GetTextSize() const
    {
        return _textSize;
    }

    size_t GetDataSize() const
    {
        return _dataSize;
    }

  private:
    std::vector<uint8_t>&       GetSegmentByBase(Address::BaseType base);
    std::vector<uint8_t> const& GetSegmentByBase(Address::BaseType base) const;

  public:
    Memory(uint32_t textSize, uint32_t dataSize);
    Memory(Memory const&)     = default;
    Memory(Memory&&) noexcept = default;
    Memory& operator=(Memory const&) = default;
    Memory& operator=(Memory&&) noexcept = default;

  public:
    /// <summary>
    /// Loads data to the given segment.
    /// </summary>
    void Load(Address::BaseType base, std::vector<uint8_t> const& data);

    /// <summary>
    /// Returns the value of the given register. Note that R32 is PC.
    /// </summary>
    uint32_t GetRegister(uint8_t registerIdx) const;

    /// <summary>
    /// Assign the given word to the given reigster. Note that R32 is PC.
    /// </summary>
    void SetRegister(uint8_t registerIdx, uint32_t newValue);

    /// <summary>
    /// Returns the byte at the given address.
    /// </summary>
    uint8_t GetByte(Address address) const;

    /// <summary>
    /// Assign the given byte to the given memory location.
    /// </summary>
    void SetByte(Address address, uint8_t byte);

    /// <summary>
    /// Returns the word at the given address in big endian.
    /// </summary>
    uint32_t GetWord(Address address) const;

    /// <summary>
    /// Assign the given word to the given memory location. Note that the word is interpreted in big
    /// endian format.
    /// </summary>
    void SetWord(Address address, uint32_t word);
};

#endif