// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Emulation.hh>
#include <simple-mips-emu/File.hh>
#include <simple-mips-emu/Memory.hh>

#include <charconv>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>

struct Range
{
    Address begin;
    Address end;
};

struct Options
{
    std::optional<Range>  range           = std::nullopt;
    bool                  dumpEachTick    = false;
    uint32_t              numInstructions = std::numeric_limits<uint32_t>::max();
    std::filesystem::path filePath {};
};

Options ParseCommandArgs(int argc, char* argv[])
{
    bool filePathGiven = false;

    Options options;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-m") == 0)
        {
            if (i == argc - 1)
                throw std::runtime_error { "Missing addresses after '-m'" };

            char const* input = argv[++i];

            Range range;
            auto  length   = strlen(input);
            auto  colonPos = strcspn(input, ":");

            if (length == colonPos)
                throw std::runtime_error { "Invalid address format" };

            if (!Address::Parse(input, input + colonPos, range.begin))
                throw std::runtime_error { "Invalid address format" };

            if (!Address::Parse(input + colonPos + 1, input + length, range.end))
                throw std::runtime_error { "Invalid address format" };

            options.range = range;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            if (options.dumpEachTick)
                throw std::runtime_error { "Duplicate option: '-d'" };
            options.dumpEachTick = true;
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            if (i == argc - 1)
                throw std::runtime_error { "Missing number of instructions after '-n'" };

            char const* input = argv[++i];

            auto result = std::from_chars(input, input + strlen(input), options.numInstructions);
            if (result.ec != std::errc {})
                throw std::runtime_error { "Invalid number of instructions" };
        }
        else
        {
            if (filePathGiven)
                throw std::runtime_error { "Multiple files are given" };
            filePathGiven    = true;
            options.filePath = argv[i];
        }
    }

    if (!filePathGiven)
        throw std::runtime_error { "No file is given" };

    return options;
}

Memory LoadMemory(Options const& options)
{
    FileReadResult fileResult { ReadFile(options.filePath) };
    if (std::holds_alternative<CannotRead>(fileResult))
    {
        CannotRead  error = std::get<CannotRead>(fileResult);
        char const* msg   = "Unknown file I/O error";
        switch (error.error.type)
        {
            case FileReadError::Type::FileDoesNotExist:
            {
                msg = "File does not exist";
                break;
            }
            case FileReadError::Type::GivenPathIsDirectory:
            {
                msg = "File is directory";
                break;
            }
            case FileReadError::Type::InvalidFormat:
            {
                msg = "Invalid file";
                break;
            }
            case FileReadError::Type::SectionSizeDoesNotMatch:
            {
                msg = "Section size does not match";
                break;
            }
        }

        throw std::runtime_error { msg };
    }

    CanRead file = std::get<CanRead>(fileResult);
    Memory  memory { std::move(file.text), std::move(file.data) };

    return memory;
}

void DumpMemory(Memory const& memory, Options const& options, std::ostream& stream)
{
    memory.DumpRegisters(stream);
    stream << '\n';
    if (options.range)
    {
        auto& range = options.range.value();
        memory.DumpMemory(stream, range.begin, range.end);
        stream << '\n';
    }
}

int main(int argc, char* argv[])
{
    try
    {
        std::ios::sync_with_stdio(false);

        Options options = ParseCommandArgs(argc, argv);
        Memory  memory  = LoadMemory(options);

        TickResult result = TickResult::Success;
        for (uint32_t i = 0; i < options.numInstructions && !memory.IsTerminated(); ++i)
        {
            result = Tick(memory);
            if (result != TickResult::Success)
                break;
            if (options.dumpEachTick)
                DumpMemory(memory, options, std::cout);
        }

        DumpMemory(memory, options, std::cout);
        return 0;
    }
    catch (std::exception const& ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
