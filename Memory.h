#pragma once

#include <vector>
#include <iostream>

#include "Constants.h"

class Memory {
public:
    Memory() {
        memory = std::vector<unsigned char>(Constants::MEMORY_SIZE);
    }

    void Write4Bytes(size_t address, uint32_t value) {
        memory[address] = (unsigned char)(value & (0xFF));
        memory[address + 1] = (unsigned char)((value >> 8) & (0xFF));
        memory[address + 2] = (unsigned char)((value >> 16)& (0xFF));
        memory[address + 3] = (unsigned char)((value >> 24) & (0xFF));
    }

    size_t Read4Bytes(size_t address) {
        size_t ans = 0;
        ans |= (size_t)memory[address];
        ans |= ((size_t)memory[address + 1] << 8);
        ans |= ((size_t)memory[address + 2] << 16);
        ans |= ((size_t)memory[address + 3] << 24);
        return ans;
    }

    size_t ReadByte(size_t address) {
        return (size_t)memory[address];
    }

    size_t Read2Byte(size_t address) {
        size_t ans = 0;
        ans |= (size_t)memory[address];
        ans |= ((size_t)memory[address + 1] << 8);
        return ans;
    }

    void LoadBinary(std::istream& in) {
        while (in.peek() != EOF) {
            uint32_t address = 0;
            uint32_t size = 0;
            in.read(reinterpret_cast<char*>(&address), sizeof(address));
            in.read(reinterpret_cast<char*>(&size), sizeof(size));
            in.read(reinterpret_cast<char*>(&memory[address]), size);
        }
    }

private:
    std::vector<unsigned char> memory;    
};