// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>

typedef double f64;
typedef float f32;
typedef uint64_t u64;
typedef int64_t i64;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint8_t u8;
typedef int8_t i8;

typedef enum { Ok, Err, Done, } Result;
#define return_defer(value) do { result = (value); goto defer; } while (0)
#define MAX_PATH_SIZE 128
#define LENGTH(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define true 1
#define false 0

#define format_buffer(BUFFER_NAME, BUFFER_SIZE, FMT_STRING, ...) \
  char BUFFER_NAME[BUFFER_SIZE] = {0}; \
  snprintf(BUFFER_NAME, BUFFER_SIZE, FMT_STRING, ##__VA_ARGS__)

typedef struct {
  u8* data;
  u32 size;
} Buffer;

void buffer_init(Buffer* buffer);

void buffer_init_new(Buffer* buffer, u32 size);

void buffer_free(Buffer* buffer);

u32 buffer_iterate(void* restrict dest, Buffer* source, u32 size, u32* iter);

Result file_read(const char* path, Buffer* buffer);

Result file_write(const char* path, Buffer* buffer);

void log_error(const char* format, ...);

f32 lerp_f32(f32 v0, f32 v1, f32 t);

#endif // _COMMON_H
