#ifndef IRINGBUF_C
#define IRINGBUF_C

#include <iringbuf.h>
#include <stdio.h>

char* get_next_buf(IRingBuf* iringbuf) {
    return iringbuf->buffer[(iringbuf->cur_pos++) % IRINGBUF_NUM];
}

void output_iringbuf(IRingBuf* iringbuf) {
    puts("\nTHE INSTR RING BUFFER DATA:\n");
    for (size_t i = 0; i < IRINGBUF_NUM; i++) {
        if (i == iringbuf->cur_pos)
            printf("->");
        if (iringbuf->buffer[i][0] != 0)
            puts(iringbuf->buffer[i]);
    }
}

#endif