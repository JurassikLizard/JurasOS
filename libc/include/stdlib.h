#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

__attribute__((__noreturn__))
void PANIC(const char *message, ...);

void delay(unsigned int millis);

#ifdef __cplusplus
}
#endif

#endif