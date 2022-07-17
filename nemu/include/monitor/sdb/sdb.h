#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char* e, bool* success);

int alloc_a_wp(char* expression);

void remove_a_wp(int no);

void print_wp_info(int no);

void print_changed_info(int no);

int* check_list();

word_t* get_before_value();

void print_all_using_wp();

int* check_list();

word_t* get_before_value();

#endif
