#ifndef TIMER3_H
#define TIMER3_H

void timer3SetStatus(uint8_t);
uint8_t timer3GetStatus(void);
uint8_t *timer3GetName(void);

void timer3Init(void);
void timer3Start(void);
void timer3Stop(void);


#endif
