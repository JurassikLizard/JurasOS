#include <kernel/timer.h>

#include "idt/irq.h"

#include <kernel/io.h>

#include <stdlib.h>
#include <stdio.h>

volatile uint32_t timer_tick;
uint32_t timer_frequency;

static void timer_callback(registers_t regs)
{
    timer_tick++;
}

void init_timer(uint32_t frequency)
{
   // Firstly, register our timer callback.
   irq_install_handler(0, &timer_callback);

   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   uint32_t divisor = 1193180 / frequency;
   timer_frequency = frequency;

   // Send the command byte.
   io_outb(0x43, 0x36);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   uint8_t l = (uint8_t)(divisor & 0xFF);
   uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

   // Send the frequency divisor.
   io_outb(0x40, l);
   io_outb(0x40, h);
}