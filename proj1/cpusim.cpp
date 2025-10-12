#include <stdio.h>
#include <stdlib.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "CPU.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) { return -1; }

    /*
     * This is the front end of your project.
     * You need to first read the instructions that are stored in a file and load them
     * into an instruction memory.
     */

    /*
     * Each cell should store 1 byte. You can define the memory either dynamically, or
     * define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is
     * 32 bits (i.e., 4 lines, saved in little-endian mode). Each line in the input file
     * is stored as an hex and is 1 byte (each four lines are one instruction). You need
     * to read the file line by line and store it into the memory. You may need a
     * mechanism to convert these values to bits so that you can read opcodes, operands,
     * etc.
     */

    ifstream infile(argv[1]);  // open the file
    if (!(infile.is_open() && infile.good())) {
        cout << "error opening file" << endl;
        return 1;
    }

    char instr[4096];
    string line;
    int i = 0;
    while (infile) {
        infile >> line;
        stringstream line2(line);
        char x;
        line2 >> x;
        instr[i] = x;  // be careful about hex
        i++;
        line2 >> x;
        instr[i] = x;  // be careful about hex
        cout << instr[i] << endl;
        i++;
    }

    int maxPC = i / 4;

    /*
     * Instantiate your CPU object here.  CPU class is the main class in this project
     * that defines different components of the processor. CPU class also has different
     * functions for each stage (e.g., fetching an instruction, decoding, etc.).
     */

    CPU myCPU;

    bool done = true;
    // processor's main loop. Each iteration is equal to one clock cycle.
    while (done) {
        // fetch

        // decode

        // ...
        myCPU.inc_pc();
        if (myCPU.read_pc() > maxPC) { break; }
    }

    int a0 = 0;
    int a1 = 0;
    // print the results (you should replace a0 and a1 with your own variables that
    // point to a0 and a1)
    printf("(%i, %i)\n", a0, a1);
}
