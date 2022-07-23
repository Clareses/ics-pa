#include <common.h>

extern uint64_t g_nr_guest_instr;
FILE* log_fp = NULL;
FILE* mat_fp = NULL;

void init_log(const char* log_file) {
    log_fp = stdout;
    if (log_file != NULL) {
        FILE* fp = fopen(log_file, "w");
        Assert(fp, "Can not open '%s'", log_file);
        log_fp = fp;
    }
    Log("Log is written to %s", log_file ? log_file : "stdout");
}

void init_mat(const char* mat_file) {
    mat_fp = stdout;
    if (mat_file != NULL) {
        FILE* fp = fopen(mat_file, "w");
        Assert(fp, "Can not open '%s'", mat_file);
        mat_fp = fp;
    }
    Log("MemoryAccess Data is written to %s", mat_file ? mat_file : "stdout");
}

bool log_enable() {
    return MUXDEF(CONFIG_TRACE,
                  (g_nr_guest_instr >= CONFIG_TRACE_START) &&
                      (g_nr_guest_instr <= CONFIG_TRACE_END),
                  false);
}
