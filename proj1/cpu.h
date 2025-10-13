#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class AluCtrl { Add, Sub, Or, And, SLT, SRA, Second };

enum class PcOp { Inc, BNE, Jmp };

enum class RdTake { Alu, Mem, Pc4 };

enum class AluOp { Add, Sub, Second, R, I };

int alu(int arg1, int arg2, AluCtrl op);

struct Control {
    PcOp pc_take;
    bool mem_read;  // should we read from mem?
    RdTake rd_take;
    AluOp alu_op;
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
    AluCtrl alu_ctrl() const;
    int mem_amt() const;

    vector<int> reg_vals() const;  // returns rs1, rs2, and rd respectively
    int read_reg(int r) const;
    void set_reg(int r, int v);

    int read_mem(int addr, int bytes) const;
    void set_mem(int word, int addr, int bytes);
};
