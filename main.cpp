#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

#include "Memory.h"
#include "CPU.h"
#include "Constants.h"
#include "Cache.h"

int main(int argc, char** argv) {
    Memory mem;
    std::ifstream fin("tests/task.bin", std::ios::binary);
    Cache cache(Policy::LRU, mem);
    CPU cpu(fin, cache);
    mem.LoadBinary(fin);
    cpu.Work();
    cache.invalidate_all();
    // cpu.dump();
    // std::cout << "=============================\n";
    //         for (int i = 0x2000; i < 0x2000 + 4 * 100; i = i + 4) {
    //     std::cout << mem.Read4Bytes(i) << "\n" << std::dec;}
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
    std::ifstream fin2("tests/task.bin", std::ios::binary);
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
    // std::cout << "Instruction Hit: " << hit_commands << "\n";
    // std::cout << "Instruction Count: " << all_commands << "\n";
    // std::cout << "Data Hit: " << hit_data << "\n";
    // std::cout << "Data Count: " << all_data << "\n";
    // std::cout << "Overall Hit: " << hit_commands + hit_data << "\n";
    // std::cout << "Overall Count: " << all_commands + all_data << "\n";
}