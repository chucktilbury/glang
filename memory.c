/*
    This is a simple wrapper around the memory allocation routines to make
    error handling easier. All memory allocation errors are fatal errors.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
    if(ptr == NULL) {
        fprintf(stderr, "FATAL ERROR: cannot allocate %lu bytes\n", num*size);
        exit(1);
    }
    return ptr;
}

void *memory_malloc(size_t size) {

    void* ptr = malloc(size);
    if(ptr == NULL) {
        fprintf(stderr, "FATAL ERROR: cannot allocate %lu bytes\n", size);
        exit(1);
    }
    return ptr;
}

void *memory_realloc(void* ptr, size_t size) {

    void* nptr = realloc(ptr, size);
    if(nptr == NULL) {
        fprintf(stderr, "FATAL ERROR: cannot reallocate %lu bytes\n", size);
        exit(1);
    }
    return nptr;
}

void memory_free(void* ptr) {

    if(mem_segment != GET_SEG(ptr)) {
        fprintf(stderr, "ERROR: Attempt to free a pointer that was not allocated: %p\n", ptr);
        // make this fatal when its finalized
    }
    else
        free(ptr);
}

char* memory_strdup(const char* str) {

    char* nptr = strdup(str);
    if(nptr == NULL) {
        fprintf(stderr, "FATAL ERROR: cannot strdup %lu bytes\n", strlen(str));
        exit(1);
    }
    return nptr;
}
