#include <stdlib.h>

#include <stdio.h>

extern volatile uint32_t timer_tick;
extern uint32_t timer_frequency;

void delay(unsigned int millis) {
    float ticks_per_millis = (float)timer_frequency / (float)1000;
    millis *= ticks_per_millis;
    millis += timer_tick;
    while(millis > timer_tick);
}