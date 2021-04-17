// Copyright (c) 2021 Chanjung Kim (paxbun). All rights reserved.
// Licensed under the MIT License.

#ifndef SIMPLE_MIPS_EMU_FILE_HH
#define SIMPLE_MIPS_EMU_FILE_HH

#include <cstdint>
#include <filesystem>
#include <variant>
#include <vector>

/// <summary>
/// Represents an error occurred when reading given files.
/// </summary>
struct FileReadError
{
    enum class Type
    {
        GivenPathIsDirectory,
        FileDoesNotExist,
        InvalidFormat,
        SectionSizeDoesNotMatch,
    };

    Type type;
};

struct CanRead
{
    uint32_t              textSize;
    uint32_t              dataSize;
    std::vector<uint32_t> text;
    std::vector<uint32_t> data;
};

struct CannotRead
{
    FileReadError error;
};

using FileReadResult = std::variant<CanRead, CannotRead>;

FileReadResult ReadFile(std::filesystem::path const& path);

#endif