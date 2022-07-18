#include <cpu/ifetch.h>
#include "../include/isa-all-instr.h"
#include "../local-include/reg.h"

#define DEBUG

#ifdef DEBUG

void output_instr_data(Decode* s){
    printf("the unrecognized instr is %032b \n", s->isa.instr.val);
    printf("the opcode is %05b%02b \n", s->isa.instr.r.opcode6_2, s->isa.instr.r.opcode1_0);
    printf("the func3  is %03b \n", s->isa.instr.r.funct3);
    printf("the func7  is %07b \n", s->isa.instr.r.funct7);
}

#endif

def_all_THelper();

static uint32_t get_instr(Decode* s) {
    return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
    void concat(decode_op_, name)(Decode * s, Operand * op, word_t val, bool flag)

static def_DopHelper(i) {
    if(flag == false) op->imm = val;
    else op->simm = val;
}

static def_DopHelper(r) {
    bool is_write = flag;
    static word_t zero_null = 0;
    op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

//! decode_I
static def_DHelper(I) {
    decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
    decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, true);
    decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}
//! decode_U
static def_DHelper(U) {
    decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, false);
    decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}
//! decode_S
static def_DHelper(S) {
    decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
    sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
    decode_op_i(s, id_src2, simm, true);
    decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}
//! decode_R
//? myCodes begin ----------------------------------------------------------
static def_DHelper(R) {
    decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
    decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
    decode_op_r(s, id_dest, s->isa.instr.r.rd, false);
}
//! decode_SB
static def_DHelper(SB) {
    sword_t simm = 0x00;
    simm |= s->isa.instr.sb.imm12 << 12;
    simm |= s->isa.instr.sb.imm12 << 11;
    simm |= s->isa.instr.sb.imm11 << 10;
    simm |= s->isa.instr.sb.imm10_5 << 4;
    simm |= s->isa.instr.sb.imm4_1 << 0;
    decode_op_i(s, id_dest, simm, true);
    decode_op_r(s, id_src1, s->isa.instr.sb.rs1, false);
    decode_op_r(s, id_src2, s->isa.instr.sb.rs2, false);
}
//! decode_UJ
static def_DHelper(UJ) {
    sword_t simm = 0x00;
    simm |= s->isa.instr.uj.imm20 << 20;
    simm |= s->isa.instr.uj.imm20 << 19;
    simm |= s->isa.instr.uj.imm19_12 << 11;
    simm |= s->isa.instr.uj.imm11 << 10;
    simm |= s->isa.instr.uj.imm10_1;
    decode_op_r(s, id_dest, s->isa.instr.uj.rd, false);
    decode_op_i(s, id_src1, simm, true);
}
//? myCodes end ------------------------------------------------------------

//! I type table
def_THelper(load) {
    /*
    uint32_t key, mask, shift;
    decode_fun("??????? ????? ????? 010 ????? ????? ??", 38, &key, &mask, &shift);
    if(((get_instr(s)>>shift) & mask) == key) {
        decode_empty(s, 0);
        return table_lw(s);
    }
    */
    // TODO : add more instruction for I type
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", lb);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", lh);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
    def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", ld);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", lhu);
    def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", lwu);
    //? myCodes end ------------------------------------------------------------
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
}

//! S type table
def_THelper(store) {
    // TODO : add more instruction for S type
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sh);
    def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
    def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", sd);
    //? myCodes end ------------------------------------------------------------
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
}

//! R type table
def_THelper(arithmetic) {
    // TODO complete the table of R type
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("0000000 ????? ????? 000 ????? ????? ??", add);
    def_INSTR_TAB("0100000 ????? ????? 000 ????? ????? ??", sub);
    def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", sll);
    def_INSTR_TAB("0000000 ????? ????? 100 ????? ????? ??", xor);
    def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srl);
    def_INSTR_TAB("0100000 ????? ????? 101 ????? ????? ??", sra);
    def_INSTR_TAB("0000000 ????? ????? 110 ????? ????? ??", or);
    def_INSTR_TAB("0000000 ????? ????? 111 ????? ????? ??", and);
    // this part is deserve for the future
    // def_INSTR_TAB("0001000 ????? ????? 011 ????? 01100 11",lr_d);
    // def_INSTR_TAB("0001100 ????? ????? 011 ????? 01100 11",sc_d);
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
    //? myCodes end ------------------------------------------------------------
}

//! I_ARI type table
def_THelper(imm_arithmetic) {
    // TODO complete the table of I_ARI type
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
    def_INSTR_TAB("0000000 ????? ????? 001 ????? ????? ??", slli);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
    def_INSTR_TAB("0000000 ????? ????? 101 ????? ????? ??", srli);
    def_INSTR_TAB("0010000 ????? ????? 101 ????? ????? ??", srai);
    def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", ori);
    def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
    //? myCodes end ------------------------------------------------------------
}

//! I_JMP type table
def_THelper(imm_jump) {
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", jalr);
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
    //? myCodes end ------------------------------------------------------------
}

//! SB type table
def_THelper(branch) {
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);
    def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
    def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
    def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
    def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", bltu);
    def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", bgeu);
#ifdef DEBUG
    output_instr_data(s);
#endif
    return EXEC_ID_inv;
    //? myCodes end ------------------------------------------------------------
}

//! UJ type table
//? myCodes begin ----------------------------------------------------------
def_THelper(branch_no_condition) {
    return EXEC_ID_jal;
}

// //! U type table
def_THelper(u_table) {
    def_INSTR_TAB("??????? ????? ????? ??? ????? ?1??? ??", lui);
    def_INSTR_TAB("??????? ????? ????? ??? ????? ?0??? ??", auipc);
    return table_inv(s);
}
//? myCodes end ------------------------------------------------------------

def_THelper(main) {
    /*
    uint32_t key, mask, shift;
    decode_fun("??????? ????? ????? ??? ????? 00000 11", 38, &key, &mask, &shift);
    if(((get_instr(s)>>shift) & mask) == key) {
        decode_I(s, 0);
        return table_load(s);
    }
    */
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I, load);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S, store);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 0?101 11", U, u_table);
    def_INSTR_TAB("??????? ????? ????? ??? ????? 11010 11", nemu_trap);
    // TODO unfinish: add more type of opcode
    //? myCodes begin ----------------------------------------------------------
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I, imm_arithmetic);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I, imm_jump);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R, arithmetic);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", SB, branch);
    def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", UJ, branch_no_condition);
    //? myCodes end ------------------------------------------------------------
#ifdef DEBUG
    output_instr_data(s);
#endif
    return table_inv(s);
};

/*
  the def_THelper(main) could be extend to
  static inline int concat(table_, main)(Decode * s){}
  which is the 'table_main'...

*   static inline int table_main(Decode* s){
*       do {
*           uint32_t key, mask, shift;
*           decode_fun(pattern, STRLEN(pattern), &key, &mask, &shift);
*           if (((get_instr(s) >> shift) & mask) == key) {
*               table_load(s);
*           }
*       }
*       do {
*           uint32_t key, mask, shift;
*           decode_fun(pattern, STRLEN(pattern), &key, &mask, &shift);
*           if (((get_instr(s) >> shift) & mask) == key) {
*               table_store(s);
*           }
*       }
*       ....
*       return table_inv(s);
*   }

*/

int isa_fetch_decode(Decode* s) {
    // fetch 32bit instruction
    s->isa.instr.val = instr_fetch(&s->snpc, 4);
    // query the table_main to get the index
    int idx = table_main(s);
    return idx;
}
