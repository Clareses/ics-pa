#include "local-include/reg.h"
#include <isa.h>

#define regs_pair(i) regs[i], gpr(i)

const char* regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void isa_reg_display() {
    printf("registers info:\n");
    for (int i = 0; i < 32; i += 4) {
        printf("  %4s = 0x%08x  %4s = 0x%08x  %4s = 0x%08x  %4s = 0x%08x \n",
               regs_pair(i), regs_pair(i + 1), regs_pair(i + 2), regs_pair(i + 3));
    }
}

word_t isa_reg_str2val(const char* s, bool* success) {
    *success = true;
    for (int i = 0; i < 32; i++){
        if(!strcmp(s, regs[i]))
            return gpr(i);
    }
    *success = false;
    return -1;
}
