// Copyright (c) 2021 Chanjung Kim. All rights reserved.
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
    std::vector<uint32_t> text;
    std::vector<uint32_t> data;
};

struct CannotRead
{
    FileReadError error;
};

/// <summary>
/// The union of all possible return values of <c>ReadFile</c>
/// </summary>
using FileReadResult = std::variant<CanRead, CannotRead>;

/// <summary>
/// Reads an executable from the given path.
/// </summary>
FileReadResult ReadFile(std::filesystem::path const& path);

/// <summary>
/// Reads an executable from the given stream.
/// </summary>
FileReadResult ReadFile(std::istream& is);

#endif