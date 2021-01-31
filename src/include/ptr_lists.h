#ifndef __PTR_LISTS_H__
#define __PTR_LISTS_H__
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Structure for a managed array.
 */
typedef struct
{
    size_t nitems;      // number of items currently in the array
    size_t index;       // current index for iterating the list
    size_t capacity;    // capacity in items
    void** buffer;      // raw buffer where the items are kept
} ptr_list_t;

void init_ptr_list(ptr_list_t* list);
ptr_list_t* create_ptr_list(void);
void destroy_ptr_list(ptr_list_t* array);
void append_ptr_list(ptr_list_t* array, void* item);
void* get_ptr_list_by_index(ptr_list_t* array, int index);
void* get_ptr_list_next(ptr_list_t* lst);
void reset_ptr_list(ptr_list_t* lst);

#endif
