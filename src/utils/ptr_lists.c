/*
 * This module manages a dynamic list of pointers. The pointers can point to any arbitrary
 * data type and the caller is responsible for freeing the data and making sure that the
 * pointer remains valid through the life of the list.
 */
#include "common.h"

/*
 * This resizes the list to grow it. This should always be called before
 * adding aything to it.
 *
 * Aborts the program upon failure.
 */
static void resize_list(ptr_list_t* list)
{
    if(list->nitems + 2 > list->capacity)
    {
        list->capacity = list->capacity << 1;
        list->buffer = REALLOC(list->buffer, list->capacity * sizeof(void*));

        if(list->buffer == NULL)
            fatal_error("cannot allocate %lu bytes for managed buffer", list->capacity * sizeof(void*));
    }
}

/*
 * Internal function that calculates a pointer into the buffer, given the index
 * of the intended item. This returns a pointer to the first byte of the item
 * that was stored. It's up to the caller to convert it to a pointer to the data
 * object that it knows about.

static unsigned char* list_at_index(ptr_list_t* list, int index)
{
    unsigned char* ptr = &list->buffer[list->item_size * index];

    return ptr;
}
*/

/*
 * Free the list buffer. This is only used when the list will no longer
 * be used, such as when the program ends.
 */
void destroy_ptr_list(ptr_list_t* list)
{
    FREE(list->buffer);
    FREE(list);
}

/*
 * Initialize a newly created or otherwise existing list.
 */
void init_ptr_list(ptr_list_t* list) {

    list->capacity = 0x01 << 3;
    list->nitems = 0;
    list->index = 0;
    list->buffer = (void**)CALLOC(list->capacity, sizeof(void*));
    if(list->buffer == NULL)
        fatal_error("cannot allocate %lu bytes for managed list buffer", list->capacity * sizeof(void*));
}

/*
 * Initially create the list and initialize the contents to initial values.
 * If the list was in use before this, the buffer will be freed.
 *
 * Size is the size of each item that that will be put in the list.
 */
ptr_list_t* create_ptr_list(void)
{
    ptr_list_t* list;

    list = (ptr_list_t*)MALLOC(sizeof(ptr_list_t));
    if(list == NULL)
        fatal_error("cannot allocate %lu bytes for managed list", sizeof(ptr_list_t));

    init_ptr_list(list);

    return list;
}

/*
 * Store the given item in the given list at the end of the list.
 */
void append_ptr_list(ptr_list_t* list, void* item)
{
    resize_list(list);
    list->buffer[list->nitems] = item;
    list->nitems++;
}

/*
 * If the index is within the bounds of the list, then return a raw pointer to
 * the element specified. If it is outside the list, or if there is nothing in
 * the list, then return NULL.
 */
void* get_ptr_list_by_index(ptr_list_t* list, int index)
{
    if(list != NULL)
    {
        if(index >= 0 && index < (int)list->nitems)
        {
            return list->buffer[index];
        }
    }

    return NULL;  // failed
}

/*
 * If the index is within the bounds of the list, then return a raw pointer to
 * the element specified. If it is outside the list, or if there is nothing in
 * the list, then return NULL.
 */
void* get_ptr_list_next(ptr_list_t* list)
{
    if(list != NULL)
    {
        if(list->index >= 0 && list->index < list->nitems)
        {
            void* retv = list->buffer[list->index];
            list->index++;
            return retv;
        }
    }

    return NULL;  // failed
}

/*
 * Reset the internal index to zero.
 */
void reset_ptr_list(ptr_list_t* list) {
    list->index = 0;
}
