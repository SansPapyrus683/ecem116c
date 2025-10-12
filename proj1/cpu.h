#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

constexpr int ALU_ADD = 0b000;
constexpr int ALU_SUB = 0b001;
constexpr int ALU_OR = 0b010;
constexpr int ALU_AND = 0b011;
constexpr int ALU_SLT = 0b100;
constexpr int ALU_SRA = 0b101;
constexpr int ALU_2ND = 0b110;

constexpr bitset<2> PC_INC = 0b00;
constexpr bitset<2> PC_BNE = 0b01;
constexpr bitset<2> PC_JMP = 0b10;

constexpr bitset<2> RD_ALU = 0b00;
constexpr bitset<2> RD_MEM = 0b01;
constexpr bitset<2> RD_PC4 = 0b10;

constexpr bitset<3> CTRL_ALU_ADD = 0b000;
constexpr bitset<3> CTRL_ALU_SUB = 0b001;
constexpr bitset<3> CTRL_ALU_2ND = 0b010;
constexpr bitset<3> CTRL_ALU_R = 0b011;
constexpr bitset<3> CTRL_ALU_I = 0b100;

int alu(int arg1, int arg2, int op);

struct Control {
    bitset<2> pc_take;
    bool mem_read;  // should we read from mem?
    bitset<2> rd_take;
    bitset<3> alu_op;
    bool mem_write;  // should we write to mem?
    bool alu_src;    // 0 = take from reg, 1 = take from imm
    bool reg_write;  // should anything get updated in the regfile?
};

class CPU {
   private:
    char mem[1 << 20] = {0};  // data memory byte addressable in little endian fashion
    const vector<bitset<32>>& instrs;
    unsigned long pc = 0;

   public:
    vector<int> reg = vector<int>(1 << 5);  // ?? why do i have to init it like this??
    CPU(const vector<bitset<32>>& instructions);
    unsigned long read_pc() const;
    void set_pc(int new_pc);

    Control ctrl_bits() const;
    int imm() const;
    int alu_ctrl() const;
    int mem_amt() const;

    vector<int> reg_vals() const;  // returns rs1, rs2, and rd respectively
    int read_reg(int r) const;
    void set_reg(int r, int v);

    int read_mem(int addr, int bytes) const;
    void set_mem(int word, int addr, int bytes);
};
