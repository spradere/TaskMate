# 0 "sysCore/TaskMate.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "sysCore/TaskMate.c"
# 1 "/usr/local/avr/include/avr/io.h" 1 3
# 99 "/usr/local/avr/include/avr/io.h" 3
# 1 "/usr/local/avr/include/avr/sfr_defs.h" 1 3
# 126 "/usr/local/avr/include/avr/sfr_defs.h" 3
# 1 "/usr/local/avr/include/inttypes.h" 1 3
# 37 "/usr/local/avr/include/inttypes.h" 3
# 1 "/usr/local/lib/gcc/avr/14.1.0/include/stdint.h" 1 3 4
# 9 "/usr/local/lib/gcc/avr/14.1.0/include/stdint.h" 3 4
# 1 "/usr/local/avr/include/stdint.h" 1 3 4
# 125 "/usr/local/avr/include/stdint.h" 3 4

# 125 "/usr/local/avr/include/stdint.h" 3 4
typedef signed int int8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef signed int int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int uint16_t __attribute__ ((__mode__ (__HI__)));
typedef signed int int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int uint32_t __attribute__ ((__mode__ (__SI__)));

typedef signed int int64_t __attribute__((__mode__(__DI__)));
typedef unsigned int uint64_t __attribute__((__mode__(__DI__)));
# 146 "/usr/local/avr/include/stdint.h" 3 4
typedef int16_t intptr_t;




typedef uint16_t uintptr_t;
# 163 "/usr/local/avr/include/stdint.h" 3 4
typedef int8_t int_least8_t;




typedef uint8_t uint_least8_t;




typedef int16_t int_least16_t;




typedef uint16_t uint_least16_t;




typedef int32_t int_least32_t;




typedef uint32_t uint_least32_t;







typedef int64_t int_least64_t;






typedef uint64_t uint_least64_t;
# 217 "/usr/local/avr/include/stdint.h" 3 4
typedef int8_t int_fast8_t;




typedef uint8_t uint_fast8_t;




typedef int16_t int_fast16_t;




typedef uint16_t uint_fast16_t;




typedef int32_t int_fast32_t;




typedef uint32_t uint_fast32_t;







typedef int64_t int_fast64_t;






typedef uint64_t uint_fast64_t;
# 277 "/usr/local/avr/include/stdint.h" 3 4
typedef int64_t intmax_t;




typedef uint64_t uintmax_t;
# 10 "/usr/local/lib/gcc/avr/14.1.0/include/stdint.h" 2 3 4
# 38 "/usr/local/avr/include/inttypes.h" 2 3
# 77 "/usr/local/avr/include/inttypes.h" 3
typedef int32_t int_farptr_t;





typedef uint32_t uint_farptr_t;
# 127 "/usr/local/avr/include/avr/sfr_defs.h" 2 3
# 100 "/usr/local/avr/include/avr/io.h" 2 3
# 785 "/usr/local/avr/include/avr/io.h" 3
# 1 "/usr/local/avr/include/avr/portpins.h" 1 3
# 786 "/usr/local/avr/include/avr/io.h" 2 3

# 1 "/usr/local/avr/include/avr/common.h" 1 3
# 788 "/usr/local/avr/include/avr/io.h" 2 3

# 1 "/usr/local/avr/include/avr/version.h" 1 3
# 790 "/usr/local/avr/include/avr/io.h" 2 3






# 1 "/usr/local/avr/include/avr/fuse.h" 1 3
# 797 "/usr/local/avr/include/avr/io.h" 2 3


# 1 "/usr/local/avr/include/avr/lock.h" 1 3
# 800 "/usr/local/avr/include/avr/io.h" 2 3
# 2 "sysCore/TaskMate.c" 2
# 1 "/usr/local/avr/include/avr/interrupt.h" 1 3
# 3 "sysCore/TaskMate.c" 2

# 1 "/root/code/TaskMate/TaskMate_current/sysCore/TaskMate_define.h" 1
# 82 "/root/code/TaskMate/TaskMate_current/sysCore/TaskMate_define.h"

# 82 "/root/code/TaskMate/TaskMate_current/sysCore/TaskMate_define.h"
typedef struct
{
 uint8_t task_id;
 volatile uint16_t task_RTC;
 uint8_t *stack_pointer;
 uint8_t stack[256];
} task_table_t;
# 104 "/root/code/TaskMate/TaskMate_current/sysCore/TaskMate_define.h"
typedef struct
{
 uint8_t driver_id;
 uint8_t *driver_name;

 void (*setStatus)(uint8_t);
 uint8_t (*getStatus)(void);

 void (*init)(void);
 void (*start)(void);
 void (*stop)(void);
} driver_table_t;
# 5 "sysCore/TaskMate.c" 2
# 1 "/root/code/TaskMate/TaskMate_current/sysCore/taskCreate.h" 1



void taskCreate(void (*)(void), uint8_t);
# 6 "sysCore/TaskMate.c" 2
# 1 "/root/code/TaskMate/TaskMate_current/sysCore/macro_list_arg.h" 1
# 7 "sysCore/TaskMate.c" 2

# 1 "/root/code/TaskMate/TaskMate_current/drivers/timer1.h" 1



void timer1SetStatus(uint8_t);
uint8_t timer1GetStatus(void);
uint8_t *timer1GetName(void);

void timer1Init(void);
void timer1Start(void);
void timer1Stop(void);
# 9 "sysCore/TaskMate.c" 2
# 1 "/root/code/TaskMate/TaskMate_current/drivers/timer3.h" 1



void timer3SetStatus(uint8_t);
uint8_t timer3GetStatus(void);
uint8_t *timer3GetName(void);

void timer3Init(void);
void timer3Start(void);
void timer3Stop(void);
# 10 "sysCore/TaskMate.c" 2

# 1 "/root/code/TaskMate/TaskMate_current/tasks/task1.h" 1



void task1(void);
# 12 "sysCore/TaskMate.c" 2
# 1 "/root/code/TaskMate/TaskMate_current/tasks/task2.h" 1




void task2(void);
# 13 "sysCore/TaskMate.c" 2



driver_table_t driver_table[2];





task_table_t task_table[1];
uint8_t task_current=0;

int main(void)
{

 uint8_t i=0;
 taskCreate(task1,i++);
 taskCreate(task2,i++);
# 40 "sysCore/TaskMate.c"
 driver_table[0]=(driver_table_t)
 {
  .driver_id = 0,
  .driver_name = timer1GetName(),
  .setStatus = timer1SetStatus,
  .getStatus = timer1GetStatus,
  .init = timer1Init,
  .start = timer1Start,
  .stop = timer1Stop
 };

 driver_table[1]=(driver_table_t)
 {
  .driver_id = 1,
  .driver_name = timer3GetName(),
  .setStatus = timer3SetStatus,
  .getStatus = timer3GetStatus,
  .init = timer3Init,
  .start = timer3Start,
  .stop = timer3Stop
 };


 for(i=0;i<2;i++)
 {
  (*driver_table[i].setStatus)( (1 << 0) | (1 << 1) );
 }



 for(i=0;i<2;i++)
 {
  if( ((*driver_table[i].getStatus)() & (1 << 0) ) != 0 ){(*driver_table[i].init)();}
 }


 for(i=0;i<2;i++)
 {
  if( ((*driver_table[i].getStatus)() & (1 << 1) ) != 0 ){(*driver_table[i].start)();}
 }



 task_current=0;
 
# 84 "sysCore/TaskMate.c" 3
(*(volatile uint8_t *)((0x3D) + 0x20)) 
# 84 "sysCore/TaskMate.c"
   = (uint16_t)task_table[task_current].stack_pointer;
 asm volatile (
  "pop r31 \n\t" "pop r30 \n\t" "pop r29 \n\t" "pop r28 \n\t" "pop r27 \n\t" "pop r26 \n\t" "pop r25 \n\t" "pop r24 \n\t" "pop r23 \n\t" "pop r22 \n\t" "pop r21 \n\t" "pop r20 \n\t" "pop r19 \n\t" "pop r18 \n\t" "pop r17 \n\t" "pop r16 \n\t" "pop r15 \n\t" "pop r14 \n\t" "pop r13 \n\t" "pop r12 \n\t" "pop r11 \n\t" "pop r10 \n\t" "pop r9 \n\t" "pop r8 \n\t" "pop r7 \n\t" "pop r6 \n\t" "pop r5 \n\t" "pop r4 \n\t" "pop r3 \n\t" "pop r2 \n\t" "pop r1 \n\t" "pop r0 \n\t" "out __SREG__, r0 \n\t" "pop r0	\n\t"
  "sei \n\t"
  "ret \n\t");

 return 0;
}



# 94 "sysCore/TaskMate.c" 3
void 
# 94 "sysCore/TaskMate.c"
TIMER3_COMPA_vect 
# 94 "sysCore/TaskMate.c" 3
(void) __attribute__ ((__signal__,__used__, __externally_visible__)) ; void 
# 94 "sysCore/TaskMate.c"
TIMER3_COMPA_vect 
# 94 "sysCore/TaskMate.c" 3
(void)

# 95 "sysCore/TaskMate.c"
{

 for(uint8_t i=0;i<1;i++)
 {
  if( task_table[i].task_RTC > 0 ){task_table[i].task_RTC--;}
 }
}


# 103 "sysCore/TaskMate.c" 3
void 
# 103 "sysCore/TaskMate.c"
TIMER1_COMPA_vect 
# 103 "sysCore/TaskMate.c" 3
(void) __attribute__ ((__signal__,__used__, __externally_visible__)) __attribute__((__naked__)); void 
# 103 "sysCore/TaskMate.c"
TIMER1_COMPA_vect 
# 103 "sysCore/TaskMate.c" 3
(void)

# 104 "sysCore/TaskMate.c"
{

 
# 106 "sysCore/TaskMate.c" 3
__asm__ __volatile__ ("sei" ::: "memory")
# 106 "sysCore/TaskMate.c"
     ;


 asm volatile ( "push r0 \n\t" "in r0, __SREG__ \n\t" "push r0 \n\t" "push r1 \n\t" "push r2 \n\t" "push r3 \n\t" "push r4 \n\t" "push r5 \n\t" "push r6 \n\t" "push r7 \n\t" "push r8 \n\t" "push r9 \n\t" "push r10 \n\t" "push r11 \n\t" "push r12 \n\t" "push r13 \n\t" "push r14 \n\t" "push r15 \n\t" "push r16 \n\t" "push r17 \n\t" "push r18 \n\t" "push r19 \n\t" "push r20 \n\t" "push r21 \n\t" "push r22 \n\t" "push r23 \n\t" "push r24 \n\t" "push r25 \n\t" "push r26 \n\t" "push r27 \n\t" "push r28 \n\t" "push r29 \n\t" "push r30 \n\t" "push r31 \n\t" );
 task_table[task_current].stack_pointer=(uint8_t *)
# 110 "sysCore/TaskMate.c" 3
                                                  (*(volatile uint8_t *)((0x3D) + 0x20))
# 110 "sysCore/TaskMate.c"
                                                    ;


 if(++task_current==1){task_current=0;}


 static uint8_t alive_cnt=0;
 if(++alive_cnt>250)
 {
  PORTB ^= (1 << PB7);
  alive_cnt=0;
 }


 
# 124 "sysCore/TaskMate.c" 3
(*(volatile uint8_t *)((0x3D) + 0x20)) 
# 124 "sysCore/TaskMate.c"
   = (uint16_t)task_table[task_current].stack_pointer;
 asm volatile ( "pop r31 \n\t" "pop r30 \n\t" "pop r29 \n\t" "pop r28 \n\t" "pop r27 \n\t" "pop r26 \n\t" "pop r25 \n\t" "pop r24 \n\t" "pop r23 \n\t" "pop r22 \n\t" "pop r21 \n\t" "pop r20 \n\t" "pop r19 \n\t" "pop r18 \n\t" "pop r17 \n\t" "pop r16 \n\t" "pop r15 \n\t" "pop r14 \n\t" "pop r13 \n\t" "pop r12 \n\t" "pop r11 \n\t" "pop r10 \n\t" "pop r9 \n\t" "pop r8 \n\t" "pop r7 \n\t" "pop r6 \n\t" "pop r5 \n\t" "pop r4 \n\t" "pop r3 \n\t" "pop r2 \n\t" "pop r1 \n\t" "pop r0 \n\t" "out __SREG__, r0 \n\t" "pop r0	\n\t" "reti \n\t");
}
