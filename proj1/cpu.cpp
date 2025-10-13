#include "cpu.h"

#include <cassert>
#include <cstdint>
#include <vector>

using std::pair;
using std::vector;

int alu(int arg1, int arg2, AluCtrl op) {
    switch (op) {
        case AluCtrl::Add:
            return arg1 + arg2;
        case AluCtrl::Sub:
            return arg1 - arg2;
        case AluCtrl::Or:
            return arg1 | arg2;
        case AluCtrl::And:
            return arg1 & arg2;
        case AluCtrl::SLT:
            return (uint32_t)arg1 < (uint32_t)arg2;
        case AluCtrl::SRA:
            return arg1 >> arg2;
        case AluCtrl::Second:
            return arg2;
    }
    return 0;
}

CPU::CPU(const vector<bitset<32>>& instructions) : instrs(instructions) {}

unsigned long CPU::read_pc() const { return pc; }

void CPU::set_pc(int new_pc) { pc = new_pc; }

Control CPU::ctrl_bits() const {
    int opcode = 0;
    for (int i = 0; i <= 6; i++) { opcode |= instrs[pc][i] << i; }

    switch (opcode) {
        case 0b0110011:  // r
            return {PcOp::Inc, 0, RdTake::Alu, AluOp::R, 0, 0, 1};
        case 0b0110111:  // u
            return {PcOp::Inc, 0, RdTake::Alu, AluOp::Second, 0, 1, 1};
        case 0b0010011:  // i
            return {PcOp::Inc, 0, RdTake::Alu, AluOp::I, 0, 1, 1};
        case 0b0000011:  // load stuff (i)
            return {PcOp::Inc, 1, RdTake::Mem, AluOp::Add, 0, 1, 1};
        case 0b0100011:  // s
            return {PcOp::Inc, 0, RdTake::Alu, AluOp::Add, 1, 1, 0};
        case 0b1100011:  // b
            return {PcOp::BNE, 0, RdTake::Alu, AluOp::Sub, 0, 0, 0};
        case 0b1100111:  // jalr (i)
            return {PcOp::Jmp, 0, RdTake::Pc4, AluOp::Add, 0, 1, 1};
    }
    assert("bad opcode" && 0);
}

AluCtrl CPU::alu_ctrl() const {
    int funct3 = 0;
    for (int i = 0; i < 3; i++) { funct3 |= instrs[pc][12 + i] << i; }

    AluOp alu_op = ctrl_bits().alu_op;
    if (alu_op == AluOp::Add) {
        return AluCtrl::Add;
    } else if (alu_op == AluOp::Sub) {
        return AluCtrl::Sub;
    } else if (alu_op == AluOp::Second) {
        return AluCtrl::Second;
    }

    switch (funct3) {
        case 0x0:  // add or sub since we only have addi and sub
            return alu_op == AluOp::R ? AluCtrl::Sub : AluCtrl::Add;
        case 0x3:
            return AluCtrl::SLT;
        case 0x5:
            return AluCtrl::SRA;
        case 0x6:
            return AluCtrl::Or;
        case 0x7:
            return AluCtrl::And;
    }
    assert("invalid stuff for alu op" && false);
}

int CPU::mem_amt() const {
    int funct3 = 0;
    for (int i = 0; i < 3; i++) { funct3 |= instrs[pc][12 + i] << i; }

    if (funct3 == 0x4) {
        return 1;
    } else if (funct3 == 0x1) {
        return 2;
    } else if (funct3 == 0x2) {
        return 4;
    }
    assert("invalid funct3" && false);
}

int CPU::imm() const {
    int opcode = 0;
    for (int i = 0; i <= 6; i++) { opcode |= instrs[pc][i] << i; }

    int res = 0;
    int msb = 31;
    switch (opcode) {
        case 0b0110111:  // u
            for (int i = 12; i < 32; i++) { res |= instrs[pc][i] << i; }
            break;
        case 0b0010011:  // i
        case 0b0000011:  // load stuff (i)
        case 0b1100111:  // jalr (i)
            for (int i = 0; i < 12; i++) { res |= instrs[pc][20 + i] << i; }
            msb = 11;
            break;
        case 0b0100011:  // s
            for (int i = 0; i < 5; i++) { res |= instrs[pc][7 + i] << i; }
            for (int i = 5; i < 12; i++) { res |= instrs[pc][25 - 5 + i] << i; }
            msb = 11;
            break;
        case 0b1100011:  // b
            // worst immediate order oat
            vector<int> what{8, 9, 10, 11, 25, 26, 27, 28, 29, 30, 7, 31};
            // notice that we start at 1
            for (int i = 1; i <= what.size(); i++) {
                res |= instrs[pc][what[i - 1]] << i;
            }
            msb = 12;
            break;
    }
    bool sign_bit = res & (1 << msb);
    for (int i = msb + 1; i < 32; i++) { res |= sign_bit << i; }

    return res;  // any other type doesn't need the imm anyways
}

vector<int> CPU::reg_vals() const {
    vector<pair<int, int>> ranges{{15, 19}, {20, 24}, {7, 11}};
    vector<int> res(ranges.size());
    for (int i = 0; i < ranges.size(); i++) {
        const auto& [start, end] = ranges[i];
        for (int j = start; j <= end; j++) { res[i] |= instrs[pc][j] << (j - start); }
    }
    return res;
}

int CPU::read_reg(int r) const { return reg.at(r); }

void CPU::set_reg(int r, int v) {
    assert(0 <= r && r < 32);
    if (r > 0) { reg[r] = v; }
}

int CPU::read_mem(int addr, int bytes) const {
    uint32_t res = 0;
    for (int i = 0; i < bytes; i++) {
        uint32_t val = (unsigned int)(unsigned char)mem[addr++];
        res += val << (i * 8);
    }
    return res;
}

void CPU::set_mem(int word, int addr, int bytes) {
    for (int i = 0; i < bytes; i++) {
        mem[addr++] = word & 0xff;
        word >>= 8;
    }
}
