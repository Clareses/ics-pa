#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char* s) {
    size_t length = 0;
    while (*s != '\0') {
        s++;
        length++;
    }
    return length;
}

char* strcpy(char* dst, const char* src) {
    size_t i = 0;
    for (i = 0; *(src + i) != '\0'; i++) {
        *(dst + i) = *(src + i);
    }
    *(dst + i) = '\0';
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dst[i] = src[i];
    for (; i < n; i++)
        dst[i] = '\0';
    return dst;
}

char* strcat(char* dst, const char* src) {
    size_t i = 0;
    char* dst_end = dst + strlen(dst);
    for (i = 0; src[i] != 0; i++) {
        dst_end[i] = src[i];
    }
    dst_end[i] = '\0';
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while (*(s1++) != '\0' && *(s2++) != '\0') {
        if (*s1 != *s2)
            return *s1 - *s2;
    }
    if (*s1 != *s2) {
        return s1[0] == '\0' ? -1 : 1;
    }
    return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}

void* memset(void* s, int c, size_t n) {
    uint8_t* byte_ptr = (uint8_t*)s;
    for (size_t i = 0; i < n; i++) {
        byte_ptr[i] = c;
    }
    return s;
}

void* memmove(void* dst, const void* src, size_t n) {
    panic("Not implemented");
}

void* memcpy(void* out, const void* in, size_t n) {
    panic("Not implemented");
}

int memcmp(const void* s1, const void* s2, size_t n) {
    panic("Not implemented");
}

#endif
