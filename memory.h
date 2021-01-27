#ifndef __MEMORY_H__
#define __MEMORY_H__

#define CALLOC memory_calloc
#define MALLOC memory_malloc
#define REALLOC memory_realloc
#define FREE memory_free
#define STRDUP memory_strdup

#include <stdlib.h>

void init_memory();
void *memory_calloc(size_t, size_t);
void *memory_malloc(size_t);
void *memory_realloc(void*, size_t);
void memory_free(void*);
char* memory_strdup(const char*);

#endif