#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//! TODO wait for test

// return >0 means true, 0 means not a %, -1 means can't recognize
int check_whether_arg_symbol(const char* begin, const char* target) {
    // if(*target != '%') return 0;
    if (*target == '%' && (target == begin || *(target - 1) != '\\')) {
        switch (*(target + 1)) {
            case 's':
                return 1;
            case 'd':
                return 2;
            default:
                return -1;
        }
    }
    return 0;
}

//  parse val to dec number and write to the dst, n is the
// buffer length of the dst, return the write length
size_t write_d_type(char* dst, int val, size_t n) {
    char num_str[32] = {0};
    size_t i;
    for (i = 31; val != 0; i--) {
        num_str[i] = val % 10 + '0';
        val /= 10;
    }
    char* num_begin = num_str + i + 1;
    while (++i < 32 && n--) {
        *(dst++) = num_str[i];
    }
    return num_str + i - num_begin;
}

size_t write_s_type(char* dst, char* str, size_t n) {
    size_t i = 0;
    while (i < n && str[i] != 0) {
        dst[i] = str[i];
        i++;
    }
    return i;
}

int out_to_the_buffer(char* buffer,
                      size_t len,
                      const char* fmt,
                      va_list valist) {
    size_t i;
    const char* ptr = fmt;
    for (i = 0; i < len - 1 && *ptr != 0; i++) {
        switch (check_whether_arg_symbol(fmt, ptr)) {
            case 0:
                buffer[i] = *ptr;
                break;
            case 1:
                i += write_s_type(buffer + i, va_arg(valist, char*), len - i) - 1;
                ptr++;
                break;
            case 2:
                i += write_d_type(buffer + i, va_arg(valist, int), len - i) - 1;
                ptr++;
                break;
            default:
                return -1;
        }
        ptr++;
    }
    buffer[i] = 0;
    return i;
}

int snprintf(char* out, size_t n, const char* fmt, ...) {
    va_list valist;
    va_start(valist, fmt);
    size_t ret = out_to_the_buffer(out,n,fmt,valist);
    va_end(valist);
    return ret;
}

int printf(const char* fmt, ...) {
    panic("Not implemented");
}

int vsprintf(char* out, const char* fmt, va_list ap) {
    panic("Not implemented");
}

int sprintf(char* out, const char* fmt, ...) {
    va_list valist;
    va_start(valist, fmt);
    size_t ret = out_to_the_buffer(out,-1,fmt,valist);
    va_end(valist);
    return ret;
}

int vsnprintf(char* out, size_t n, const char* fmt, va_list ap) {
    panic("Not implemented");
}

#endif
