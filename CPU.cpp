#include "CPU.h"

void CPU::Execute(Instruction instruction) {
    if (instruction.rd() == 0) {
        return;
    }
    switch (instruction.opcode()) {
    case 0b0110011:
        ExecuteRType(instruction);
        break;
    case 0b0010011:
        ExecuteIType(instruction);
        break;
    case 0b0000011:
        ExecuteLoad(instruction);
        break;
    case 0b0100011:
        ExecuteStore(instruction);
        break;
    case 0b1100011:
        ExecuteBranch(instruction);
        break;
    }
}

void CPU::ExecuteRType(Instruction instruction) {
    uint32_t rd = instruction.rd();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();
    switch (instruction.funct7()) {
    case 0x00:
        switch (instruction.funct3())
        {
        case 0x0: // ADD
            registers[rd] = registers[rs1] + registers[rs2];
            break;
        case 0x1: // SHIFT LEFT LOGICAL
            registers[rd] = (registers[rs1] << registers[rs2]);
            break;
        case 0x2: // SET LESS THAN
            registers[rd] = ((int32_t)registers[rs1] < (int32_t)registers[rs2] ? 1 : 0);
            break;
        case 0x3: // SET LESS THAN (U)
            registers[rd] = (registers[rs1] < registers[rs2] ? 1 : 0);
            break;
        case 0x4: // XOR
            registers[rd] = registers[rs1] ^ registers[rs2];
            break;
        case 0x5: // SHIFT RIGHT LOGICAL
            registers[rd] = (registers[rs1] >> registers[rs2]);
            break;
        case 0x6: // OR
            registers[rd] = registers[rs1] | registers[rs2];
            break;
        case 0x7: // AND
            registers[rd] = registers[rs1] & registers[rs2];
            break;
        }
        break;
    case 0x01:
        switch (instruction.funct3())
        {
        case 0x0: // MUL
            registers[rd] = int32_t(registers[rs1]) * int32_t(registers[rs2]);
            break;
        case 0x1: // MUL High
            int64_t result = int64_t(int32_t(registers[rs1])) * int64_t(int32_t(registers[rs2]));
            registers[rd] = uint32_t(result >> 32);
            break;
        case 0x2: // MUL High (S) (U)
            int64_t result = int64_t(int32_t(registers[rs1])) * uint64_t(registers[rs2]);
            registers[rd] = uint32_t(result >> 32);
            break;
        case 0x3: // MUL High (U)
            uint64_t result = uint64_t(registers[rs1]) * uint64_t(registers[rs2]);
            registers[rd] = uint32_t(result >> 32);
            break;
        case 0x4: // DIV
            registers[rd] = int32_t(registers[rs1]) / int32_t(registers[rs2]);
            break;
        case 0x5: // DIV (U)
            registers[rd] = registers[rs1] / registers[rs2];
            break;
        case 0x6: // Remainder
            registers[rd] = int32_t(registers[rs1]) % int32_t(registers[rs2]);
            break;
        case 0x7: // Remainder (U)
            registers[rd] = registers[rs1] % registers[rs2];
            break;
        }
        break;
    case 0x20:
        switch (instruction.funct3())
        {
        case 0x0: // SUB
            registers[rd] = registers[rs1] - registers[rs2];
            break;
        case 0x5: // SHIFT RIGHT ARITHMETIC
            registers[rd] = (int32_t(registers[rs1]) >> registers[rs2]);
            break;
        }
        break;
    }
}

void CPU::ExecuteIType(Instruction instruction) {
    int32_t imm = instruction.immI();
    uint32_t rd = instruction.rd();
    uint32_t rs1 = instruction.rs1();
    switch (instruction.funct3())
    {
    case 0x0: // ADD
        registers[rd] = registers[rs1] + imm;
        break;
    case 0x1: // SHIFT LEFT LOGICAL
        registers[rd] = registers[rs1] << imm;
        break;
    case 0x2: // SET LESS THAN IMM
        break;
    case 0x3: // SET LESS THAN IMM (U)
        break;
    case 0x4: // XOR
        registers[rd] = registers[rs1] ^ imm;
        break;
    case 0x5:
        if (true) {} // SHIFT RIGHT LOGICAL IMM
        else {} // SHIFT LEFT LOGICAL IMM
        break;
    case 0x6: // OR
        registers[rd] = registers[rs1] | imm;
        break;
    case 0x7: // AND
        registers[rd] = registers[rs1] & imm;
        break;
    }
}

void CPU::ExecuteLoad(Instruction instruction) {
    int32_t imm = instruction.immI();
    uint32_t rd = instruction.rd();
    uint32_t rs1 = instruction.rs1();
    switch (instruction.funct3())
    {
    case 0x0:
        registers[rd] = SignExtension(RAM.ReadByte(rs1 + imm), 8);
        break;
    case 0x1:
        registers[rd] = SignExtension(RAM.Read2Bytes(rs1 + imm), 16);
        break;
    case 0x2:
        registers[rd] = SignExtension(RAM.Read4Bytes(rs1 + imm), 32);
        break;
    case 0x4: // UNSIGNED
        registers[rd] = RAM.Read2Bytes(rs1 + imm);
        break;
    case 0x5: // UNSIGNED
        registers[rd] = RAM.Read4Bytes(rs1 + imm);
        break;
    }
}

void CPU::ExecuteStore(Instruction instruction) {
    int32_t imm = instruction.immI();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();

    switch (instruction.funct3()) {
    case 0x0: // SB
        RAM.WriteByte(rs1 + imm, (uint8_t)registers[rs2]);
        break;
    case 0x1: // SH
        RAM.Write2Bytes(rs1 + imm, (uint16_t)registers[rs2]);
        break;
    case 0x2: // SW
        RAM.Write4Bytes(rs1 + imm, (uint32_t)registers[rs2]);
        break;
    }
}

void CPU::ExecuteBranch(Instruction instruction) {
    int32_t imm = instruction.immI();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();

    switch (instruction.funct3()) {
    case 0x0: // BEQ
        if (registers[rs1] == registers[rs2]) {
            pc += imm;
        }
        break;
    case 0x1: // BNE
        if (registers[rs1] != registers[rs2]) {
            pc += imm;
        }
        break;
    case 0x4: // BLT
        if ((int32_t)registers[rs1] < (int32_t)registers[rs2]) {
            pc += imm;
        }
        break;
    case 0x5: // BGE
        if ((int32_t)registers[rs1] >= (int32_t)registers[rs2]) {
            pc += imm;
        }
        break;
    case 0x6: // BLTU
        if (registers[rs1] < registers[rs2]) {
            pc += imm;
        }
        break;
    case 0x7: // BGEU
        if (registers[rs1] >= registers[rs2]) {
            pc += imm;
        }
        break;
    }
}
