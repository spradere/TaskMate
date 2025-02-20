#ifndef TIMER1_H
#define TIMER1_H

void timer1SetStatus(uint8_t);
uint8_t timer1GetStatus(void);
uint8_t *timer1GetName(void);

void timer1Init(void);
void timer1Start(void);
void timer1Stop(void);



#endif
