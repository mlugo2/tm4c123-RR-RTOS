#include "osKernel.h"

int count0, count1, count2;

void Task0(void)
{
	count0 = 0;
	while(1)
	{
		count0++;
	}
}

void Task1(void)
{
	count1 = 0;
	while(1)
	{
		count1++;
	}
}

void Task2(void)
{
	count2 = 0;
	while(1)
	{
		count2++;
	}
}

#define QUANTA 10

int main(void)
{
	OS_Init();					// initialize, disable interrupts
	OS_AddThreads(&Task0, &Task1, &Task2);
	OS_Launch(QUANTA);	// interrupts enabled
}
