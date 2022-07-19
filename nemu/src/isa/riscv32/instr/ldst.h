// #define def_EHelper(name) static inline void concat(exec_, name)(Decode * s)

// #include "/home/clares/ics2021/nemu/src/engine/interpreter/rtl-basic.h"

// TODO complete all these function

//! def_EHelper(name)
//! static inline void exec_name (Decode *s)

// #define DEBUG

#ifdef DEBUG
#include <stdio.h>

void output_debug_info(Decode* s, const char* str) {
    printf("0x%08x : %s", s->pc, str);
}

#endif

def_EHelper(add) {
    rtl_add(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "add\n");
#endif
}

def_EHelper(sub) {
    rtl_sub(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sub\n");
#endif
}

def_EHelper(sll) {
    rtl_sll(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sll\n");
#endif
}

def_EHelper(xor) {
    rtl_xor(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "xor\n");
#endif
}

def_EHelper(srl) {
    rtl_srl(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "srl\n");
#endif
}

def_EHelper(sra) {
    rtl_sra(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sra\n");
#endif
}

def_EHelper(or) {
    rtl_or(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "or\n");
#endif
}

def_EHelper(and) {
    rtl_and(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "and\n");
#endif
}

def_EHelper(lb) {
    rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lb\n");
#endif
}

def_EHelper(lh) {
    rtl_lms(s, ddest, dsrc1, id_src2->imm, 2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lh\n");
#endif
}

def_EHelper(lw) {
    rtl_lms(s, ddest, dsrc1, id_src2->imm, 4);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lw\n");
#endif
}

def_EHelper(ld) {
    rtl_lms(s, ddest, dsrc1, id_src2->imm, 8);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "ld\n");
#endif
}

def_EHelper(lbu) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lbu\n");
#endif
}

def_EHelper(lhu) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lhu\n");
#endif
}

def_EHelper(lwu) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lwu\n");
#endif
}

// TODO wait for test these instr

def_EHelper(addi) {
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "addi\n");
    // printf("ddest=0x%x | %b\n", *ddest, *ddest);
    // printf("dsrc1=0x%x | %b\n", *dsrc1, *dsrc1);
    // printf("imm=0x%x | %b\n", s->isa.instr.i.simm11_0,
    // s->isa.instr.i.simm11_0);
#endif

    rtl_addi(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    // printf("addi no implemention\n");
}

def_EHelper(slli) {
    rtl_slli(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "slli\n");
#endif
    // printf("slli no implemention\n");
}

def_EHelper(xori) {
    rtl_xori(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "xori\n");
#endif
    // printf("xori no implemention\n");
}

def_EHelper(srli) {
    rtl_srli(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "srli\n");
#endif
    // printf("srli no implemention\n");
}

def_EHelper(srai) {
    rtl_srai(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "srai\n");
#endif
    // printf("srai no implemention\n");
}

def_EHelper(ori) {
    rtl_ori(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "ori\n");
#endif
    // printf("ori no implemention\n");
}

def_EHelper(andi) {
    rtl_andi(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "andi\n");
#endif
    // printf("andi no implemention\n");
}

def_EHelper(jalr) {
    rtl_addi(s, ddest, &s->pc, 4);
    rtl_addi(s, &s->dnpc, dsrc1, s->isa.instr.i.simm11_0);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "jalr\n");
#endif
}

def_EHelper(sb) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sb\n");
#endif
}

def_EHelper(sh) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sh\n");
#endif
}

def_EHelper(sw) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sw\n");
#endif
}

def_EHelper(sd) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 8);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sd\n");
#endif
}

def_EHelper(beq) {
    if (*(id_src1->preg) == *(id_src2->preg)) s->dnpc = s->pc + id_dest->simm;
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "beq\n");
#endif
}

def_EHelper(bne) {
    // isa_reg_display();
    if ((int)*(id_src1->preg) != (int)*(id_src2->preg)) {
        s->dnpc = s->pc + id_dest->simm;
        // printf("Branch!, src1 = 0x%08x , src2 = 0x%08x\n",
        // *(id_src1->preg),*(id_src2->preg));
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "bne\n");
#endif
}

def_EHelper(blt) {
    if ((int)*(id_src1->preg) < (int)*(id_src2->preg)) {
        s->dnpc = s->pc + id_dest->simm;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "blt\n");
#endif
}

def_EHelper(bge) {
    if ((int)*(id_src1->preg) >= (int)*(id_src2->preg)) {
        s->dnpc = s->pc + id_dest->simm;
    }
    gpr(0) = 0;
#ifdef DEBUG
    // printf("rs1 = 0x%08x , rs2 = 0x%08x\n", (int)*(id_src1->preg),
    // (int)*(id_src2->preg));
    output_debug_info(s, "bge\n");
#endif
}

def_EHelper(bltu) {
    if (*(id_src1->preg) < *(id_src2->preg)) {
        s->dnpc = s->pc + id_dest->simm;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "bltu\n");
#endif
}

def_EHelper(bgeu) {
    if (*(id_src1->preg) >= *(id_src2->preg)) {
        s->dnpc = s->pc + id_dest->simm;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "bgeu\n");
#endif
}

def_EHelper(jal) {
    // if(ddest != )
    rtl_addi(s, ddest, &s->pc, 4);
    rtl_addi(s, &s->dnpc, &s->pc, id_src1->simm);
    gpr(0) = 0;
#ifdef DEBUG
    printf("%08x\n", *ddest);
    output_debug_info(s, "jal\n");
    // printf("the step is %d | %x",id_src1->simm, id_src1->simm);
#endif
}

def_EHelper(auipc) {
    rtl_addi(s, ddest, &s->pc, s->isa.instr.u.imm31_12 << 12);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "auipc\n");
#endif
}

def_EHelper(slti) {
    int rs1 = *dsrc1;
    int rs2 = id_src2->simm;
    if (rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "slti\n");
#endif
}

def_EHelper(sltiu) {
    unsigned int rs1 = *dsrc1;
    unsigned int rs2 = id_src2->simm;
    if (rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "slti\n");
#endif
}

def_EHelper(lui) {
    rtl_li(s, ddest, id_src1->imm);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "lui\n");
#endif
}

def_EHelper(slt) {
    int rs1 = *dsrc1;
    int rs2 = *dsrc2;
    if (rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "slt\n");
#endif
}

def_EHelper(sltu) {
    unsigned int rs1 = *dsrc1;
    unsigned int rs2 = *dsrc2;
    if (rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "sltu\n");
#endif
}

def_EHelper(mul) {
    rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "mul\n");
#endif
}

def_EHelper(div) {
    rtl_divs_q(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "div\n");
#endif
}

def_EHelper(mulh) {
    rtl_muls_hi(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "mulh\n");
#endif
}

def_EHelper(divu) {
    rtl_divu_q(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "divu\n");
#endif
}

def_EHelper(rem) {
    rtl_divs_r(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "rem\n");
#endif
}

def_EHelper(remu) {
    rtl_divu_r(s, ddest, dsrc1, dsrc2);
    gpr(0) = 0;
#ifdef DEBUG
    output_debug_info(s, "rem\n");
#endif
}