#ifndef FTRACER_C
#define FTRACER_C

#include <elf.h>
#include <fcntl.h>
#include <ftracer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LIST_LENGTH 128

static size_t func_data_list_length = 0;

static int init_success_flag = 0;

typedef struct FuncDataItem {
    uint32_t addr;
    char func_name[64];
} FuncDataItem;

FuncDataItem func_symbol_table[MAX_LIST_LENGTH] = {};

FuncNode* new_node(uint32_t addr, const char* name) {
    FuncNode* res = (FuncNode*)malloc(sizeof(FuncNode));
    res->addr = addr;
    strncpy(res->func_name, name, 63);
    res->next = NULL;
    return res;
}

#define check_flag                                         \
    do {                                                   \
        if (!init_success_flag) {                          \
            fprintf(_this->fd, "FTRACER_INIT_ERROR...\n"); \
            return;                                        \
        }                                                  \
    } while (0)

#define DEBUG

#ifdef DEBUG
void output_func_symtab() {
    for (size_t i = 0; i < func_data_list_length; i++) {
        printf("0x%08x: %s\n", func_symbol_table[i].addr,
               func_symbol_table[i].func_name);
    }
}
#endif

int parse_elf_file(const char* name) {
    // open the elf file
    int fd = open(name, O_RDONLY);
    if (fd < 0)
        goto BAD_PARSE;
    // get the file info by std::stat
    struct stat st;
    if (fstat(fd, &st) < 0)
        goto BAD_PARSE;
    // map the file into memory
    uint8_t* mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mem == MAP_FAILED)
        goto BAD_PARSE;
    // get elf_header from mem
    Elf32_Ehdr* elf_header = (Elf32_Ehdr*)mem;
    // get sec_header from elf_header;
    Elf32_Shdr* sec_header = (Elf32_Shdr*)(mem + elf_header->e_shoff);
    // find the str_tab and the sym_tab
    uint8_t* str_tab = NULL;
    uint8_t* sym_tab = NULL;
    size_t sym_tab_size = 0;
    for (size_t i = 0; i < elf_header->e_shnum; i++) {
        Elf32_Shdr* sec = sec_header + i;
        if (sec->sh_type == SHT_STRTAB && str_tab == NULL) {
            str_tab = mem + sec->sh_offset;
        }
        if (sec->sh_type == SHT_SYMTAB) {
            sym_tab = mem + sec->sh_offset;
            sym_tab_size = sec->sh_size;
        }
    }
    // parse the symtab with strtab
    Elf32_Sym* sym_tab_item = (Elf32_Sym*)sym_tab;
    Elf32_Sym* sym_tab_end =
        (Elf32_Sym*)sym_tab + sym_tab_size / sizeof(Elf32_Sym);
    while (sym_tab_item < sym_tab_end) {
        if (ELF32_ST_TYPE(sym_tab_item->st_info) == STT_FUNC) {
            FuncDataItem* func = &func_symbol_table[func_data_list_length++];
            func->addr = sym_tab_item->st_value;
            strncpy(func->func_name, (char*)&(str_tab[sym_tab_item->st_name]),
                    63);
        }
        sym_tab_item++;
    }
    return 0;
BAD_PARSE:
    return -1;
}

int compare(const void* a, const void* b) {
    if (((FuncDataItem*)a)->addr > ((FuncDataItem*)b)->addr)
        return 1;
    return 0;
}

void init_func_tracer(const char* elf_name,
                      const char* log_file_name,
                      FuncTracer* _this) {
    _this->top = new_node(-1, "FUNC_START_POSITION");
    _this->depth = 0;

    _this->fd = fopen(log_file_name, "w");

    size_t parse_res = parse_elf_file(elf_name);
    qsort(func_symbol_table, func_data_list_length, sizeof(FuncDataItem),
          compare);

    if (_this->fd != NULL && !parse_res) {
        init_success_flag = 1;
#ifdef DEBUG
        printf("PARSE ELF SUCCESS, symtab len = %d\n",
               (int)func_data_list_length);
        output_func_symtab();
#endif
    }
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
    fprintf(_this->fd, "0x%08x:", src_addr);
    for (int i = 0; i < _this->depth; i++)
        fputs("| ", _this->fd);
    fprintf(_this->fd, "-call [%s at 0x%08x]\n", name, dst_addr);
    _this->depth += 1;
}

void ret_func(uint32_t src_addr, FuncTracer* _this) {
    FuncNode* old = _this->top;
    _this->top = old->next;
    // output the function info
    fprintf(_this->fd, "0x%08x:", src_addr);
    for (int i = 0; i < _this->depth; i++)
        fputs("| ", _this->fd);
    fprintf(_this->fd, "-ret [%s]\n", old->func_name);
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
    check_flag;
    size_t index = get_func_index_by_addr(dst_addr);
    // printf("try call!%d\n", (int)index);
    if (func_symbol_table[index].addr == dst_addr) {
        call_func(src_addr, index, _this);
    }
}

void try_ret_func(uint32_t dst_addr, uint32_t src_addr, FuncTracer* _this) {
    check_flag;
    puts("try ret!\n");
    size_t index = get_func_index_by_addr(dst_addr);
    while (_this->top->addr != func_symbol_table[index].addr) {
        ret_func(src_addr, _this);
    }
    ret_func(src_addr, _this);
}
#endif
