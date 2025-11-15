#ifndef PROCSIM_HPP
#define PROCSIM_HPP

#include <cstdint>
#include <cstdio>

#define DEFAULT_K0 1
#define DEFAULT_K1 2
#define DEFAULT_K2 3
#define DEFAULT_R 8
#define DEFAULT_F 4

typedef struct _proc_inst_t {
    uint32_t instruction_address;
    int32_t op_code;
    int32_t src_reg[2];
    int32_t dest_reg;

    // stack overflow said i could do this
    mutable bool rs1_ready;
    mutable bool rs2_ready;
    mutable bool rd_ready;
    mutable bool running = false;
    int tag;
} proc_inst_t;

typedef struct _proc_stats_t {
    float avg_inst_retired;
    float avg_inst_fired;
    float avg_disp_size;
    unsigned long max_disp_size;
    unsigned long retired_instruction;
    unsigned long cycle_count;
} proc_stats_t;

bool read_instruction(proc_inst_t* p_inst);

void setup_proc(int r, int k0, int k1, int k2, int f);
void run_proc(proc_stats_t* p_stats);
void complete_proc(proc_stats_t* p_stats);

#endif /* PROCSIM_HPP */
