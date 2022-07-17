#include <monitor/sdb/sdb.h>
#include <cpu/cpu.h>
#include <isa.h>
#include <memory/paddr.h>
#include <readline/history.h>
#include <readline/readline.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
    //读取到的字符串buffer的初始化
    static char* line_read = NULL;
    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    //读取字符，读取前会输出一个nemu字符串
    //得注意该函数会动态分配内存，因此使用后需要释放
    line_read = readline("(nemu) ");

    //如果该指针不为空且内容也不为空～
    if (line_read && *line_read) {
        //将该行添加到历史记录中
        add_history(line_read);
    }

    //返回该行数据（C的问题似乎被解决了...所有内存申请都被放到下一次调用时释放了...）
    return line_read;
}

//输入c时执行的函数？
// c是继续执行被暂停的程序
static int cmd_c(char* args) {
    cpu_exec(-1);
    return 0;
}

//输入q执行的函数
static int cmd_q(char* args) {
    return -1;
}

//? myCodes begin ----------------------------------------------------------
//输入x执行的函数
static int cmd_x(char* args) {
    if (args == NULL)
        goto X_ARGS_ERROR;

    char** next_char = NULL;
    char* args_end = args + strlen(args);
    char* N = strtok(args, " ");
    char* EXPR = N + strlen(N) + 1;

    if (N == NULL || EXPR >= args_end)
        goto X_ARGS_ERROR;

    uint32_t n = strtol(N, next_char, 10);
    uint32_t addr = strtol(EXPR, next_char, 16);

    for (int i = 0; i < n; i++) {
        uint8_t res = paddr_read(addr + i, 1);
        printf("0x%08x  %016b\n", addr + i, res);
    }
    return 0;

X_ARGS_ERROR:
    printf("x command args error: x [N] [EXPR]\n");
    return 0;
}

static int cmd_si(char* args){
    size_t step = 1;
    if(args != NULL){
        step = atoi(args);
    }
    cpu_exec(step);
    return 0;
}

//输入info执行的函数
static int cmd_info(char* args) {
    if (args == NULL) {
        printf("info command args error: info [opt]\n");
        return 0;
    }
    switch (*args) {
        case 'r':
            isa_reg_display();
            break;
        case 'w':
            print_all_using_wp();
            break;
        default:
            break;
    }
    return 0;
}

static int cmd_p(char* args) {
    if (args == NULL) {
        printf("p command args error: p [expression]\n");
        return 0;
    }
    bool success = true;
    printf("%s = ", args);
    word_t res = expr(args, &success);
    if (success == false)
        printf("expr error\n");
    else
        printf("dec = %d  |  hex = 0x%08x  |  bin = %032b)\n", res, res, res);
    return 0;
}

static int cmd_w(char* args) {
    if (args == NULL) {
        printf("w command args error: w [expression]\n");
        return 0;
    }
    int no = alloc_a_wp(args);
    if(no != -1){
        print_wp_info(no);
    }
    return 0;
}

static int cmd_d(char* args){
    if (args == NULL) {
        printf("d command args error: d [WP no]\n");
        return 0;
    }
    int no = atoi(args);
    remove_a_wp(no);
    return 0;
}

//? myCodes end -----------------------------------------------------------

static int cmd_help(char* args);

static struct {
    const char* name;
    const char* description;
    int (*handler)(char*);
} cmd_table[] = {

//? myCodes begin ----------------------------------------------------------
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},

    // TODO: Add more commands
    //执行后N步
    {"si", "Run current program next N step and then stop, default N is 1", cmd_si},
    //输出reg or wp信息
    {"info",
     "SUBCMD can be 'r' or 'w',"
     "to print the state of registers or watch points",
     cmd_info},
    //输出具体内存地址的内容
    {"x",
     "Scan the memory: Get the value of the EXPR, and set it as the"
     "start of the address, output next N 4bytes with hex format",
     cmd_x},
    //表达式求值
    {"p", "Print the value of EXPR", cmd_p},
    //设置监视点
    {"w", "set watch point which will stop when EXPR changing", cmd_w},
    //删除监视点
    {"d", "delete the watch point whose id is N", cmd_d},
//? myCodes end -----------------------------------------------------------

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char* args) {
    /* extract the first argument */
    char* arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    } else {
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void sdb_set_batch_mode() {
    is_batch_mode = true;
}

void sdb_mainloop() {
    //是否是批处理模式？？？
    if (is_batch_mode) {
        //直接执行
        cmd_c(NULL);
        return;
    }

    //进入调试主循环,不断获取新的cmdline
    for (char* str; (str = rl_gets()) != NULL;) {
        //先取得了字符串尾的指针
        char* str_end = str + strlen(str);
        /*将字符串在第一个空格处的片段获取到*/
        char* cmd = strtok(str, " ");
        //如果没有字符串？说明没有输入，进入下一次循环
        if (cmd == NULL) {
            continue;
        }
        /* 假设cmd之后的字符串全是args,需要进一步的解析*/
        char* args = cmd + strlen(cmd) + 1;

        //如果args已经到结尾了，说明没有args
        if (args >= str_end) {
            args = NULL;
        }

#ifdef CONFIG_DEVICE
        extern void sdl_clear_event_queue();
        sdl_clear_event_queue();
#endif

        int i;
        for (i = 0; i < NR_CMD; i++) {
            //遍历比较cmd与对应cmd_table的表项，如果匹配，则退出该搜索执行循环
            if (strcmp(cmd, cmd_table[i].name) == 0) {
                //尝试执行对应表项的处理函数（将args传入），若异常则退出
                if (cmd_table[i].handler(args) < 0) {
                    return;
                }
                break;
            }
        }
        //如果搜索到结尾了，输出unknown command
        if (i == NR_CMD) {
            printf("Unknown command '%s'\n", cmd);
        }
    }
}

void init_sdb() {
    /* Compile the regular expressions.
       编译常用的表达式？ */
    init_regex();

    /* Initialize the watchpoint pool.
    初始化了一个断点池，结构比较简单的～ */
    init_wp_pool();
}
