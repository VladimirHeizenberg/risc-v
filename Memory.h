#pragma once

#include <vector>
#include <iostream>

#include "Constants.h"

class Memory {
public:
    Memory() {
        memory = std::vector<uint8_t>(Constants::MEMORY_SIZE);
    }

    void WriteByte(size_t address, uint8_t value) {
        memory[address] = value;
    }

    void Write2Bytes(size_t address, uint16_t value) {
        memory[address] = (uint8_t)(value & (0xFF));
        memory[address + 1] = (uint8_t)((value >> 8) & (0xFF));
    }

    void Write4Bytes(size_t address, uint32_t value) {
        memory[address] = (uint8_t)(value & (0xFF));
        memory[address + 1] = (uint8_t)((value >> 8) & (0xFF));
        memory[address + 2] = (uint8_t)((value >> 16)& (0xFF));
        memory[address + 3] = (uint8_t)((value >> 24) & (0xFF));
    }

    size_t ReadByte(size_t address) {
        return (size_t)memory[address];
    }

    size_t Read2Bytes(size_t address) {
        size_t ans = 0;
        ans |= (size_t)memory[address];
        ans |= ((size_t)memory[address + 1] << 8);
        return ans;
    }

    size_t Read4Bytes(size_t address) {
        size_t ans = 0;
        ans |= (size_t)memory[address];
        ans |= ((size_t)memory[address + 1] << 8);
        ans |= ((size_t)memory[address + 2] << 16);
        ans |= ((size_t)memory[address + 3] << 24);
        return ans;
    }

    std::vector<uint8_t> ReadBlock(size_t address, size_t size) {
        std::vector<uint8_t> result;
        for (int i = 0; i < size; ++i) {
            result.push_back(ReadByte(address + i));
        }
        return result;
    }

    void WriteBlock(size_t address, const std::vector<uint8_t>& data) {
        for (int i = 0; i < data.size(); ++i) {
            memory[address + i] = data[i];
        }
    }

    void dump() {
        for (int i = 0; i < 32; i = i + 4) {
            std::cout << std::bitset<32>((int32_t)Read4Bytes(i)) << " ";
        }
    }

    void LoadBinary(std::istream& in) {
        while (in.peek() != EOF) {
            // TODO: add serialization
            uint32_t address = 0;
            uint32_t size = 0;
            in.read(reinterpret_cast<char*>(&address), sizeof(address));
            in.read(reinterpret_cast<char*>(&size), sizeof(size));
            in.read(reinterpret_cast<char*>(&memory[address]), size);
        }
    }

private:
    std::vector<uint8_t> memory;    
};