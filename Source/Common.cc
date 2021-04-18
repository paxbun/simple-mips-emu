// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Common.hh>

#include <limits>
#include <regex>
#include <string>

bool ParseWord(char const* begin, char const* end, uint32_t& out) noexcept
{
    std::regex  re { "^ *0x([0-9a-fA-F]+) *$" };
    std::cmatch match;

    if (!std::regex_match(begin, end, match, re))
        return false;

    try
    {
        uint64_t u = std::stoull(match[1].str(), nullptr, 16);
        if (static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()) < u)
            return false;

        out = static_cast<uint32_t>(u);
        return true;
    }
    catch (...)
    {
        return false;
    }
}
