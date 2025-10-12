#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "cpu.h"
#include "debugging.hpp"

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
    while (infile >> curr) {
        bytes.push_back((char) curr);
    }

    vector<bitset<32>> instructions(bytes.size() / 4);
    for (int i = 0; i < bytes.size(); i += 4) {
        int at = 0;
        for (int j = 0; j < 4; j++) {
            char b = bytes[i + j];
            for (int k = 0; k < 8; k++) {
                instructions[i / 4].set(at++, b & (1 << k));
            }
        }
    }

    /*
     * Instantiate your CPU object here.  CPU class is the main class in this project
     * that defines different components of the processor. CPU class also has different
     * functions for each stage (e.g., fetching an instruction, decoding, etc.).
     */

    CPU cpu(instructions);

    bool done = true;
    // processor's main loop. Each iteration is equal to one clock cycle.
    while (cpu.read_pc() < instructions.size()) {
        cpu.inc_pc();
    }

    int a0 = 0;
    int a1 = 0;
    // print the results (you should replace a0 and a1 with your own variables that
    // point to a0 and a1)
    printf("(%i, %i)\n", a0, a1);
}
