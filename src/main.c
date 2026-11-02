/*
 * main.c - Sandbox for testing
 * 
 * To use the functions you implemented in code.c, you must include the header:
 *   #include "code.h"
 * 
 * This tells the compiler where to find the function prototypes.
 * Without this include, the compiler won't know about your functions!
 */

#include <stdio.h>
#include "code.h"

int main(void)
{
// Part 1
    int *a = alloc_zeroed_ints(3);
    a = resize_ints(a, 3, 6);
    a[5] = 42;
    printf("a[0]=%d a[5]=%d\n", a[0], a[5]);
    safe_free((void **)&a);
    printf("a after safe_free: %p\n", (void *)a);

    // Part 2
    ByteBuffer *b = buffer_create(2);
    buffer_append_string(b, "Hello");
    buffer_append_byte(b, ' ');
    buffer_append_string(b, "World");
    buffer_insert(b, 5, ",", 1);
    printf("size=%zu capacity=%zu find('W')=%d\n", b->size, b->capacity, buffer_find(b, 'W'));
    printf("content: %.*s\n", (int)b->size, (char *)b->data);

    // Part 3
    buffer_clear(b);
    buffer_append_string(b, "one,two,,four");
    size_t n = 0;
    char **tokens = buffer_split(b, ',', &n);
    for (size_t i = 0; i < n; i++)
    {
        printf("token %zu: \"%s\"\n", i, tokens[i]);
    }
    free_tokens(&tokens, n);

    ByteBuffer *copy = buffer_clone(b);
    printf("clone equals original: %d\n", buffer_equals(b, copy));

    buffer_destroy(&copy);
    buffer_destroy(&b);
    return 0;
}