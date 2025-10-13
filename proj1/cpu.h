#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using std::vector;

/** the operation that's actually fed to the alu */
enum class AluCtrl { Add, Sub, Or, And, SLT, SRA, Second };

/** what the pc should do after the current instruction */
enum class PcOp { Inc, BNE, Jmp };

/** where rd should get its value from */
enum class RdTake { Alu, Mem, Pc4 };

/** what the controller gives to the alu decoder */
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
    const vector<std::bitset<32>>& instrs;
    unsigned long pc = 0;

   public:
    vector<int> reg = vector<int>(1 << 5);  // ?? why do i have to init it like this??
    CPU(const vector<std::bitset<32>>& instructions);
    unsigned long read_pc() const;
    void set_pc(int new_pc);

    int read_reg(int r) const;
    void set_reg(int r, int v);
    int read_mem(int addr, int bytes) const;
    void set_mem(int word, int addr, int bytes);

    // all of these functions are given the instruction at the current pc
    Control ctrl_bits() const;
    int imm() const;               // the immediate if there is any
    AluCtrl alu_ctrl() const;      // the actual alu operation to perform
    int mem_amt() const;           // how much memory to read or write
    vector<int> reg_vals() const;  // returns rs1, rs2, and rd respectively
};
