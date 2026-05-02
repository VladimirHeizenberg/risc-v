#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "emulator/Memory.h"
#include "emulator/CPU.h"
#include "emulator/Constants.h"
#include "emulator/Cache.h"

int main(int argc, char** argv) {

    bool in_file = false;
    bool out_file = false;
    std::string input_file;
    std::string output_file;
    size_t address;
    size_t size;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-i") {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing argument for -i");
            }
            input_file = argv[++i];
            in_file = true;
        } else if (arg == "-o") {
            if (i + 3 >= argc) {
                throw std::runtime_error("Missing arguments for -o");
            }
            output_file = argv[++i];
            address = std::strtoul(argv[++i], nullptr, 0);
            size = std::strtoul(argv[++i], nullptr, 0);
            out_file = true;
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    if (!in_file) {
        throw std::runtime_error("Missing necessar argument -i");
    }
    

    Memory mem;
    std::ifstream fin(input_file, std::ios::binary);
    Cache cache(Policy::LRU, mem);
    CPU cpu(fin, cache);
    mem.LoadBinary(fin);
    cpu.Work();
    cache.invalidate_all();
    fin.close();

    std::cout << "replacement\thit rate\thit rate (inst)\thit rate (data)\n";
    auto hit_commands = cache.GetHitsCommands();
    auto hit_data = cache.GetHitsData();
    auto all_commands = cache.GetAllCommands();
    auto all_data = cache.GetAllData();
    double hit_rate = (float)(hit_commands + hit_data) / (all_commands + all_data) * 100;
    double instr_hit_rate = (float)hit_commands / all_commands * 100;
    double data_hit_rate = (float)hit_data / all_data * 100;
    printf("        LRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n", hit_rate, instr_hit_rate, data_hit_rate);

    Memory mem2;
    Cache cache2(Policy::bit_pLRU, mem2);
    std::ifstream fin2(input_file, std::ios::binary);
    CPU cpu2(fin2, cache2);
    mem2.LoadBinary(fin2);
    cpu2.Work();
    cache2.invalidate_all();
    fin2.close();

    hit_commands = cache2.GetHitsCommands();
    hit_data = cache2.GetHitsData();
    all_commands = cache2.GetAllCommands();
    all_data = cache2.GetAllData();
    hit_rate = (float)(hit_commands + hit_data) / (all_commands + all_data) * 100;
    instr_hit_rate = (float)hit_commands / all_commands * 100;
    data_hit_rate = (float)hit_data / all_data * 100;
    printf("      bpLRU\t%3.5f%%\t%3.5f%%\t%3.5f%%\n", hit_rate, instr_hit_rate, data_hit_rate);

    if (out_file) {
        std::ofstream out(output_file, std::ios::binary);
        cpu2.dump(out);
        mem2.dump(out, address, size);
    }
    // std::cout << "Instruction Hit: " << hit_commands << "\n";
    // std::cout << "Instruction Count: " << all_commands << "\n";
    // std::cout << "Data Hit: " << hit_data << "\n";
    // std::cout << "Data Count: " << all_data << "\n";
    // std::cout << "Overall Hit: " << hit_commands + hit_data << "\n";
    // std::cout << "Overall Count: " << all_commands + all_data << "\n";
}