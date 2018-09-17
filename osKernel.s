				area |.text|, code, readonly, align=2
				thumb
				; Variables from C files
				extern RunPt
				; Functions sent to C files
				export SysTick_Handler
				export StartCritical
				export EndCritical
				export DisableInterrupts
				export StartOS
					
SysTick_Handler			; Saves r0-r3, r12, lr, pc, psr
	cpsid I				; prevent interrupts during switch
	push {r4-r11}		; save remaining reg r4-r11
	ldr r0, =RunPt		; r0 points to RunPt, old thread
	ldr r1, [r0]		; r1 = RunPt
	str sp, [r1]		; save sp into TCB
	ldr r1, [r1,#4]		; r1 = RunPt->next
	str r1, [r0]		; RunPt = r1
	ldr sp, [r1]		; new thread sp; SP = RunPt->sp;
	pop {r4-r11}		; restore regs r4-r11
	cpsie I				; tasks run with interrupts enabled
	bx lr				; restore r0-r3, r12, lr, pc, psr
	
;**************** StartCritical *********************
; make a copy of previous I bit, disable interrupts
; inputs: none		outputs: previous I bit
StartCritical
	mrs r0, primask		; save old status
	cpsid I				; mask all (except faults)
	bx lr
	
;********************* EndCritical ***************************
; using the copy of previous I bit, restore I bit to previous value
; inputs: previous I bit 	outputs: none
EndCritical
	msr primask, r0
	bx lr
	
DisableInterrupts
	cpsid I
	bx lr
	
StartOS
	ldr r0, =RunPt		; currently running thread
	ldr r1, [r0]		; r1 = value of RunPt
	ldr sp, [r1]		; new thread SP; SP = RunPt->sp;
	pop {r4-r11}		; restore regs r4-r11
	pop {r0-r3}			; restore regs r0-r3
	pop {r12}
	add sp, sp, #4		; discard lr from initial stack
	pop {lr}			; start location
	add sp, sp, #4		; discard psr
	cpsie I				; enable interrupts at processor level
	bx lr				; start first thread
	
	align
	end