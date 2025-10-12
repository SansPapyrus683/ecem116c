#include <vector>

#include "cpu.h"

using std::vector;

CPU::CPU(const vector<bitset<32>>& instructions) : instr(instructions) {
    for (int i = 0; i < 4096; i++) { mem[i] = (0); }
}

unsigned long CPU::read_pc() const { return pc; }
void CPU::inc_pc() { pc++; }
