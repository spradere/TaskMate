#ifndef SYSCALL_H
#define SYSCALL_H


uint8_t sysCallGetTaskID(void);

void sysCallSetTaskRTC(uint16_t);
uint16_t sysCallGetTaskRTC(void);

#endif
