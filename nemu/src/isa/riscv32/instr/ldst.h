// #define def_EHelper(name) static inline void concat(exec_, name)(Decode * s)

// #include "/home/clares/ics2021/nemu/src/engine/interpreter/rtl-basic.h"

// TODO complete all these function

//! def_EHelper(name)
//! static inline void exec_name (Decode *s)

def_EHelper(add) {
    rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
    rtl_sub(s, ddest, dsrc1, dsrc2);
}


def_EHelper(sll) {
    rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
    rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
    rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
    rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
    rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
    rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(lb) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lh) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(lw) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(ld) {
    rtl_lm(s, ddest, dsrc1, id_src2->imm, 8);
}

def_EHelper(lbu) {
    printf("lbu no implemention!\n");
}

def_EHelper(lhu) {
    printf("lhu no implemention!\n");
}

def_EHelper(lwu) {
    printf("lwu no implemention!\n");
}

def_EHelper(addi) {
    rtl_addi(s, ddest, dsrc1, s->instr.i.simm11_0);
    // printf("addi no implemention!\n");
}

def_EHelper(slli) {
    printf("slli no implemention!\n");
}

def_EHelper(xori) {
    printf("xori no implemention!\n");
}

def_EHelper(srli) {
    printf("srli no implemention!\n");
}

def_EHelper(srai) {
    printf("srai no implemention!\n");
}

def_EHelper(ori) {
    printf("ori no implemention!\n");
}

def_EHelper(andi) {
    printf("andi no implemention!\n");
}

def_EHelper(jalr) {
    printf("jalr no implemention!\n");
}

def_EHelper(sb) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(sh) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sw) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(sd) {
    rtl_sm(s, ddest, dsrc1, id_src2->imm, 8);
}

def_EHelper(beq) {
    printf("beq no implemention!\n");
}

def_EHelper(bne) {
    printf("bne no implemention!\n");
}

def_EHelper(blt) {
    printf("blt no implemention!\n");
}

def_EHelper(bge) {
    printf("bge no implemention!\n");
}

def_EHelper(bltu) {
    printf("bltu no implemention!\n");
}

def_EHelper(bgeu) {
    printf("bgeu no implemention!\n");
}

def_EHelper(jal) {
    printf("jal no implemention!\n");
}

def_EHelper(auipc) {
    printf("auipc no implemention!\n");
}

// def_EHelper(lui) {
//     printf("lui no implemention!\n");
// }