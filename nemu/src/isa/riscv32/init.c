#include <isa.h>
#include <memory/paddr.h>

// this is not consistent with uint8_t
// but it is ok since we do not access the array directly
static const uint32_t img [] = {
  0x800002b7,  // lui t0,0x80000   lui -> load unsigned int
  0x005282b3,  // add t0, t0, t0
  0x0002a023,  // sw  zero,0(t0)   sw  -> set word
  0x0002a503,  // lw  a0,0(t0)     lw  -> load word
  0x0000006b,  // nemu_trap        ????
};

static void restart() {
  /* Set the initial program counter. */
  cpu.pc = RESET_VECTOR;

  /* The zero register is always 0. */
  cpu.gpr[0]._32 = 0;
}

void init_isa() {
  /* Load built-in image.
  载入built-in镜像？ memcpy是将img（不知道是啥）复制到了内存中（是哪一段内存呢）
  将函数和宏展开，RESET_VECOTR是一个地址（config中配置了该位是基于MEMORY_BASE
  上添加了一个RESET_OFFSET），通过阅读，可以发现该地址就是该软件计算机开机时执行
  第一句指令的位置（见restart函数）。此外，img的作用也看出来了，是开机时执行的几
  句机器指令！（见img的定义中的注释）*/
  memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));

  /* Initialize this virtual computer system. 
  重启（将pc设置为重启后的地址，cpu的寄存器堆中的x0(riscv中一直为0的寄存器)置0）*/
  restart();
}
