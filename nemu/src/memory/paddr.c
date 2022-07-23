#include <device/mmio.h>
#include <isa.h>
#include <memory/host.h>
#include <memory/paddr.h>

#if defined(CONFIG_TARGET_AM)
static uint8_t* pmem = NULL;
#else
// 一个大小128MB的内存
static uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};
#endif

uint8_t* guest_to_host(paddr_t paddr) {
    return pmem + paddr - CONFIG_MBASE;
}
paddr_t host_to_guest(uint8_t* haddr) {
    return haddr - pmem + CONFIG_MBASE;
}

static word_t pmem_read(paddr_t addr, int len) {
    word_t ret = host_read(guest_to_host(addr), len);
    return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
    host_write(guest_to_host(addr), len, data);
}

void init_mem() {
#if defined(CONFIG_TARGET_AM)
    pmem = malloc(CONFIG_MSIZE);
    assert(pmem);
#endif
#ifdef CONFIG_MEM_RANDOM
    //获取内存的指针
    uint32_t* p = (uint32_t*)pmem;
    int i;
    //把内存都随机化
    for (i = 0; i < (int)(CONFIG_MSIZE / sizeof(p[0])); i++) {
        p[i] = rand();
    }
#endif
    //输出日志，可以先不管
    Log("physical memory area [" FMT_PADDR ", " FMT_PADDR "]",
        (paddr_t)CONFIG_MBASE, (paddr_t)CONFIG_MBASE + CONFIG_MSIZE);
}

word_t paddr_read(paddr_t addr, int len) {
    if (likely(in_pmem(addr))) {
        word_t tmp = pmem_read(addr, len);
#ifdef CONFIG_MTRACE_COND
        if (MTRACE_COND)
            mat_write(
                "from 0x%08x , access 0x%08x , READ  %2d BYTES , val = %-12d "
                "0x%08x \n",
                cpu.pc, addr, len, tmp, tmp);
        return tmp;
#endif
    }
    MUXDEF(CONFIG_DEVICE, return mmio_read(addr, len),
           panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR
                 ", " FMT_PADDR ") at pc = " FMT_WORD,
                 addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
}

void paddr_write(paddr_t addr, int len, word_t data) {
    if (likely(in_pmem(addr))) {
        pmem_write(addr, len, data);
#ifdef CONFIG_MTRACE_COND
        if (MTRACE_COND)
            mat_write(
                "from 0x%08x , access 0x%08x , WRITE %2d BYTES , val = %-12d "
                "0x%08x \n",
                cpu.pc, addr, len, data, data);
#endif
        return;
    }
    MUXDEF(CONFIG_DEVICE, mmio_write(addr, len, data),
           panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR
                 ", " FMT_PADDR ") at pc = " FMT_WORD,
                 addr, CONFIG_MBASE, CONFIG_MBASE + CONFIG_MSIZE, cpu.pc));
}
