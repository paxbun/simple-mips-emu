// Copyright (c) 2021 Chanjung Kim. All rights reserved.
// Licensed under the MIT License.

#include <simple-mips-emu/Memory.hh>

#include <iostream>

int main(int argc, char* argv[])
{
    Memory memory { 20, 20 };

    memory.DumpRegisters(std::cout);
    memory.DumpMemory(std::cout, Address::MakeData(0), Address::MakeData(16));

    return 0;
}