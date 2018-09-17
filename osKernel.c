#include "osKernel.h"

// Assembler function prototypes
uint32_t StartCritical(void);
void EndCritical(uint32_t);
void DisableInterrupts(void);
void StartOS(void);

#define NUM_THREADS		3		// maximum number of threads
#define STACK_SIZE		100 // number of 32-bit words in stack
#define BUS_FREQ			16000000
uint32_t MILLIS_PRESCALER;

#define SYSPRI3					(*((volatile uint32_t *)0xe000ed20))

typedef struct tcb {
	uint32_t *sp;						// pointer to stack, valid for threads not running
	struct tcb *next;				// linked-list pointer	
} tcbType;

tcbType tcbs[NUM_THREADS];
tcbType *RunPt;
uint32_t Stacks[NUM_THREADS][STACK_SIZE];


void SetInitialStack(int i)
{
	tcbs[i].sp = &Stacks[i][STACK_SIZE-16];		// thread stack pointer
	Stacks[i][STACK_SIZE-1] = 0x01000000;			// Thumb bit
	Stacks[i][STACK_SIZE-3] = 0x14141414;			// R14
	Stacks[i][STACK_SIZE-4] = 0x12121212;			// R12
	Stacks[i][STACK_SIZE-5] = 0x03030303;			// R3
	Stacks[i][STACK_SIZE-6] = 0x02020202;			// R2
	Stacks[i][STACK_SIZE-7] = 0x01010101;			// R1
	Stacks[i][STACK_SIZE-8] = 0x00000000;			// R0
	Stacks[i][STACK_SIZE-9] = 0x11111111;			// R11
	Stacks[i][STACK_SIZE-10] =0x10101010;			// R10
	Stacks[i][STACK_SIZE-11] =0x09090909;			// R9
	Stacks[i][STACK_SIZE-12] =0x08080808;			// R8
	Stacks[i][STACK_SIZE-13] =0x07070707;			// R7
	Stacks[i][STACK_SIZE-14] =0x06060606;			// R6
	Stacks[i][STACK_SIZE-15] =0x05050505;			// R5
	Stacks[i][STACK_SIZE-16] =0x04040404;			// R4
}

int OS_AddThreads(void(*task0)(void), void(*task1)(void), void(*task2)(void)) {

	uint32_t status;
	// TODO: implement StartCritical in asm
	status = StartCritical();
	
	tcbs[0].next = &tcbs[1];									// task0 points to task1
	tcbs[1].next = &tcbs[2];									// task1 points to task2
	tcbs[2].next = &tcbs[0];									// task2 points to task0
	SetInitialStack(0);	Stacks[0][STACK_SIZE-2] = (uint32_t)(task0);	// PC
	SetInitialStack(1);	Stacks[1][STACK_SIZE-2] = (uint32_t)(task1);	// PC
	SetInitialStack(2);	Stacks[2][STACK_SIZE-2] = (uint32_t)(task2);	// PC
	RunPt = &tcbs[0];													// thread 0 will run first
	// TODO: implement EndCritical() in asm
	EndCritical(status);
	
	return 1;																	// return successful
}

void OS_Init(void)
{
	// TODO: Implement DisableInterrupts() in asm
	DisableInterrupts();
	MILLIS_PRESCALER = BUS_FREQ / 1000;
}

void OS_Launch(uint32_t TimeSlice)
{
	SysTick->CTRL = 0;												// disable SysTick during setup
	SysTick->VAL = 0;
	SysTick->LOAD = (TimeSlice*MILLIS_PRESCALER)-1;
	SYSPRI3 = (SYSPRI3&0x00FFFFFF) | 0xe0000000;	// Priority 7
	SysTick->CTRL = 0x00000007;
	// TODO: Implement StartOS in asm
	StartOS();
}
