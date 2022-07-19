#include <cpu/decode.h>
#include "../local-include/rtl.h"

//TODO unfinish: add the rest instructions for this ISA
//? myCodes begin ----------------------------------------------------------
// #define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap)

#define INSTR_LIST(f)   f(nemu_trap)\
                        f(add) f(sub) f(sll) f(xor) f(srl) f(sra) f(or) f(and) f(slt) f(sltu) f(mul) f(mulh) f(div) f(divu) f(rem) f(remu)\
                        f(lb) f(lh) f(lw) f(ld) f(lbu) f(lhu) f(lwu)\
                        f(addi) f(slli) f(xori) f(srli) f(srai) f(ori) f(andi)  f(slti) f(sltiu)\
                        f(jalr)\
                        f(sb) f(sh) f(sw) f(sd)\
                        f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(auipc)\
                        f(lui) f(jal) f(inv)
                        // R I I_ARI I_JMP S SB U UJ

//? myCodes end ------------------------------------------------------------

// the exec_id will be defined here
def_all_EXEC_ID();
