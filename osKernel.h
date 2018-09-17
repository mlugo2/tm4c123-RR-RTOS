#ifndef _OS_KERNEL_H
#define _OS_KERNEL_H

#include<stdint.h>
#include "TM4C123GH6PM.h"

void SetInitialStack(int i);
int OS_AddThreads(void(*)(void), void(*)(void), void(*)(void));
void OS_Init(void);
void OS_Launch(uint32_t);

#endif
