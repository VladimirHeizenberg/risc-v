#include "CPU.h"

#include <bitset>
// Дай Бог чтоб работало

void CPU::Execute(Instruction instruction) {
    // std::cout << pc << "pc : " <<  std::bitset<7>(instruction.opcode()) << "\n" << std::dec;
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
    case 0b0110111:
        registers[instruction.rd()] = (instruction.immU() << 12);
        break;
    case 0b0010111:
        registers[instruction.rd()] =  pc + (instruction.immU() << 12);
        break;
    case 0b1101111:
        registers[instruction.rd()] = pc + 4;
        pc += instruction.immJ();
        return;
    case 0b1100111:
        if (instruction.funct3() == 0x0) {
            uint32_t next_pc = (registers[instruction.rs1()] + instruction.immI()) & ~1;
            registers[instruction.rd()] = pc + 4;
            pc = next_pc;
            return;
        }
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize opcode: " + 
            std::bitset<7>(instruction.opcode()).to_string()
        );
        break;
    }
    pc += 4;
    registers[0] = 0;
}

void CPU::ExecuteRType(Instruction instruction) {
    uint32_t rd = instruction.rd();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();
    
    // std::cout << pc << "[R-Type] " << std::hex << instruction.raw_instr << " " << instruction.funct7() << " " << instruction.funct3() << "-" << std::dec;
    
    switch (instruction.funct7()) {
    case 0x00:
        switch (instruction.funct3()) {
        case 0x0: // ADD
            registers[rd] = registers[rs1] + registers[rs2];
            break;
        case 0x1: // SLL
            registers[rd] = registers[rs1] << registers[rs2];
            break;
        case 0x2: // SLT
            registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
            break;
        case 0x3: // SLTU
            registers[rd] = uint32_t(registers[rs1]) < uint32_t(registers[rs2]) ? 1 : 0;
            break;
        case 0x4: // XOR
            registers[rd] = registers[rs1] ^ registers[rs2];
            break;
        case 0x5: // SRL
            registers[rd] = registers[rs1] >> registers[rs2];
            break;
        case 0x6: // OR
            registers[rd] = registers[rs1] | registers[rs2];
            break;
        case 0x7: // AND
            registers[rd] = registers[rs1] & registers[rs2];
            break;
        default:
            throw std::runtime_error(
                "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
                "| didn't manage to recognize funct3: " + 
                std::bitset<7>(instruction.funct3()).to_string()
            );
            break;
        }
        break;
        
    case 0x20:
        switch (instruction.funct3()) {
        case 0x0: // SUB
            registers[rd] = registers[rs1] - registers[rs2];
            break;
        case 0x5: // SRA
            registers[rd] = (registers[rs1] >> registers[rs2]);
            break;
        default:
            throw std::runtime_error(
                "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
                "| didn't manage to recognize funct3: " + 
                std::bitset<7>(instruction.funct3()).to_string()
            );
            break;
        }
        break;

    case 0x01:
        int64_t result;
        uint64_t result1;
        switch (instruction.funct3()) {
        case 0x0: // MUL
            registers[rd] = registers[rs1] * registers[rs2];
            break;
        case 0x1: // MULH
            result = int64_t(registers[rs1]) * int64_t(registers[rs2]);
            registers[rd] = int32_t(result >> 32);
            break;
        case 0x2: // MULHSU
            result = int64_t(registers[rs1]) * uint64_t(registers[rs2]);
            registers[rd] = uint32_t(result >> 32);
            break;
        case 0x3: // MULHU
            result1 = uint64_t(registers[rs1]) * uint64_t(registers[rs2]);
            registers[rd] = int32_t(result1 >> 32);
            break;
        case 0x4: // DIV
            if (registers[rs2] == 0) registers[rd] = -1;
            else registers[rd] = registers[rs1] / registers[rs2];
            break;
        case 0x5: // DIVU
            if (registers[rs2] == 0) registers[rd] = -1;
            else registers[rd] = uint32_t(registers[rs1]) / uint32_t(registers[rs2]);
            break;
        case 0x6: // REM
            if (registers[rs2] == 0) registers[rd] = registers[rs1];
            else registers[rd] = registers[rs1] % registers[rs2];
            break;
        case 0x7: // REMU
            if (registers[rs2] == 0) registers[rd] = registers[rs1];
            else registers[rd] = uint32_t(registers[rs1]) % uint32_t(registers[rs2]);
            break;
        default:
            throw std::runtime_error(
                "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
                "| didn't manage to recognize funct3: " + 
                std::bitset<7>(instruction.funct3()).to_string()
            );
            break;
        }
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize funct7: " + 
            std::bitset<7>(instruction.funct7()).to_string()
        );
        break;
    }
}


void CPU::ExecuteIType(Instruction instruction) {
    int32_t imm = instruction.immI();
    uint32_t rd = instruction.rd();
    uint32_t rs1 = instruction.rs1();
    // std::cout << pc << "[I-Type] " << instruction.immI() << std::dec << "\n";
    switch (instruction.funct3())
    {
    case 0x0: // ADD
        registers[rd] = registers[rs1] + imm;
        break;
    case 0x1: // SHIFT LEFT LOGICAL
        registers[rd] = registers[rs1] << imm;
        break;
    case 0x2: // SET LESS THAN IMM
        registers[rd] = (registers[rs1] < imm ? 1 : 0);
        break;
    case 0x3: // SET LESS THAN IMM (U)
        registers[rd] = ((uint32_t)registers[rs1] < (uint32_t)imm ? 1 : 0);
        break;
    case 0x4: // XOR
        registers[rd] = registers[rs1] ^ imm;
        break;
    case 0x5:
        if ((instruction.raw_instr >> 30) & 0x1) {// SRAI
            registers[rd] = (registers[rs1] >> (imm & 0x1F));
        } else { // SRLI
            registers[rd] = (registers[rs1] >> (imm & 0x1F));
        }
        break;
    case 0x6: // OR
        registers[rd] = registers[rs1] | imm;
        break;
    case 0x7: // AND
        registers[rd] = registers[rs1] & imm;
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize funct3: " + 
            std::bitset<7>(instruction.funct3()).to_string()
        );
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
        registers[rd] = SignExtension(RAM.ReadByte(registers[rs1] + imm), 8);
        break;
    case 0x1:
        registers[rd] = SignExtension(RAM.Read2Bytes(registers[rs1] + imm), 16);
        break;
    case 0x2:
        registers[rd] = SignExtension(RAM.Read4Bytes(registers[rs1] + imm), 32);
        break;
    case 0x4: // UNSIGNED
        registers[rd] = RAM.Read2Bytes(registers[rs1] + imm);
        break;
    case 0x5: // UNSIGNED
        registers[rd] = RAM.Read4Bytes(registers[rs1] + imm);
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize funct3: " + 
            std::bitset<7>(instruction.funct3()).to_string()
        );
        break;
    }
}

void CPU::ExecuteStore(Instruction instruction) {
    int32_t imm = instruction.immS();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();
    // std::cout << "store: " << instruction.funct3() << "\n";

    switch (instruction.funct3()) {
    case 0x0: // STORE BYTE
        RAM.WriteByte(registers[rs1] + imm, (uint8_t)registers[rs2]);
        break;
    case 0x1: // STORE HALF
        RAM.Write2Bytes(registers[rs1] + imm, (uint16_t)registers[rs2]);
        break;
    case 0x2: // STORE WORD
        RAM.Write4Bytes(registers[rs1] + imm, (uint32_t)registers[rs2]);
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize funct3: " + 
            std::bitset<7>(instruction.funct3()).to_string()
        );
        break;
    }
}

void CPU::ExecuteBranch(Instruction instruction) {
    int32_t imm = instruction.immB();
    uint32_t rs1 = instruction.rs1();
    uint32_t rs2 = instruction.rs2();
    // std::cout << "branch command: " << pc <<" " << instruction.funct3() << " " << imm << "\n";
    // std::cout << registers[rs1] << " " << registers[rs2] << " " << rs1 << " " << rs2 << "\n";

    switch (instruction.funct3()) {
    case 0x0:
        if (registers[rs1] == registers[rs2]) {
            pc += imm - 4;
        }
        break;
    case 0x1:
        if (registers[rs1] != registers[rs2]) {
            pc += imm - 4;
        }
        break;
    case 0x4:
        if (registers[rs1] < registers[rs2]) {
            pc += imm - 4;
        }
        break;
    case 0x5:
        if (registers[rs1] >= registers[rs2]) {
            pc += imm - 4;
        }
        break;
    case 0x6: // UNSIGNED
        if ((uint32_t)registers[rs1] < (uint32_t)registers[rs2]) {
            pc += imm - 4;
        }
        break;
    case 0x7: // UNSIGNED
        if ((uint32_t)registers[rs1] >= (uint32_t)registers[rs2]) {
            pc += imm - 4;
        }
        break;
    default:
        throw std::runtime_error(
            "unknown command:" + std::bitset<32>(instruction.raw_instr).to_string() +
            "| didn't manage to recognize funct3: " + 
            std::bitset<7>(instruction.funct3()).to_string()
        );
        break;    
    }
}

void CPU::Work() {
    while (pc != ra) {
        // std::cout << pc << ": " << ra << " - ";
        Instruction instruction(RAM.Read4Bytes(pc, true));
        // std::cout << std::hex << instruction.raw_instr << std::dec << "\n";
        if (instruction.opcode() == 0b1110011) {
            if (instruction.immI() == 0x0) {
                pc = ra;
                break;
            } else if (instruction.immI() == 0x1) {
                pc = ra;
                break;
            }
        }
        Execute(instruction);
    }
}

void CPU::dump(std::ostream& out) {
    for (int i = 0; i < 32; ++i) {
        int32_t value = registers[i];
        char byte1 = (char)(value & (0xFF));
        char byte2 = (char)((value >> 8) & (0xFF));
        char byte3 = (char)((value >> 16) & (0xFF));
        char byte4 = (char)((value >> 24) & (0xFF));
        out.write(&byte1, sizeof(byte1));
        out.write(&byte2, sizeof(byte2));
        out.write(&byte3, sizeof(byte3));
        out.write(&byte4, sizeof(byte4));
    }
}