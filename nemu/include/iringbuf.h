#ifndef IRINGBUF_H
#define IRINGBUF_H

#include <stdint.h>

#define IRINGBUF_LENGTH 128
#define IRINGBUF_NUM 32

typedef struct IRingBuf {
    char buffer[IRINGBUF_NUM][IRINGBUF_LENGTH];
    uint8_t cur_pos;
} IRingBuf;

char* get_next_buf(IRingBuf*);

void output_iringbuf(IRingBuf*);

#endif