#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <iostream>
#include <string>

using namespace std;

class CPU {
   private:
    int mem[4096];  // data memory byte addressable in little endian fashion;
    unsigned long pc = 0;

   public:
    CPU();
    unsigned long read_pc() const;
    void inc_pc();
};
