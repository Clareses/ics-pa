#include <stdio.h>
#include <monitor/sdb/sdb.h>

// provide 32 WP
#define NR_WP 32

typedef struct watchpoint {
    int NO;                   // the ID of this WP
    struct watchpoint* next;  // connect WP as a linkedList

    // TODO: Add more members if necessary
    //? myCodes begin -------------------------------------
    int is_used;
    char* expression;   // record the expression of wp
    word_t last_value;  // record the last time value of expression
    //? myCodes end ---------------------------------------

} WP;

// watchpoint pool, all the WP are in it
static WP wp_pool[NR_WP] = {};

// headers of used and unused linkList
static WP *head = NULL, *free_ = NULL;

// init the WP pool
void init_wp_pool() {
    int i;
    // overview the hole pool
    for (i = 0; i < NR_WP; i++) {
        // update the NO
        wp_pool[i].NO = i;
        wp_pool[i].is_used = 0;
        // connect pre to next as a linkedList (the free list)
        wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    }

    // init the headers
    head = NULL;
    free_ = wp_pool;
}

// TODO: Implement the functionality of watchpoint
//? myCodes begin ----------------------------------------------------------

static int change_no_list[32] = {};
static word_t before_val_list[32] = {};

// this function add a watch point into used list from unused list
WP* new_wp(const char* expression) {
    if (free_ == NULL) {
        printf("There is no free watch point to use.\n");
        return NULL;
    }
    WP* wp = free_;
    free_ = wp->next;
    wp->next = head;
    head = wp;
    wp->expression = (char*)malloc(sizeof(expression));
    wp->is_used = 1;
    strcpy(wp->expression, expression);
    return head;
}

// this function delete a watch point from used list
void free_wp(WP* wp) {
    if (head->NO == wp->NO) {
        head = wp->next;
        wp->next = free_;
        free_ = wp;
        goto FREE_SUCCESS;
    }
    WP* ptr = head;
    while (ptr != NULL && ptr->next->NO != wp->NO)
        ptr = ptr->next;
    if (ptr == NULL) {
        printf("No match watch point to delete.\n");
        return;
    }
    ptr->next = wp->next;
    wp->next = free_;
    wp->is_used = 0;
    free_ = wp;
FREE_SUCCESS:
    free(wp->expression);
    printf("Free watch point success.\n");
    return;
}

//! the interface for caller

// alloc a wp, return the wp no;
int alloc_a_wp(char* expression) {
    WP* wp = new_wp(expression);
    if (wp) {
        return wp->NO;
    }
    return -1;
}

// delete a wp by no;
void remove_a_wp(int no) {
    WP* wp = wp_pool + no;
    if (no > NR_WP || wp->is_used == 0) {
        printf("No using wp's no is %d\n", no);
        return ;
    }
    free_wp(wp);
}

// print the info of a wp by num
void print_wp_info(int no) {
    WP* wp = wp_pool + no;
    if (no > NR_WP || wp->is_used == 0) {
        printf("No using wp's no is %d\n", no);
        return ;
    }
    printf("No.%d  |  expr = %s  |  value = (dec)%d , (hex)0x%x , (bin)%032b \n",
           wp->NO, wp->expression, wp->last_value, wp->last_value, wp->last_value);
}

// print changed logs
void print_changed_info(int no) {
    WP* wp = wp_pool + no;
    if (no > NR_WP || wp->is_used == 0) {
        printf("No using wp's no is %d\n", no);
        return ;
    }
    printf(
        "No.%d  |  expr = %s\n"
        "    before :  value = (dec)%12d , (hex)0x%08x , (bin)%032b \n"
        "    now    :  value = (dec)%12d , (hex)0x%08x , (bin)%032b \n",
        wp->NO, wp->expression, before_val_list[no], before_val_list[no], before_val_list[no], wp->last_value, wp->last_value, wp->last_value);
}

// check if value changed
int* check_list() {
    memset(change_no_list, -1, sizeof(change_no_list));
    memset(before_val_list, 0, sizeof(before_val_list));
    bool success = true;
    int cnt = 0;
    for (WP* wp = head; wp != NULL; wp = wp->next) {
        word_t value = expr(wp->expression, &success);
        if (success && value != wp->last_value) {
            change_no_list[cnt++] = wp->NO;
            change_no_list[wp->NO] = wp->last_value;
            wp->last_value = value;
        }
    }
    return change_no_list;
}

word_t* get_before_value() {
    return before_val_list;
}

void print_all_using_wp(){
    for (WP* wp = head; wp != NULL; wp = wp->next) {
        print_wp_info(wp->NO);
    }
}

//? myCodes end -----------------------------------------------------------