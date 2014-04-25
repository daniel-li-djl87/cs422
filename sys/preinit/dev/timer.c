/*
 * The 8253 Programmable Interval Timer (PIT),
 * which generates interrupts on IRQ 0.
 */

#include <preinit/lib/types.h>
#include <lib/x86.h>

#include "timer.h"

/* I/O addresses of the timer */
#define	IO_TIMER1	0x040		/* 8253 Timer #1 */

/*
 * Frequency of all three count-down timers; (TIMER_FREQ/freq) is the
 * appropriate count to generate a frequency of freq hz.
 */
#define TIMER_DIV(x)	((TIMER_FREQ+(x)/2)/(x))

/*
 * Macros for specifying values to be written into a mode register.
 */
#define	TIMER_MODE	(IO_TIMER1 + 3)	/* timer mode port */
#define		TIMER_SEL0	0x00	/* select counter 0 */
#define		TIMER_RATEGEN	0x04	/* mode 2, rate generator */
#define		TIMER_16BIT	0x30	/* r/w counter 16 bits, LSB first */

// Initialize the programmable interval timer.

void
timer_hw_init(void)
{
	outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
	outb(IO_TIMER1, TIMER_DIV(100) % 256);
	outb(IO_TIMER1, TIMER_DIV(100) / 256);
}