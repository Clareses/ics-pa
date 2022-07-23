#ifndef FTRACER_C
#define FTRACER_C

#include <elf.h>
#include <ftracer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIST_LENGTH 128

static size_t func_data_list_length = 0;

typedef struct FuncDataItem {
    uint32_t addr;
    char func_name[64];
} FuncDataItem;

FuncDataItem func_symbol_table[MAX_LIST_LENGTH] = {};

FuncNode* new_node(uint32_t addr, const char* name) {
    FuncNode* res = (FuncNode*)malloc(sizeof(FuncNode));
    res->addr = addr;
    strncpy(res->func_name, name, 64);
    res->next = NULL;
    return res;
}

void init_func_tracer(const char* elf_name, FuncTracer* _this) {
    _this->top = new_node(-1, "FUNC_START_POSITION");
    _this->depth = 0;
    // TODO read the elf file, and add the info into the funcdatalist, and sort
    // it by addr
}

size_t get_func_index_by_addr(uint32_t addr) {
    // we choose the linear search to get result
    // it require the list has been sorted
    size_t i = 0;
    while (i < func_data_list_length && func_symbol_table[i].addr > addr) {
        i++;
    }
    return i;
}

void call_func(uint32_t src_addr, size_t table_index, FuncTracer* _this) {
    uint32_t dst_addr = func_symbol_table[table_index].addr;
    char* name = func_symbol_table[table_index].func_name;
    FuncNode* new = new_node(dst_addr, name);
    new->next = _this->top;
    _this->top = new;
    // output the function info
    printf("0x%08x:", src_addr);
    for (int i = 0; i < _this->depth; i++)
        puts("| ");
    printf("-call [%s at 0x%08x]\n", name, dst_addr);
    _this->depth += 1;
}

void ret_func(uint32_t src_addr ,FuncTracer* _this) {
    FuncNode* old = _this->top;
    _this->top = old->next;
    // output the function info
    printf("0x%08x:", src_addr);
    for (int i = 0; i < _this->depth; i++)
        puts("| ");
    printf("-ret [%s]\n", old->func_name);
    _this->depth -= 1;
    free(old);
}

// int is_call_function(uint32_t dst_addr, FuncTracer* _this) {
//     size_t list_index = get_func_index_by_addr(dst_addr);
//     if(_this->top->addr != func_data_list[list_index].addr)
//         return 1;
//     return 0;
// }

void try_call_func(uint32_t dst_addr, uint32_t src_addr, FuncTracer* _this) {
    size_t index = get_func_index_by_addr(dst_addr);
    if (func_symbol_table[index].addr == dst_addr) {
        call_func(dst_addr, func_symbol_table[index].func_name, _this);
    }
}

void try_ret_func(uint32_t dst_addr, uint32_t src_addr, FuncTracer* _this) {
    size_t index = get_func_index_by_addr(dst_addr);
    while (_this->top->addr != func_symbol_table[index].addr) {
        ret_func(src_addr, _this);
    }
    ret_func(src_addr, _this);
}
#endif
