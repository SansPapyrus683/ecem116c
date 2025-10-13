#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "cpu.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { return -1; }

    ifstream infile(argv[1]);  // open the file
    if (!(infile.is_open() && infile.good())) {
        cout << "error opening file" << endl;
        return 1;
    }
    infile >> hex;

    vector<char> bytes;
    int curr;
    while (infile >> curr) { bytes.push_back((char)curr); }

    vector<bitset<32>> instructions(bytes.size() / 4);
    for (int i = 0; i < bytes.size(); i += 4) {
        int at = 0;
        for (int j = 0; j < 4; j++) {
            char b = bytes[i + j];
            for (int k = 0; k < 8; k++) { instructions[i / 4].set(at++, b & (1 << k)); }
        }
    }

    CPU cpu(instructions);
    // processor's main loop. Each iteration is equal to one clock cycle.
    while (cpu.read_pc() < instructions.size()) {
        int curr_pc = cpu.read_pc();
        Control ctrl = cpu.ctrl_bits();
        int imm = cpu.imm();

        vector<int> reg = cpu.reg_vals();  // rs1, rs2, & rd respectively
        int rs1 = cpu.read_reg(reg[0]);  // read rs1 & rs2 first
        int rs2 = cpu.read_reg(reg[1]);

        // do the actual computation
        int arg1 = rs1;
        int arg2 = ctrl.alu_src ? imm : rs2;
        int alu_res = alu(arg1, arg2, cpu.alu_ctrl());

        // read into star_res or write depending on the controller
        int star_res = 0;
        if (ctrl.mem_read) { star_res = cpu.read_mem(alu_res, cpu.mem_amt()); }
        if (ctrl.mem_write) { cpu.set_mem(rs2, alu_res, cpu.mem_amt()); }

        // check which value we want to write to rd (if at all)
        if (ctrl.reg_write) {
            int val = 0;
            switch (ctrl.rd_take) {
                case RdTake::Alu:
                    val = alu_res;
                    break;
                case RdTake::Mem:
                    val = star_res;
                    break;
                case RdTake::Pc4:
                    val = curr_pc * 4 + 4;
                    break;
            }
            cpu.set_reg(reg[2], val);
        }

        // check pc_take to see how we should update the pc
        int next_pc = 0;
        switch (ctrl.pc_take) {
            case PcOp::Inc:
                next_pc = curr_pc + 1;
                break;
            case PcOp::BNE:
                next_pc = alu_res != 0 ? curr_pc + imm / 4 : curr_pc + 1;
                break;
            case PcOp::Jmp:
                next_pc = alu_res / 4;
                break;
        }
        cpu.set_pc(next_pc);
    }

    // a0 & a1 correspond to x10 & x11 respectively
    printf("(%i,%i)\n", cpu.read_reg(10), cpu.read_reg(11));
}
