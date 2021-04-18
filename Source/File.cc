// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/File.hh>

#include <charconv>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

std::vector<uint8_t> MakeBytesFromWords(std::vector<uint32_t>::iterator begin,
                                        std::vector<uint32_t>::iterator end)
{
    size_t numBytes { static_cast<size_t>(std::distance(begin, end) * 4) };

    std::vector<uint8_t> rtn(numBytes, 0);

    auto rtnIt = rtn.begin();
    for (auto it = begin; it != end; ++it)
    {
        uint32_t word = *it;
        // MIPS uses big-endian
        *(rtnIt++) = static_cast<uint8_t>((word & 0xFF000000) >> 24);
        *(rtnIt++) = static_cast<uint8_t>((word & 0x00FF0000) >> 16);
        *(rtnIt++) = static_cast<uint8_t>((word & 0x0000FF00) >> 8);
        *(rtnIt++) = static_cast<uint8_t>((word & 0x000000FF) >> 0);
    }

    return rtn;
}

}

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
        if (std::find_if(line.begin(), line.end(), [](char c) { return !isspace(c); })
            == line.end())
            continue;

        if (line.size() < 3)
            return CannotRead { FileReadError::Type::InvalidFormat };

        if (line[0] != '0' || line[1] != 'x')
            return CannotRead { FileReadError::Type::InvalidFormat };

        auto result = std::from_chars(
            std::addressof(line[2]), std::addressof(line[line.size()]), value, 16);

        if (result.ec != std::errc {})
            return CannotRead { FileReadError::Type::InvalidFormat };

        words.push_back(value);
    }

    if (words.size() < 2)
        return CannotRead { FileReadError::Type::SectionSizeDoesNotMatch };

    if (words[0] + words[1] != static_cast<uint32_t>(words.size() - 2))
        return CannotRead { FileReadError::Type::SectionSizeDoesNotMatch };

    return CanRead {
        MakeBytesFromWords(words.begin() + 2, words.begin() + 2 + words[0]),
        MakeBytesFromWords(words.end() - words[1], words.end()),
    };
}