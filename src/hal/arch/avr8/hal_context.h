/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_context.h
 * @brief hal context save / restore
 *
 * @todo nothing
 */

#define AVR8_PUSH_ALL_REGS                                                                                        \
	"push r0 \n\t"                                                                                           \
	"in r0, __SREG__ \n\t"                                                                                   \
	"push r0 \n\t"                                                                                           \
	"push r1 \n\t"                                                                                           \
	"push r2 \n\t"                                                                                           \
	"push r3 \n\t"                                                                                           \
	"push r4 \n\t"                                                                                           \
	"push r5 \n\t"                                                                                           \
	"push r6 \n\t"                                                                                           \
	"push r7 \n\t"                                                                                           \
	"push r8 \n\t"                                                                                           \
	"push r9 \n\t"                                                                                           \
	"push r10 \n\t"                                                                                          \
	"push r11 \n\t"                                                                                          \
	"push r12 \n\t"                                                                                          \
	"push r13 \n\t"                                                                                          \
	"push r14 \n\t"                                                                                          \
	"push r15 \n\t"                                                                                          \
	"push r16 \n\t"                                                                                          \
	"push r17 \n\t"                                                                                          \
	"push r18 \n\t"                                                                                          \
	"push r19 \n\t"                                                                                          \
	"push r20 \n\t"                                                                                          \
	"push r21 \n\t"                                                                                          \
	"push r22 \n\t"                                                                                          \
	"push r23 \n\t"                                                                                          \
	"push r24 \n\t"                                                                                          \
	"push r25 \n\t"                                                                                          \
	"push r26 \n\t"                                                                                          \
	"push r27 \n\t"                                                                                          \
	"push r28 \n\t"                                                                                          \
	"push r29 \n\t"                                                                                          \
	"push r30 \n\t"                                                                                          \
	"push r31 \n\t"

#define AVR8_POP_ALL_REGS                                                                                         \
	"pop r31 \n\t"                                                                                           \
	"pop r30 \n\t"                                                                                           \
	"pop r29 \n\t"                                                                                           \
	"pop r28 \n\t"                                                                                           \
	"pop r27 \n\t"                                                                                           \
	"pop r26 \n\t"                                                                                           \
	"pop r25 \n\t"                                                                                           \
	"pop r24 \n\t"                                                                                           \
	"pop r23 \n\t"                                                                                           \
	"pop r22 \n\t"                                                                                           \
	"pop r21 \n\t"                                                                                           \
	"pop r20 \n\t"                                                                                           \
	"pop r19 \n\t"                                                                                           \
	"pop r18 \n\t"                                                                                           \
	"pop r17 \n\t"                                                                                           \
	"pop r16 \n\t"                                                                                           \
	"pop r15 \n\t"                                                                                           \
	"pop r14 \n\t"                                                                                           \
	"pop r13 \n\t"                                                                                           \
	"pop r12 \n\t"                                                                                           \
	"pop r11 \n\t"                                                                                           \
	"pop r10 \n\t"                                                                                           \
	"pop r9 \n\t"                                                                                            \
	"pop r8 \n\t"                                                                                            \
	"pop r7 \n\t"                                                                                            \
	"pop r6 \n\t"                                                                                            \
	"pop r5 \n\t"                                                                                            \
	"pop r4 \n\t"                                                                                            \
	"pop r3 \n\t"                                                                                            \
	"pop r2 \n\t"                                                                                            \
	"pop r1 \n\t"                                                                                            \
	"pop r0 \n\t"                                                                                            \
	"out __SREG__, r0 \n\t"                                                                                  \
	"pop r0	\n\t"

static inline __attribute__((always_inline)) void hal_contextSave()
{
	asm volatile( AVR8_PUSH_ALL_REGS );
}

static inline __attribute__((always_inline)) void hal_contextRestore()
{
	asm volatile( AVR8_POP_ALL_REGS );
}

static inline __attribute__((always_inline)) void hal_returnFromInterupt()
{
	asm volatile("reti \n\t");
}

static inline __attribute__((always_inline)) void hal_setGlobalInterupt()
{
	asm volatile("sei \n\t");
}

static inline __attribute__((always_inline)) void hal_clearGlobalInterupt()
{
	asm volatile("cli \n\t");
}
