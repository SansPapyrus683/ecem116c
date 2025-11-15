#include "procsim.hpp"

#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

int max_res;
int rs_sz;
int fetch_num;

int cyc = 0;
int tag = 1;
int last_tag = -1;
int done_amt = 0;

struct TraceInfo {
    int fetch;
    int disp;
    int sched;
    int exec;
    int state;
};
vector<TraceInfo> traces;

map<int, int> last_write;
vector<proc_inst_t> fetched;

deque<proc_inst_t> dispatch_q;
vector<size_t> size_hist;

map<int, set<int>> to_write;
set<proc_inst_t> rs;

deque<proc_inst_t> execing;
vector<int> fu_free_num;

vector<proc_inst_t> to_delete;
vector<proc_inst_t> completed;

bool operator<(const proc_inst_t& i1, const proc_inst_t& i2) { return i1.tag < i2.tag; }

/**
 * Subroutine for initializing the processor. You many add and initialize any global or
 * heap variables as needed.
 *
 * @r ROB size
 * @k0 Number of k0 FUs
 * @k1 Number of k1 FUs
 * @k2 Number of k2 FUs
 * @f Number of instructions to fetch
 */
void setup_proc(int r, int k0, int k1, int k2, int f) {
    max_res = r;
    rs_sz = 2 * (k0 + k1 + k2);
    fu_free_num = {k0, k1, k2};
    fetch_num = f;
}

void fetch() {
    for (int i = 0; i < fetch_num; i++) {
        proc_inst_t to_push;
        if (read_instruction(&to_push)) {
            to_push.tag = tag++;
            if (to_push.op_code == -1) { to_push.op_code = 1; }

            to_push.prev_w[0] = last_write[to_push.src_reg[0]];
            to_push.prev_w[1] = last_write[to_push.src_reg[1]];
            if (to_push.dest_reg != -1) { last_write[to_push.dest_reg] = tag - 1; }

            fetched.push_back(to_push);
            traces.push_back({cyc, -1, -1, -1, -1});
        } else {
            last_tag = tag - 1;
        }
    }
}

void dispatch() {
    for (const proc_inst_t& pi : fetched) {
        dispatch_q.push_back(pi);
        traces[pi.tag - 1].disp = cyc;
    }
    size_hist.push_back(dispatch_q.size());
    fetched.clear();
}

void schedule() {
    while ((int)rs.size() < rs_sz && !dispatch_q.empty()) {
        proc_inst_t i = dispatch_q.front();
        dispatch_q.pop_front();
        traces[i.tag - 1].sched = cyc;

        // no need to check if reg is -1 bc that set is guaranteed to be empty
        i.rs1_ready = !to_write[i.src_reg[0]].count(i.prev_w[0]);
        i.rs2_ready = !to_write[i.src_reg[1]].count(i.prev_w[1]);

        if (i.dest_reg != -1) { to_write[i.dest_reg].insert(i.tag); }

        rs.insert(i);
    }
}

void execute() {
    for (const proc_inst_t& pi : rs) {
        if (pi.running) { continue; }

        bool reg_good = pi.rs1_ready && pi.rs2_ready;
        if (reg_good && fu_free_num[pi.op_code] > 0) {
            fu_free_num[pi.op_code]--;
            execing.push_back(pi);
            traces[pi.tag - 1].exec = cyc;
            pi.running = true;
        }
    }
}

void state_upd() {
    for (int i = 0; i < max_res && !execing.empty(); i++) {
        proc_inst_t done = execing.front();
        execing.pop_front();

        fu_free_num[done.op_code]++;
        traces[done.tag - 1].state = cyc;
        completed.push_back(done);
        done_amt++;
    }
}

/**
 * Subroutine that simulates the processor.
 *   The processor should fetch instructions as appropriate, until all instructions have
 * executed
 *
 * @p_stats Pointer to the statistics structure
 */
void run_proc(proc_stats_t* p_stats) {
    while (last_tag == -1 || done_amt < last_tag) {
        cyc++;

        state_upd();
        execute();
        schedule();
        dispatch();
        fetch();

        // why is this done at the end bro
        for (const proc_inst_t& i : to_delete) { rs.erase(i); }

        for (const proc_inst_t& i : completed) { to_write[i.dest_reg].erase(i.tag); }
        for (const proc_inst_t& i : rs) {
            i.rs1_ready = !to_write[i.src_reg[0]].count(i.prev_w[0]);
            i.rs2_ready = !to_write[i.src_reg[1]].count(i.prev_w[1]);
        }

        to_delete = completed;
        completed = {};
    }

    printf("INST\tFETCH\tDISP\tSCHED\tEXEC\tSTATE\n");
    for (int i = 0; i < (int)traces.size(); i++) {
        const TraceInfo& ti = traces[i];
        printf("%i\t%i\t%i\t%i\t%i\t%i\n", i + 1, ti.fetch, ti.disp, ti.sched, ti.exec,
               ti.state);
    }
    printf("\n");
}

/**
 * Subroutine for cleaning up any outstanding instructions and calculating overall
 * statistics such as average IPC, average fire rate etc.
 *
 * @p_stats Pointer to the statistics structure
 */
void complete_proc(proc_stats_t* p_stats) {
    long long dispatch_avg = 0;
    for (size_t s : size_hist) {
        dispatch_avg += s;
    }

    p_stats->retired_instruction = last_tag;
    p_stats->max_disp_size = *max_element(size_hist.begin(), size_hist.end());
    p_stats->avg_disp_size = (double)dispatch_avg / cyc;
    p_stats->avg_inst_fired = (double)last_tag / cyc;
    p_stats->avg_inst_retired = (double)last_tag / cyc;
    p_stats->cycle_count = cyc;
}
