#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__

#include <isa.h>

/*
! the Decode Structure include these parts:
!-- PCs (include currentPC, dynamic, static)
!-- OPRANDS (include dest, src1, src2)
  -- preg
  -- imm
  -- simm
!-- ISAinfo
 ?-- val (save the whole instruction)
 ?-- instr union
    -- i type code
    -- s type code
    -- l type code

   In decode.c, use some macro to move the values in ISAinfo
 into the Decodes Oprands or PCs

!   Everytime, we use pattern to get the shift and the mask and key, then
! compare the key with the instruction after shift, if match, we call the
! Decode_I/S/U func to load the value in ISAinfo into the Decodes Struct,
! and then step into a new macro (table_load/store/lui), compare pattern,
! then call the table_lw/.....(a lot of instructions). Finally, this tab-
! le_xx (defined in 'def_all_THelper'), return a new EXEC_ID to idx...

 ! if want to add some ISA, 
 ! first should add some instr type in isa-def.h
 ! then can find the Defination of INSTR_LIST in file 'isa-all-instr.h'
 ! add the pattern of the instruction into decode.c
 ! and then add some function to ldst.h
*/

typedef struct {
    union {
        IFDEF(CONFIG_ISA_x86, uint64_t* pfreg);
        IFDEF(CONFIG_ISA_x86, uint64_t fval);
        //! we only use this part, a operand could only be
        //! a register 、a unsigned imm or a signed imm
        rtlreg_t* preg;
        word_t imm;
        sword_t simm;
    };
    IFDEF(CONFIG_ISA_x86, rtlreg_t val);
    IFDEF(CONFIG_ISA_x86, uint8_t type);
    IFDEF(CONFIG_ISA_x86, uint8_t reg);
} Operand;

typedef struct Decode {
    // the program counter, point to the current exec position
    vaddr_t pc;
    // static next pc, point to the next 32 bits place (4 byte)
    vaddr_t snpc;
    // dynamic next pc, point to the next place the cpu will exec actually
    vaddr_t dnpc;
    // a handler for the instruction
    void (*EHelper)(struct Decode*);
    // the operand(操作数) has been parsed
    Operand dest, src1, src2;
    // record the info of how to parse the instruction
    ISADecodeInfo isa;
    // log data buffer, ignore
    IFDEF(CONFIG_ITRACE, char logbuf[128]);
} Decode;

#define id_src1 (&s->src1)
#define id_src2 (&s->src2)
#define id_dest (&s->dest)

// `INSTR_LIST` is defined at src/isa/$ISA/include/isa-all-instr.h
#define def_EXEC_ID(name) concat(EXEC_ID_, name),
#define def_all_EXEC_ID() enum { MAP(INSTR_LIST, def_EXEC_ID) TOTAL_INSTR }

// --- prototype of table helpers ---
#define def_THelper(name) static inline int concat(table_, name)(Decode * s)
#define def_THelper_body(name)         \
    def_THelper(name) {                \
        return concat(EXEC_ID_, name); \
    }
#define def_all_THelper() MAP(INSTR_LIST, def_THelper_body)

// --- prototype of decode helpers ---
#define def_DHelper(name) void concat(decode_, name)(Decode * s, int width)
// empty decode helper
static inline def_DHelper(empty) {}

// parse the instruction using the str pattern...
__attribute__((always_inline)) static inline void pattern_decode(const char* str,
                                                                 int len,
                                                                 uint32_t* key,
                                                                 uint32_t* mask,
                                                                 uint32_t* shift) {
    uint32_t __key = 0, __mask = 0, __shift = 0;

#define macro(i)                                                   \
    if ((i) >= len)                                                \
        goto finish;                                               \
    else {                                                         \
        char c = str[i];                                           \
        if (c != ' ') {                                            \
            Assert(c == '0' || c == '1' || c == '?',               \
                   "invalid character '%c' in pattern string", c); \
            __key = (__key << 1) | (c == '1' ? 1 : 0);             \
            __mask = (__mask << 1) | (c == '?' ? 0 : 1);           \
            __shift = (c == '?' ? __shift + 1 : 0);                \
        }                                                          \
    }

/* #define macro(i)
    if ((i) >= len)
        goto finish;
    else {
        char c = str[i];
        if (c != ' ') {
            Assert(c == '0' || c == '1' || c == '?',
                   "invalid character '%c' in pattern string", c);
            __key = (__key << 1) | (c == '1' ? 1 : 0);
            __mask = (__mask << 1) | (c == '?' ? 0 : 1);
            __shift = (c == '?' ? __shift + 1 : 0);
        }
    }
*/

//! this part just repeat the 'macro' 64 times
//! so why doesn't use a 'for' ?

/*
! maybe can change it like this...
for(int i = 0; i < len; i++){
    char c = str[i];
    if(c != ' '){
        Assert(c == '0' || c == '1' || c == '?', "invalid...");
        __key = (__key << 1) | (c == '1' ? 1 : 0);
        __mask = (__mask << 1) | (c == '?' ? 0 : 1);
        __shift = (c == '?' ? __shift + 1 : 0);
    }
}
*/
#define macro2(i) \
    macro(i);     \
    macro((i) + 1)
#define macro4(i) \
    macro2(i);    \
    macro2((i) + 2)
#define macro8(i) \
    macro4(i);    \
    macro4((i) + 4)
#define macro16(i) \
    macro8(i);     \
    macro8((i) + 8)
#define macro32(i) \
    macro16(i);    \
    macro16((i) + 16)
#define macro64(i) \
    macro32(i);    \
    macro32((i) + 32)
    macro64(0);
    panic("pattern too long");
#undef macro
finish:
    // get the final result by shift the values
    *key = __key >> __shift;
    *mask = __mask >> __shift;
    *shift = __shift;

    //! example input & output
    //! input str: ????1011011? len: 12
    //! _key = 000010110110  _mask = 000011111110  _shift = 1
    //! key  = 000001011011  mask  = 000001111111  shift  = 1
}

__attribute__((always_inline)) static inline void pattern_decode_hex(const char* str,
                                                                     int len,
                                                                     uint32_t* key,
                                                                     uint32_t* mask,
                                                                     uint32_t* shift) {
    uint32_t __key = 0, __mask = 0, __shift = 0;
    /* #define macro(i)\
    //     if ((i) >= len)\
    //         goto finish;\
    //     else {\
    //         char c = str[i];\
    //         if (c != ' ') {\
    //             Assert((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || c == '?',\
    //                    "invalid character '%c' in pattern string", c);\
    //             __key = (__key << 4) | (c == '?' ? 0 : (c >= '0' && c <= '9') ? c - '0'\
    // : c - 'a' + 10);\
    //             __mask = (__mask << 4) | (c == '?' ? 0 : 0xf);\
    //             __shift = (c == '?' ? __shift + 4 : 0);\
    //         }\
    //     }*/
    for (int i = 0; i < len; i++) {
        char c = str[i];
        if ((i) >= len)
            goto finish;
        if (c != ' ') {
            Assert((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || c == '?',
                   "invalid character '%c' in pattern string", c);
            __key = (__key << 4) | (c == '?' ? 0 : (c >= '0' && c <= '9') ? c - '0'
                                                                          : c - 'a' + 10);
            __mask = (__mask << 4) | (c == '?' ? 0 : 0xf);
            __shift = (c == '?' ? __shift + 4 : 0);
        }
    }
    // macro16(0);
    panic("pattern too long");
#undef macro
finish:
    *key = __key >> __shift;
    *mask = __mask >> __shift;
    *shift = __shift;
}

// --- pattern matching wrappers for decode ---
//! a macro to decode the instruction
//! it will call the pattern_decode, and then compare the instruction with the pattern
//! if matched, execute the body part
#define def_INSTR_raw(decode_fun, pattern, body)                   \
    do {                                                           \
        uint32_t key, mask, shift;                                 \
        decode_fun(pattern, STRLEN(pattern), &key, &mask, &shift); \
        if (((get_instr(s) >> shift) & mask) == key) {             \
            body;                                                  \
        }                                                          \
    } while (0)

//! this macro is easy to call the INSTR_raw
//! macro extend will to be like this..
/*
def_INSTR_IDTABW(pattern, id, tab, width) =>
{
    uint32_t key, mask, shift;
    decode_fun(pattern, STRLEN(pattern), &key, &mask, &shift);
    if(((get_instr(s)>>shift) & mask) == key) {
        decode_id(s, width);
        return table_tab(s);
    }
}
*/
#define def_INSTR_IDTABW(pattern, id, tab, width) \
    def_INSTR_raw(pattern_decode, pattern,        \
                  { concat(decode_, id)(s, width); return concat(table_, tab)(s); })

/*
def_INSTR_IDTAB(pattern, id, tab) =>
{
    uint32_t key, mask, shift;
    decode_fun(pattern, STRLEN(pattern), &key, &mask, &shift);
    if(((get_instr(s)>>shift) & mask) == key) {
        decode_id(s, 0);
        return table_tab(s);
    }
}
*/
#define def_INSTR_IDTAB(pattern, id, tab) def_INSTR_IDTABW(pattern, id, tab, 0)

#define def_INSTR_TABW(pattern, tab, width) \
    def_INSTR_IDTABW(pattern, empty, tab, width)

#define def_INSTR_TAB(pattern, tab) def_INSTR_IDTABW(pattern, empty, tab, 0)

#define def_hex_INSTR_IDTABW(pattern, id, tab, width) \
    def_INSTR_raw(pattern_decode_hex, pattern,        \
                  { concat(decode_, id)(s, width); return concat(table_, tab)(s); })
#define def_hex_INSTR_IDTAB(pattern, id, tab) def_hex_INSTR_IDTABW(pattern, id, tab, 0)
#define def_hex_INSTR_TABW(pattern, tab, width) def_hex_INSTR_IDTABW(pattern, empty, tab, width)
#define def_hex_INSTR_TAB(pattern, tab) def_hex_INSTR_IDTABW(pattern, empty, tab, 0)

#endif
