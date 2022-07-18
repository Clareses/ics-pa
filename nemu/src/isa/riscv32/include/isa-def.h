#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
    struct {
        rtlreg_t _32;
    } gpr[32];

    vaddr_t pc;
} riscv32_CPU_state;

// decode
typedef struct {
    union {
        // load & imm arithmetic
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t rd : 5;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            int32_t simm11_0 : 12;
        } i;

        // store
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t imm4_0 : 5;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            int32_t simm11_5 : 7;
        } s;

        // big imm type
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t rd : 5;
            uint32_t imm31_12 : 20;
        } u;

        // TODO unfinish: add more instr type
        //? myCodes begin ----------------------------------------------------------
        // I_ari
        // struct {
        //     uint32_t opcode1_0 : 2;
        //     uint32_t opcode6_2 : 5;
        //     uint32_t rd : 5;
        //     uint32_t funct3 : 3;
        //     uint32_t rs1 : 5;
        //     int32_t simm11_0 : 12;
        // } i_ari;

        // I_jmp
        // struct {
        //     uint32_t opcode1_0 : 2;
        //     uint32_t opcode6_2 : 5;
        //     uint32_t rd : 5;
        //     uint32_t funct3 : 3;
        //     uint32_t rs1 : 5;
        //     int32_t simm11_0 : 12;
        // } i_jmp;

        // arithmetic
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t rd : 5;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            uint32_t funct7 : 7;
        } r;

        // branch with condition
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t imm11 : 1;
            uint32_t imm4_1 : 4;
            uint32_t funct3 : 3;
            uint32_t rs1 : 5;
            uint32_t rs2 : 5;
            uint32_t imm10_5 : 6;
            uint32_t imm12 : 1;
        } sb;

        // branch with no condition
        struct {
            uint32_t opcode1_0 : 2;
            uint32_t opcode6_2 : 5;
            uint32_t rd : 5;
            uint32_t imm19_12 : 8;
            uint32_t imm11 : 1;
            uint32_t imm10_1 : 10;
            uint32_t imm20 : 1;
        } uj;
        //? myCodes end ------------------------------------------------------------

        uint32_t val;
    } instr;
} riscv32_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif
