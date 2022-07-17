// #define def_EHelper(name) static inline void concat(exec_, name)(Decode * s)

// #include "/home/clares/ics2021/nemu/src/engine/interpreter/rtl-basic.h"

// TODO complete all these function

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
    printf("no implemention!\n");
}

def_EHelper(lhu) {
    printf("no implemention!\n");
}

def_EHelper(lwu) {
    printf("no implemention!\n");
}

def_EHelper(addi) {
    printf("no implemention!\n");
}

def_EHelper(slli) {
    printf("no implemention!\n");
}

def_EHelper(xori) {
    printf("no implemention!\n");
}

def_EHelper(srli) {
    printf("no implemention!\n");
}

def_EHelper(srai) {
    printf("no implemention!\n");
}

def_EHelper(ori) {
    printf("no implemention!\n");
}

def_EHelper(andi) {
    printf("no implemention!\n");
}

def_EHelper(jalr) {
    printf("no implemention!\n");
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
    printf("no implemention!\n");
}

def_EHelper(bne) {
    printf("no implemention!\n");
}

def_EHelper(blt) {
    printf("no implemention!\n");
}

def_EHelper(bge) {
    printf("no implemention!\n");
}

def_EHelper(bltu) {
    printf("no implemention!\n");
}

def_EHelper(bgeu) {
    printf("no implemention!\n");
}

def_EHelper(jal) {
    printf("no implemention!\n");
}