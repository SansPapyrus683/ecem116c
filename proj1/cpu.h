#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class CPU {
   private:
    int mem[1 << 12];  // data memory byte addressable in little endian fashion;
    const vector<bitset<32>>& instr;
    unsigned long pc = 0;

   public:
    CPU(const vector<bitset<32>>& instructions);
    unsigned long read_pc() const;
    void inc_pc();
};
