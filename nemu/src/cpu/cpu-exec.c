#include <cpu/cpu.h>
#include <cpu/difftest.h>
#include <cpu/exec.h>
#include <isa-all-instr.h>
#include <locale.h>
#include <monitor/sdb/sdb.h>

/* The assembly code of instructions executed is only output to the screen
  when the number of instructions executed is less than this value.
  This is useful when you use the `si' command.
  You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

CPU_state cpu = {};

uint64_t g_nr_guest_instr = 0;
static uint64_t g_timer = 0;  // unit: us
static bool g_print_step = false;
const rtlreg_t rzero = 0;
rtlreg_t tmp_reg[4];
IRingBuf iringbuf;

void device_update();
void fetch_decode(Decode* s, vaddr_t pc);

// trace is to record the running info, difftest is to test whether the
// watchpoint changed
static void trace_and_difftest(Decode* _this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
    if (ITRACE_COND)
        log_write("%s\n", _this->logbuf);
#endif
    if (g_print_step) {
        IFDEF(CONFIG_ITRACE, puts(_this->logbuf));
    }
    IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));

    // TODO: check the wp list, halt if expr-value change
    //? myCodes begin ----------------------------------------------------------
    int cnt = 0;
    int* list = check_list();
    while (list[cnt] != -1) {
        print_changed_info(list[cnt++]);
    }
    if (cnt != 0)
        nemu_state.state = NEMU_STOP;
    //? myCodes end ------------------------------------------------------------
}

#include <isa-exec.h>

#define FILL_EXEC_TABLE(name) [concat(EXEC_ID_, name)] = concat(exec_, name),

// TODO unfinish: we need trace the function place by this table
//? myCodes begin ----------------------------------------------------------
static const void* g_exec_table[TOTAL_INSTR] = {
    MAP(INSTR_LIST, FILL_EXEC_TABLE)};

// this macro extend to:
// g_exec_table[TOTAL_INSTR] = { INSTR_LIST(FILL_EXEC_TABLE) } =>
// {
//    [id] = function,
// }
//! then find that the function is in the ldst.h

//? myCodes end ------------------------------------------------------------

// as the name, it's the cpu's routine
static void fetch_decode_exec_updatepc(Decode* s) {
    // fetch instruction and decode? the result may saved in Decode* s
    // after RTFSC, we find that it just update some value in Decode
    // and what's better, simulator doesn't implement a pipeline, we don't need
    // to think about some fucking things about it
    fetch_decode(s, cpu.pc);
    //! this is very important, EHelper record the handler of instruction, it
    //! updated in fetch_decode
    s->EHelper(s);
    // update the cpu's pc
    cpu.pc = s->dnpc;
}

static void statistic() {
    IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%ld", "%'ld")
    Log("host time spent = " NUMBERIC_FMT " us", g_timer);
    Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_instr);
    if (g_timer > 0)
        Log("simulation frequency = " NUMBERIC_FMT " instr/s",
            g_nr_guest_instr * 1000000 / g_timer);
    else
        Log("Finish running in less than 1 us and can not calculate the "
            "simulation frequency");
}

void assert_fail_msg() {
    output_iringbuf(&iringbuf);
    isa_reg_display();
    statistic();
}

void write_instr_log(char* buffer_list[], Decode* s) {
    if (*buffer_list == NULL)
        return;
    char buffer[128] = {0};
    char* p = buffer;
    p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
    // ilen maybe means instruction length
    int ilen = s->snpc - s->pc;  // always 4 bytes in RISCV32
    uint8_t* instr = (uint8_t*)&s->isa.instr.val;
    for (int i = 0; i < ilen; i++) {
        p += snprintf(p, 4, " %02x", instr[i]);
    }
    // this part is for align the longest instr with the shortest instr
    int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
    int space_len = ilen_max - ilen;
    if (space_len < 0)
        space_len = 0;
    space_len = space_len * 3 + 1;
    memset(p, ' ', space_len);
    p += space_len;
    // output the disassemble code to log... before call it, define it first~
    // in fact, it's a function provide by LLVM compiler... implement in file
    // 'disasm.cc'
    void disassemble(char* str, int size, uint64_t pc, uint8_t* code,
                     int nbyte);
    disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
                MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc),
                (uint8_t*)&s->isa.instr.val, ilen);
    for(size_t i = 0; i < 2; i++){
        strncpy(buffer_list[i], buffer, 128);
    }
}

// fetch a instruction from pc, decode and save the result in s
void fetch_decode(Decode* s, vaddr_t pc) {
    //! save pc's value; but why?
    s->pc = pc;
    s->snpc = pc;

    //! this function belong to a concrete ISA
    //! so the decode must save PC first
    //! we put the code here for convinience
    // *int isa_fetch_decode(Decode *s) {
    // ~    // save the instruction in decode.isa.instr.val
    // ~    // the instr_fetch will modify the snpc and return a 32bit
    // instruction
    // *    s->isa.instr.val = instr_fetch(&s->snpc, 4);
    // *    int idx = table_main(s);
    // *    return idx;
    // *}
    //! we can find that the idx is the index of the handler for an instruction
    int idx = isa_fetch_decode(s);

    //! suppose dnpc is equal to snpc... it seems like we can only use one pc...
    s->dnpc = s->snpc;
    //* execute helper save the handler of instruction
    //* so that we can call it in fetch_decode_exec_updatepc function
    s->EHelper = g_exec_table[idx];

    char* buffer_list[] = {
        get_next_buf(&iringbuf),
#ifdef CONFIG_ITRACE
        s->logbuf,
#endif     
    };
    write_instr_log(buffer_list,s);
}


/* Simulate how the CPU works. */
//! n is the num of step the cpu will exec, if give -1, it will be the largest
//! unsigned number
void cpu_exec(uint64_t n) {
    g_print_step = (n < MAX_INSTR_TO_PRINT);
    // judge the current state of nemu, if END or ABORT, ask user to reboot
    switch (nemu_state.state) {
        case NEMU_END:
        case NEMU_ABORT:
            printf(
                "Program execution has ended. To restart the program, exit "
                "NEMU and run again.\n");
            return;
        // if not abort or end, it can only be stop, so can change the state to
        // RUNNING
        default:
            nemu_state.state = NEMU_RUNNING;
    }
    // record when the cpu start to work
    uint64_t timer_start = get_time();
    // simulate a virtual Decoder to parse the Instruction
    Decode s;
    for (; n > 0; n--) {
        // isa_reg_display();
        // cpu's routine ...
        fetch_decode_exec_updatepc(&s);
        //! what's this... it seems like a counter to how many instruction has
        //! been exec
        g_nr_guest_instr++;
        // debug function, check whether a watch point has change
        trace_and_difftest(&s, cpu.pc);
        // trace_and_difftest function may change the state of the CPU
        if (nemu_state.state != NEMU_RUNNING)
            break;
        // some other things... at least now it useless
        IFDEF(CONFIG_DEVICE, device_update());
    }

    // record the stop time
    uint64_t timer_end = get_time();
    // get time cost
    g_timer += timer_end - timer_start;

    // now check the state of the simulator
    switch (nemu_state.state) {
        //! why the state can be RUNNING?
        case NEMU_RUNNING:
            // turn the state to STOP...
            nemu_state.state = NEMU_STOP;
            break;

        // if END or ABORT, alert user
        case NEMU_END:
        case NEMU_ABORT:
            Log("nemu: %s at pc = " FMT_WORD,
                (nemu_state.state == NEMU_ABORT
                     ? ASNI_FMT("ABORT", ASNI_FG_RED)
                     : (nemu_state.halt_ret == 0
                            ? ASNI_FMT("HIT GOOD TRAP", ASNI_FG_GREEN)
                            : ASNI_FMT("HIT BAD TRAP", ASNI_FG_RED))),
                nemu_state.halt_pc);

        //! this function for what ? ?
        case NEMU_QUIT:
            statistic();
    }
}
