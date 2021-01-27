/*
    This is a simple wrapper around the memory allocation routines to make
    error handling easier. All memory allocation errors are fatal errors.
*/
#include "common.h"

static uint64_t mem_segment;
#define SEG_MASK 0xFFFF00000000
#define GET_SEG(p) (((uint64_t)p)&SEG_MASK)

void memory_init() {

    // int stackvar;
    void* ptr = malloc(1);
    // fprintf(stderr, "stk=%p, mal=%p, glo=%p\n", &stackvar, ptr, &mem_segment);
    mem_segment = GET_SEG(ptr);
}

void *memory_calloc(size_t num, size_t size) {

    void* ptr = calloc(num, size);
    if(ptr == NULL)
        fatal_error("cannot allocate %lu bytes\n", num*size);

    return ptr;
}

void *memory_malloc(size_t size) {

    void* ptr = malloc(size);
    if(ptr == NULL)
        fatal_error("cannot allocate %lu bytes\n", size);

    return ptr;
}

void *memory_realloc(void* ptr, size_t size) {

    void* nptr = realloc(ptr, size);
    if(nptr == NULL)
        fatal_error("cannot reallocate %lu bytes\n", size);

    return nptr;
}

void memory_free(void* ptr) {

    if(mem_segment != GET_SEG(ptr))
        fatal_error("Attempt to free a pointer that was not allocated: %p\n", ptr);
    else
        free(ptr);
}

char* memory_strdup(const char* str) {

    char* nptr = strdup(str);
    if(nptr == NULL)
        fatal_error("cannot strdup %lu bytes\n", strlen(str));

    return nptr;
}
