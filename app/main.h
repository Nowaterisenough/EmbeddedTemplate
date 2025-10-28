#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* 统一只包含板级头，由板选择正确系列的 HAL/CMSIS 头 */
#include "board.h"

    /* 导出函数 */
    void Error_Handler(void);

#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */