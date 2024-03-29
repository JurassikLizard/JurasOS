#ifndef _BITMAP_H
#define _BITMAP_H 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t size;
    uint8_t *buffer;
} bitmap_t;

bool bitmap_get(bitmap_t * bitmap, uint32_t index);
void bitmap_set(bitmap_t * bitmap, uint32_t index, bool value);

#ifdef __cplusplus
}
#endif

#endif