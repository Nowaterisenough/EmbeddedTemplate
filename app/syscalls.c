/**
 * @file    syscalls.c
 * @brief   Minimal newlib syscalls implementation
 */

#include <sys/stat.h>
#include <errno.h>

/* 堆的起始和结束地址（由链接脚本定义） */
extern char _end;      /* 由链接脚本定义的堆起始 */
extern char _estack;   /* 由链接脚本定义的栈顶 */

/**
 * _sbrk - 简单的堆分配实现
 */
void *_sbrk(int incr)
{
    static char *heap_end = NULL;
    char *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    /* 简单检查是否溢出 (预留4KB给栈) */
    if (heap_end + incr > (&_estack - 4096)) {
        errno = ENOMEM;
        return (void *)-1;
    }

    heap_end += incr;
    return (void *)prev_heap_end;
}
