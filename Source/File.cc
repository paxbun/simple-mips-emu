// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/File.hh>

#include <charconv>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

FileReadResult ReadFile(std::filesystem::path const& path)
{
    if (fs::is_directory(path))
        return CannotRead { FileReadError::Type::GivenPathIsDirectory };

    std::ifstream ifs { path };
    if (!ifs)
        return CannotRead { FileReadError::Type::FileDoesNotExist };

    return ReadFile(ifs);
}

FileReadResult ReadFile(std::istream& is)
{
    std::vector<uint32_t> words;

    std::string line;
    uint32_t    value;
    while (std::getline(is, line))
    {
        if (line.size() < 3)
            return CannotRead { FileReadError::Type::InvalidFormat };

        if (line[0] != '0' || line[1] != 'x')
            return CannotRead { FileReadError::Type::InvalidFormat };

        auto result = std::from_chars(
            std::addressof(line[2]), std::addressof(line[line.size()]), value, 16);

        if (result.ec != std::errc {})
            return CannotRead { FileReadError::Type::InvalidFormat };
    }

    if (words.size() < 2)
        return CannotRead { FileReadError::Type::SectionSizeDoesNotMatch };

    if (words[0] + words[1] != static_cast<uint32_t>(words.size() - 2))
        return CannotRead { FileReadError::Type::SectionSizeDoesNotMatch };

    return CanRead {
        std::vector(words.begin() + 2, words.begin() + 2 + words[0]),
        std::vector(words.end() - words[1], words.end()),
    };
}