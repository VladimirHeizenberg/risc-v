#pragma once

#include <iostream>

#include "Memory.h"

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
    uint32_t immI() {
        return (
            ((int32_t)((raw_instr >> 20) & 0xFFF) << 20) >> 20
        );
    }
};

class CPU {
public:
    void Execute(Instruction instruction);
    void ExecuteRType(Instruction instruction);
    void ExecuteIType(Instruction instruction);
    void ExecuteLoad(Instruction instruction);
    void ExecuteStore(Instruction instruction);
    void ExecuteBranch(Instruction instruction);
    void ExecuteMul(Instruction instruction);
private:
    uint32_t registers[32];
    Memory& RAM;
};