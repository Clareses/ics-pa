// #define def_EHelper(name) static inline void concat(exec_, name)(Decode * s)

// #include "/home/clares/ics2021/nemu/src/engine/interpreter/rtl-basic.h"

// TODO complete all these function

//! def_EHelper(name)
//! static inline void exec_name (Decode *s)

// #define DEBUG

#ifdef DEBUG
#include <stdio.h>

void output_debug_info(Decode* s, const char* str){
    printf("0x%08x : %s", s->pc, str);
}

#endif

def_EHelper(add) {
    rtl_add(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"add\n");
#endif
}

def_EHelper(sub) {
    rtl_sub(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"sub\n");
#endif
}


def_EHelper(sll) {
    rtl_sll(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"sll\n");
#endif
}

def_EHelper(xor) {
    rtl_xor(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"xor\n");
#endif
}

def_EHelper(srl) {
    rtl_srl(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"srl\n");
#endif
}

def_EHelper(sra) {
    rtl_sra(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"sra\n");
#endif
}

def_EHelper(or) {
    rtl_or(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"or\n");
#endif
}

def_EHelper(and) {
    rtl_and(s, ddest, dsrc1, dsrc2);
#ifdef DEBUG
    output_debug_info(s,"and\n");
#endif
}

def_EHelper(lb) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
#ifdef DEBUG
    output_debug_info(s,"lb\n");
#endif
}

def_EHelper(lh) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
#ifdef DEBUG
    output_debug_info(s,"lh\n");
#endif
}

def_EHelper(lw) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
#ifdef DEBUG
    output_debug_info(s,"lw\n");
#endif
}

def_EHelper(ld) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 8);
#ifdef DEBUG
    output_debug_info(s,"ld\n");
#endif
}

def_EHelper(lbu) {
    printf("lbu no implemention\n");
}

def_EHelper(lhu) {
    printf("lhu no implemention\n");
}

def_EHelper(lwu) {
    printf("lwu no implemention\n");
}

//TODO wait for test these instr

def_EHelper(addi) {
   
#ifdef DEBUG
    output_debug_info(s,"addi\n");
    // printf("ddest=0x%x | %b\n", *ddest, *ddest);
    // printf("dsrc1=0x%x | %b\n", *dsrc1, *dsrc1);
    // printf("imm=0x%x | %b\n", s->isa.instr.i.simm11_0, s->isa.instr.i.simm11_0);
#endif

    rtl_addi(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
    // printf("addi no implemention\n");
}

def_EHelper(slli) {
    rtl_slli(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"slli\n");
#endif
    // printf("slli no implemention\n");
}

def_EHelper(xori) {
    rtl_xori(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"xori\n");
#endif
    // printf("xori no implemention\n");
}

def_EHelper(srli) {
    rtl_srli(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"srli\n");
#endif
    // printf("srli no implemention\n");
}

def_EHelper(srai) {
    rtl_srai(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"srai\n");
#endif
    // printf("srai no implemention\n");
}

def_EHelper(ori) {
    rtl_ori(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"ori\n");
#endif
    // printf("ori no implemention\n");
}

def_EHelper(andi) {
    rtl_andi(s, ddest, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"andi\n");
#endif
    // printf("andi no implemention\n");
}

def_EHelper(jalr) {
    rtl_addi(s, ddest, &s->pc, 4);
    rtl_addi(s, &s->dnpc, dsrc1, s->isa.instr.i.simm11_0);
#ifdef DEBUG
    output_debug_info(s,"jalr\n");
#endif
}

def_EHelper(sb) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
#ifdef DEBUG
    output_debug_info(s,"sb\n");
#endif
}

def_EHelper(sh) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
#ifdef DEBUG
    output_debug_info(s,"sh\n");
#endif
}

def_EHelper(sw) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
#ifdef DEBUG
    output_debug_info(s,"sw\n");
#endif
}

def_EHelper(sd) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 8);
#ifdef DEBUG
    output_debug_info(s,"sd\n");
#endif
}

def_EHelper(beq) {
    if(*(id_src1->preg) == *(id_src2->preg))
        s->dnpc = s->pc + id_dest->simm;
#ifdef DEBUG
    output_debug_info(s,"beq\n");
#endif
}

def_EHelper(bne) {
    if(*(id_src1->preg) != *(id_src2->preg))
        s->dnpc = s->pc + id_dest->simm;
#ifdef DEBUG
    output_debug_info(s,"bne\n");
#endif
}

def_EHelper(blt) {
    printf("blt no implemention\n");
}

def_EHelper(bge) {
    printf("bge no implemention\n");
}

def_EHelper(bltu) {
    printf("bltu no implemention\n");
}

def_EHelper(bgeu) {
    printf("bgeu no implemention\n");
}

def_EHelper(jal) {
    rtl_addi(s, ddest, &s->pc, 4);
    rtl_addi(s, &s->dnpc, &s->pc, id_src1->simm);
#ifdef DEBUG
    output_debug_info(s,"jal\n");
    // printf("the step is %d | %x",id_src1->simm, id_src1->simm);
#endif
}

def_EHelper(auipc) {
    rtl_addi(s, ddest, &s->pc, s->isa.instr.u.imm31_12 << 12);
#ifdef DEBUG
    output_debug_info(s,"auipc\n");
#endif
}

def_EHelper(slti) {
    int rs1 = *dsrc1;
    int rs2 = id_src2->simm;
    if(rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
#ifdef DEBUG
    output_debug_info(s,"slti\n");
#endif
}

def_EHelper(sltiu) {
    unsigned int rs1 = *dsrc1;
    unsigned int rs2 = id_src2->simm;
    if(rs1 < rs2) {
        *ddest = 1;
    } else {
        *ddest = 0;
    }
#ifdef DEBUG
    output_debug_info(s,"slti\n");
#endif
}

def_EHelper(lui) {
    rtl_li(s, ddest, id_src1->imm);
#ifdef DEBUG
    output_debug_info(s,"lui\n");
#endif
}
