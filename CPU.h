#pragma once

#include <iostream>

#include "Memory.h"
#include "Cache.h"

struct Instruction {
    uint32_t raw_instr;
    Instruction(uint32_t instr) : raw_instr(instr) {}
    uint32_t opcode() {
        return raw_instr & 0x7F;
    }
    uint32_t rd() {
        return (raw_instr >> 7) & 0x1F;
    }
    uint32_t funct3() {
        return (raw_instr >> 12) & 0x7;
    }
    uint32_t rs1() {
        return (raw_instr >> 15) & 0x1F;
    }
    uint32_t rs2() {
        return (raw_instr >> 20) & 0x1F;
    }
    uint32_t funct7() {
        return (raw_instr >> 25) & 0x7F;
    }
    int32_t immI() {
        return (
            ((int32_t)((raw_instr >> 20) & 0xFFF) << 20) >> 20
        );
    }
    int32_t immB() {
        uint32_t imm = 0;
        imm |= ((raw_instr >> 8) & 0xF) << 1;
        imm |= ((raw_instr >> 25) & 0x3F) << 5;
        imm |= ((raw_instr >> 7) & 0x1) << 11;
        imm |= ((raw_instr >> 31) & 0x1) << 12;
        return ((int32_t)(imm << 19) >> 19);
    }
    int32_t immS() {
        uint32_t imm = 0;
        imm |= ((raw_instr >> 7) & 0x1F);
        imm |= ((raw_instr >> 25) & 0x7F) << 5;
        return (int32_t)(imm << 20) >> 20;
    }
    int32_t immU() {
        return (raw_instr & 0xFFFFF000) >> 12;
    }
    int32_t immJ() {
        uint32_t imm = 0;
        imm |= ((raw_instr >> 21) & 0x3FF) << 1;
        imm |= ((raw_instr >> 20) & 0x1) << 11;
        imm |= ((raw_instr >> 12) & 0xFF) << 12;
        imm |= ((raw_instr >> 31) & 0x1) << 20;
        return ((int32_t)(imm << 11)) >> 11;
    }

};

class CPU {
public:
    CPU(Cache& ram)
    : RAM(ram) {}
    CPU(std::istream& in, Cache& ram)
    : RAM(ram)
    , pc(0) {
        for (int i = 0; i < 32; ++i) {
            registers[i] = 0;
        }

        for (int j = 0; j < 4; ++j) {
            uint8_t byte;
            in.read((char*)&byte, sizeof(byte));
            pc |= (static_cast<int32_t>(byte) << (8 * j));
        }
        for (int i = 1; i < 32; ++i) {
            for (int j = 0; j < 4; ++j) {
                char byte;
                in.read(&byte, sizeof(unsigned char));
                registers[i] |= (byte << (8 * j));
            }
        }
        ra = registers[1];
    }
    void Execute(Instruction instruction);
    void ExecuteRType(Instruction instruction);
    void ExecuteIType(Instruction instruction);
    void ExecuteLoad(Instruction instruction);
    void ExecuteStore(Instruction instruction);
    void ExecuteBranch(Instruction instruction);
    void Work();
    void dump();

    int32_t SignExtension(int32_t num, int size) {
        return ((num << (32 - size)) >> (32 - size));
    }
private:
    int32_t registers[32];
    int32_t pc;
    int32_t ra;
    Cache& RAM;
};