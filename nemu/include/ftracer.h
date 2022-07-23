#ifndef FTRACER_H
#define FTRACER_H

#include <stdint.h>
#include <stdio.h>

typedef struct FuncNode {
    uint32_t addr;
    char func_name[64];
    FuncNode* next;
} FuncNode;

typedef struct FuncTracer {
    FuncNode* top;
    uint16_t depth;
} FuncTracer;

void init_func_tracer(const char* elf_name, FuncTracer* _this);

size_t get_func_index_by_addr(uint32_t addr);

void try_call_func(uint32_t dst_addr, uint32_t src_addr, FuncTracer* _this);

void try_ret_func(uint32_t dst_addr, uint32_t src_addr, FuncTracer* _this);

// void call_func(uint32_t addr, FuncTracer* _this);

// void ret_func(uint32_t addr, FuncTracer* _this);

#endif