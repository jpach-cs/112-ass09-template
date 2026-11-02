#include <stdlib.h>
#include <string.h>
#include "code.h"

/*
 * CSCI 112 Assignment 9 - Dynamic Memory and Memory Functions
 * 
 * Every function below is a skeleton that already compiles. Replace the
 * "// TODO" line (and the "(void)..." lines under it, which only silence
 * "unused parameter" warnings) with your implementation.
 */

/*
 * RULES (read them once, they apply to every function):
 *  - Check the result of every malloc/calloc/realloc. On failure return NULL / false
 *    and leak nothing (free what you already allocated).
 *  - Never write  p = realloc(p, ...);  - use a temporary pointer.
 *  - NULL arguments must never crash your function (see each description).
 *  - Only the standard library: <stdlib.h>, <string.h>, <stdbool.h>, <stddef.h>.
 *  - Compile with:  gcc -g -Wall -Wextra -fsanitize=address,undefined main.c code.c -o main
 */

/* ======================================================================
 * PART 1: Raw dynamic memory (malloc, calloc, realloc, free)
 * ====================================================================== */

/*
 * STEP 1: alloc_ints
 *
 * Allocate room for `count` ints with malloc().
 *  - count == 0 or allocation failure -> return NULL.
 *  - The memory is NOT initialized.
 */

int *alloc_ints(size_t count)
{
 // Return NULL if count is 0 as requested
    if (count == 0) {
        return NULL;
    }


    int *ptr = (int *)malloc(count * sizeof(int));


    return ptr;
}

/*
 * STEP 2: alloc_zeroed_ints
 *
 * Same as alloc_ints, but every byte of the block must be zero (use calloc()).
 *  - count == 0 or allocation failure -> return NULL.
 */

int *alloc_zeroed_ints(size_t count)
{
    
if (count == 0) {
        return NULL;
    }

    
    int *ptr = (int *)calloc(count, sizeof(int));


    return ptr;
}

/*
 * STEP 3: safe_free
 *
 * Receives the ADDRESS of a pointer (usage:  safe_free((void **)&p); ).
 * Free the block that *ptr points to and set *ptr to NULL (nullify!).
 *  - safe_free(NULL) does nothing.
 *  - If *ptr is already NULL, nothing happens.
 *  - Calling it twice on the same pointer must be harmless.
 */

void safe_free(void **ptr)
{
if (ptr == NULL) {
        return;
    }

    free(*ptr);

    *ptr = NULL;
}

/*
 * STEP 4: resize_ints
 *
 * Change a block holding `old_count` ints so that it holds `new_count` ints.
 *  - new_count == 0: free arr and return NULL.
 *  - arr == NULL: behave like alloc_zeroed_ints(new_count).
 *  - Otherwise use realloc() through a TEMPORARY pointer. If it fails,
 *    return NULL and leave arr untouched and still valid.
 *  - Old values are kept. If the block grew, the new ints
 *    (old_count ... new_count - 1) must be set to zero with memset().
 *  - Return the (possibly moved) block.
 */

int *resize_ints(int *arr, size_t old_count, size_t new_count)
{
 // Case 1: new_count == 0 -> free arr and return NULL
    if (new_count == 0) {
        free(arr);
        return NULL;
    }

    // Case 2: arr == NULL -> behave like alloc_zeroed_ints(new_count)
    if (arr == NULL) {
        // calloc allocates and zeroes the memory
        return (int *)calloc(new_count, sizeof(int));
    }

    // Case 3: Standard resize. Use a TEMPORARY pointer to check for failure first.
    int *temp = (int *)realloc(arr, new_count * sizeof(int));
    
    // If realloc fails, it returns NULL, but the original 'arr' is still valid
    if (temp == NULL) {
        return NULL;
    }

    // Assign the successfully reallocated pointer back
    arr = temp;

    // Case 4: If the block grew, zero out the newly appended integers
    if (new_count > old_count) {
        size_t new_elements = new_count - old_count;
        // Calculate the starting memory address of the new elements
        int *start_of_new_elements = arr + old_count;
        
        memset(start_of_new_elements, 0, new_elements * sizeof(int));
    }

    return arr;
}

/*
 * STEP 5: duplicate_ints
 *
 * Return a NEW block holding a copy of `count` ints from src (malloc + memcpy).
 *  - src == NULL, count == 0 or allocation failure -> return NULL.
 *  - The caller owns the returned block.
 */

int *duplicate_ints(const int *src, size_t count)
{
  if (src == NULL || count == 0) {
        return NULL;
    }

    // Calculate total bytes needed
    size_t total_bytes = count * sizeof(int);

    // Allocate the new block
    int *dest = (int *)malloc(total_bytes);

    // Return NULL naturally if allocation fails
    if (dest == NULL) {
        return NULL;
}
 // Copy the contents from src to the newly allocated block
    memcpy(dest, src, total_bytes);

    return dest;
}
/* ======================================================================
 * PART 2: ByteBuffer - a growing block of bytes
 *
 * Invariants of a valid buffer:
 *   size <= capacity,  capacity >= 1,
 *   every byte of data from index `size` on is 0.
 * ====================================================================== */

/*
 * STEP 6: buffer_create
 *
 * Allocate the struct AND its data block (`capacity` bytes).
 * Fill the data block with zeros (memset), set size = 0.
 *  - capacity == 0 or any allocation failure -> return NULL.
 *  - If the struct was allocated but the data block was not,
 *    FREE THE STRUCT before returning NULL (no leak!).
 */

ByteBuffer *buffer_create(size_t capacity)
{
if (capacity == 0) {
        return NULL;
    }

    // 1. Allocate the management structure
    ByteBuffer *buf = (ByteBuffer *)malloc(sizeof(ByteBuffer));
    if (buf == NULL) {
        return NULL; // Allocation failure
    }

    // 2. Allocate the underlying data block (requested in bytes)
    buf->data = (unsigned char *)malloc(capacity);
    if (buf->data == NULL) {
        // CLEANUP: Free the struct to prevent a leak if data allocation failed
        free(buf);
        return NULL;
    }

    // 3. Initialize fields and clear the data block
    memset(buf->data, 0, capacity);
    buf->capacity = capacity;
    buf->size = 0;

    return buf;
}

/*
 * STEP 7: buffer_reserve
 *
 * Make sure the buffer can hold at least `min_capacity` bytes.
 *  - buf == NULL -> false.
 *  - min_capacity <= capacity -> true, nothing changes.
 *  - Otherwise DOUBLE the capacity until it is >= min_capacity
 *    (4 -> 8 -> 16 -> 32 ...), realloc the data (temporary pointer!),
 *    set the new bytes to zero (memset) and update capacity. Return true.
 *  - If realloc fails -> false, the buffer stays unchanged.
 */

bool buffer_reserve(ByteBuffer *buf, size_t min_capacity)
{
 if (buf == NULL) {
        return false;
    }

    // Case 2: min_capacity <= capacity -> true, nothing changes
    if (min_capacity <= buf->capacity) {
        return true;
    }

    // Case 3: Geometric growth. Double the capacity until it hits or exceeds min_capacity
    size_t new_capacity = buf->capacity;
    
    // Safety check: if current capacity is 0, initialize it to a base value (like 4)
    if (new_capacity == 0) {
        new_capacity = 4; 
    }

    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }

    // Try resizing through a TEMPORARY pointer to keep the buffer safe on failure
    unsigned char *temp = (unsigned char *)realloc(buf->data, new_capacity);
    if (temp == NULL) {
        return false; // realloc failed, buffer stays completely untouched
    }

    // Zero out the newly appended bytes
    size_t old_capacity = buf->capacity;
    size_t extra_bytes = new_capacity - old_capacity;
    
    // temp + old_capacity moves the pointer directly to where the new memory begins
    memset(temp + old_capacity, 0, extra_bytes);

    // Commit changes to the structure
    buf->data = temp;
    buf->capacity = new_capacity;

    return true;
}

/*
 * STEP 8: buffer_append
 *
 * Append `n` bytes from `data` after the content (memcpy).
 * Grow with buffer_reserve(buf, size + n) if needed.
 *  - buf == NULL, or data == NULL while n > 0 -> false.
 *  - n == 0 -> true, nothing changes.
 *  - On success size grows by n.
 */

bool buffer_append(ByteBuffer *buf, const void *data, size_t n)
{
 if (n == 0) {
        return true;
    }

    // Case 2: buf == NULL, or data == NULL while n > 0 -> false
    if (buf == NULL || data == NULL) {
        return false;
    }

    // Calculate minimum required capacity to hold existing + incoming payload
    size_t required_capacity = buf->size + n;

    // Use Step 7's reserve function to expand backing array if needed
    if (!buffer_reserve(buf, required_capacity)) {
        return false; // Reservation / reallocation failed
    }

    // Copy incoming bytes to the tail of the current buffer data content
    // buf->data + buf->size shifts pointer straight to the first unused byte
    memcpy(buf->data + buf->size, data, n);

    // Commit the tracked element growth size
    buf->size += n;

    return true;
}

/*
 * STEP 9: buffer_append_byte
 *
 * Append one byte. Hint: it is a one-liner using buffer_append().
 */

bool buffer_append_byte(ByteBuffer *buf, unsigned char byte)
{
    // TODO
    return buffer_append(buf, &byte, 1);
}

/*
 * STEP 10: buffer_append_string
 *
 * Append the characters of `str` WITHOUT the terminating '\0'
 * (use strlen() and buffer_append()).
 *  - str == NULL -> false.
 *  - empty string -> true, nothing changes.
 */

bool buffer_append_string(ByteBuffer *buf, const char *str)
{
if (str == NULL) {
        return false;
    }

    // Calculate length of string excluding '\0'
    size_t len = strlen(str);

    // Case 2: empty string ("") -> true, nothing changes
    if (len == 0) {
        return true;
    }

    // Delegate the remaining safety checks, reservation, and writing to buffer_append
    return buffer_append(buf, str, len);
}

/*
 * STEP 11: buffer_find
 *
 * Return the index of the FIRST occurrence of `byte` in the CONTENT
 * (bytes 0 ... size-1, never the spare room!). Use memchr().
 *  - not found or buf == NULL -> -1.
 *  - Hint: index = (found pointer) - (start of data).
 */

int buffer_find(const ByteBuffer *buf, unsigned char byte)
{
 if (buf == NULL || buf->size == 0 || buf->data == NULL) {
        return -1;
    }

    // Use memchr to look for the target byte up to buf->size bounds
    void *found = memchr(buf->data, byte, buf->size);

    // Case 2: Byte not found in the populated array content
    if (found == NULL) {
        return -1;
    }

    // Case 3: Calculate the index using pointer arithmetic
    // Cast to unsigned char* to ensure arithmetic happens byte-by-byte
    return (int)((unsigned char *)found - buf->data);
}
/*
 * STEP 12: buffer_equals
 *
 * true if both buffers have the same size AND the same content bytes
 * (memcmp). Capacities do not matter. Two empty buffers are equal.
 *  - any argument NULL -> false.
 */

bool buffer_equals(const ByteBuffer *a, const ByteBuffer *b)
{
 if (a == NULL || b == NULL) {
        return false;
    }

    // Case 2: Sizes must match for content to be identical
    if (a->size != b->size) {
        return false;
    }

    // Case 3: Two empty buffers are equal (sizes are equal and both are 0)
    if (a->size == 0) {
        return true;
    }

    // Case 4: Compare active content bytes up to the shared size boundary
    // memcmp returns 0 if the memory blocks match perfectly
    return memcmp(a->data, b->data, a->size) == 0;
}

/*
 * STEP 13: buffer_insert
 *
 * Insert `n` bytes from `data` so that they start at index `pos`.
 * The old bytes from `pos` on move to the right. pos == size means "append".
 *  - buf == NULL, pos > size, or data == NULL while n > 0 -> false
 *    (buffer unchanged).
 *  - n == 0 -> true, nothing changes.
 *  - Grow with buffer_reserve() if needed.
 *  - The tail moves onto ITSELF (overlap!) -> memmove(), then memcpy()
 *    for the new bytes.
 */

bool buffer_insert(ByteBuffer *buf, size_t pos, const void *data, size_t n)
{
 if (n == 0) {
        return true;
    }

    // Case 2: Validation checks
    if (buf == NULL || pos > buf->size || data == NULL) {
        return false;
    }

    // Calculate the new required minimum capacity
    size_t required_capacity = buf->size + n;

    // Grow the backing array safely if needed
    if (!buffer_reserve(buf, required_capacity)) {
        return false; // Reservation / reallocation failed, buffer stays unchanged
    }

    // Case 3: Shift the existing tail to the right to clear room for the insertion
    if (pos < buf->size) {
        size_t bytes_to_move = buf->size - pos;
        
        // Target destination is shifted right by n bytes
        unsigned char *dest = buf->data + pos + n;
        // Source is the current position of the tail
        unsigned char *src = buf->data + pos;

        // Use memmove instead of memcpy because src and dest overlap within buf->data
        memmove(dest, src, bytes_to_move);
    }

    // Case 4: Copy the incoming new data bytes directly into the cleared slot
    memcpy(buf->data + pos, data, n);

    // Commit the new size tracking
    buf->size += n;

    return true;
}

/*
 * STEP 14: buffer_remove
 *
 * Remove `n` bytes starting at index `pos`; the following bytes move left.
 *  - buf == NULL, pos > size or pos + n > size -> false (buffer unchanged).
 *    Write the check so that it cannot overflow:  n > size - pos
 *  - n == 0 -> true, nothing changes.
 *  - Use memmove() for the shift. Afterwards the n bytes that became unused
 *    at the end of the content must be set to zero (memset). Then size -= n.
 */

bool buffer_remove(ByteBuffer *buf, size_t pos, size_t n)
{
if (n == 0) {
        return true;
    }

    // Case 2: Validation constraints
    // Checking "pos > buf->size" first ensures the subtraction "buf->size - pos" remains safe from underflow
    if (buf == NULL || pos > buf->size || n > buf->size - pos) {
        return false;
    }

    // Calculate how many bytes sit after the removed range and need to shift left
    size_t bytes_to_shift = buf->size - (pos + n);

    // Case 3: Shift the trailing bytes to the left if there are any
    if (bytes_to_shift > 0) {
        unsigned char *dest = buf->data + pos;
        unsigned char *src = buf->data + pos + n;
        
        // Use memmove since the source and destination regions overlap inside the array
        memmove(dest, src, bytes_to_shift);
    }

    // Case 4: Zero out the newly vacated bytes at the tail end of the valid content
    size_t new_size = buf->size - n;
    memset(buf->data + new_size, 0, n);

    // Commit the new tracked data size
    buf->size = new_size;

    return true;
}

/*
 * STEP 15: buffer_clear
 *
 * Set size to 0 and zero the WHOLE data block (memset, `capacity` bytes).
 * The capacity stays the same. buf == NULL -> do nothing.
 */

void buffer_clear(ByteBuffer *buf)
{
 if (buf == NULL) {
        return;
    }

    // Zero out the entire backing block up to its capacity boundary
    if (buf->data != NULL && buf->capacity > 0) {
        memset(buf->data, 0, buf->capacity);
    }

    // Reset the tracked data content marker back to zero
    buf->size = 0;
}

/*
 * STEP 16: buffer_destroy
 *
 * Receives the ADDRESS of your pointer (usage:  buffer_destroy(&buf); ).
 * Free the data block, then the struct, then set *buf to NULL.
 *  - buf == NULL or *buf == NULL -> do nothing.
 *  - Calling it twice must be harmless.
 */

void buffer_destroy(ByteBuffer **buf)
{
if (buf == NULL || *buf == NULL) {
        return;
    }

    // 1. Free the inner array data block first
    // (Freeing the struct first would make this pointer unreachable, creating a leak!)
    free((*buf)->data);

    // 2. Free the outer management structure wrapper
    free(*buf);

    // 3. Nullify the original pointer to guarantee subsequent calls are harmless
    *buf = NULL;
}

/* ======================================================================
 * PART 3: Deep copies, searching and arrays of strings
 * ====================================================================== */

/*
 * STEP 17: buffer_clone
 *
 * Return a DEEP copy: a new struct with its OWN data block, the same
 * capacity and the same content (memcpy `size` bytes).
 *  - src == NULL or allocation failure -> NULL.
 *  - Changing the clone must never change the source (and vice versa).
 *  - Hint: buffer_create() gives you the new buffer.
 */

ByteBuffer *buffer_clone(const ByteBuffer *src)
{
  
    if (src == NULL) {
        return NULL;
    }


    ByteBuffer *dest = buffer_create(src->capacity);
    if (dest == NULL) {
        return NULL;
    }

   
    dest->size = src->size;

    
    if (dest->size > 0 && src->data != NULL) {
        memcpy(dest->data, src->data, src->size);
    }

    return dest;
}




/*
 * STEP 18: buffer_concat
 *
 * Return a NEW buffer holding the content of `a` followed by the content of `b`.
 *  - Its capacity is EXACTLY a->size + b->size (but at least 1).
 *  - Neither source is modified.
 *  - Any NULL argument or allocation failure -> NULL.
 *  - Hint: buffer_create() + buffer_append() twice.
 */

ByteBuffer *buffer_concat(const ByteBuffer *a, const ByteBuffer *b)
{
  
    if (a == NULL || b == NULL) {
        return NULL;
    }

  
    size_t required_capacity = a->size + b->size;
    if (required_capacity == 0) {
        required_capacity = 1;
    }

    ByteBuffer *dest = buffer_create(required_capacity);
    if (dest == NULL) {
        return NULL;
    }

 
    buffer_append(dest, a->data, a->size);
    buffer_append(dest, b->data, b->size);

    return dest;
}

/*
 * STEP 19: buffer_count
 *
 * Return how many times `byte` occurs in the CONTENT (bytes 0 ... size-1).
 * Use memchr() repeatedly, each time continuing right AFTER the last hit.
 *  - buf == NULL -> 0.
 */

size_t buffer_count(const ByteBuffer *buf, unsigned char byte)
{
   // Return 0 if the buffer pointer is NULL or if it contains no elements
    if (buf == NULL || buf->data == NULL || buf->size == 0) {
        return 0;
    }

    size_t count = 0;
    const unsigned char *current_ptr = buf->data;
    size_t remaining_bytes = buf->size;

    
    while (remaining_bytes > 0) {
        const unsigned char *hit = memchr(current_ptr, byte, remaining_bytes);
        
     
        if (hit == NULL) {
            break;
        }

        count++;
        

        size_t bytes_passed = (hit - current_ptr) + 1;
        current_ptr += bytes_passed;
        remaining_bytes -= bytes_passed;
    }

    return count;
}

/*
 * STEP 20: buffer_shrink_to_fit
 *
 * Reduce the capacity to `size` (but at least 1) using realloc through a
 * TEMPORARY pointer. The content stays.
 *  - buf == NULL -> false.
 *  - Success (also when nothing had to change) -> true.
 *  - realloc fails -> false, buffer unchanged.
 */

bool buffer_shrink_to_fit(ByteBuffer *buf)
{
 
    if (buf == NULL) {
        return false;
    }


    size_t target_capacity = buf->size;
    if (target_capacity == 0) {
        target_capacity = 1;
    }

 
    if (buf->capacity == target_capacity) {
        return true;
    }

    unsigned char *temp = realloc(buf->data, target_capacity * sizeof(unsigned char));

    if (temp == NULL) {
        return false;
    }

    buf->data = temp;
    buf->capacity = target_capacity;

    return true;
}

/*
 * STEP 21: buffer_split
 *
 * Split the content at every `delim` byte. Return a heap-allocated ARRAY
 * of heap-allocated STRINGS.
 *  - A buffer with k delimiters gives k + 1 tokens. Empty tokens are kept:
 *        "a,bb,,ccc" split at ','  ->  "a", "bb", "", "ccc"
 *        "a,"                      ->  "a", ""
 *        ""   (empty buffer)       ->  exactly ONE empty token ""
 *  - Every token is a separate malloc'ed, '\0'-terminated copy
 *    (memcpy + terminator). Tokens never point into the buffer;
 *    the buffer is not modified.
 *  - The array of pointers must grow while you discover tokens: start small
 *    (for example 4 pointers) and use realloc (doubling).
 *  - Use memchr() to find the delimiters.
 *  - *out_count receives the number of tokens.
 *  - On ANY failure (also buf == NULL or out_count == NULL): return NULL;
 *    if out_count != NULL set *out_count = 0; and FREE everything you
 *    allocated so far.
 */

char **buffer_split(const ByteBuffer *buf, unsigned char delim, size_t *out_count)
{
// Validate required output pointer
    if (out_count == NULL) {
        return NULL;
    }
    *out_count = 0;

    // Validate buffer pointer
    if (buf == NULL) {
        return NULL;
    }

    // Initialize dynamic array allocation tracking
    size_t array_capacity = 4;
    char **tokens = malloc(array_capacity * sizeof(char *));
    if (tokens == NULL) {
        return NULL;
    }

    size_t token_count = 0;
    const unsigned char *current_ptr = buf->data;
    size_t remaining_bytes = buf->size;

    // Outer loop runs at least once to handle an empty buffer/trailing delimiter
    // It breaks internally when there are no more remaining bytes after processing a token
    while (1) {
        // Find the next delimiter within the remaining slice
        const unsigned char *hit = NULL;
        if (remaining_bytes > 0 && current_ptr != NULL) {
            hit = memchr(current_ptr, delim, remaining_bytes);
        }

        // Calculate length of the current token segment
        size_t token_len = (hit != NULL) ? (size_t)(hit - current_ptr) : remaining_bytes;

        // Double array capacity if we hit our pointer limit
        if (token_count >= array_capacity) {
            size_t new_capacity = array_capacity * 2;
            char **temp_tokens = realloc(tokens, new_capacity * sizeof(char *));
            if (temp_tokens == NULL) {
                goto cleanup_failure;
            }
            tokens = temp_tokens;
            array_capacity = new_capacity;
        }

        // Allocate the string copy including the null terminator
        tokens[token_count] = malloc((token_len + 1) * sizeof(char));
        if (tokens[token_count] == NULL) {
            goto cleanup_failure;
        }

        // Copy content if available and null-terminate
        if (token_len > 0 && current_ptr != NULL) {
            memcpy(tokens[token_count], current_ptr, token_len);
        }
        tokens[token_count][token_len] = '\0';
        token_count++;

        // Shift pointers forward past the delimiter if one was hit
        if (hit != NULL) {
            size_t bytes_consumed = token_len + 1;
            current_ptr += bytes_consumed;
            remaining_bytes -= bytes_consumed;
        } else {
            // No more delimiters left; we just processed the final token
            break;
        }
    }

    *out_count = token_count;
    return tokens;

cleanup_failure:
    // If any allocation fails, release all strings and the array shell
    if (tokens != NULL) {
        for (size_t i = 0; i < token_count; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }
    *out_count = 0;
    return NULL;
}

/*
 * STEP 22: free_tokens
 *
 * Receives the ADDRESS of your tokens pointer (usage: free_tokens(&tokens, n); ).
 * Free every token first, then the array itself, then set *tokens to NULL.
 *  - tokens == NULL or *tokens == NULL -> do nothing.
 *  - Calling it twice must be harmless.
 */

void free_tokens(char ***tokens, size_t count)
{
 // Do nothing if the pointer to the array or the array itself is NULL
    if (tokens == NULL || *tokens == NULL) {
        return;
    }

    // Free every individual token string
    for (size_t i = 0; i < count; i++) {
        // Double-check pointer just in case a token was partially allocated or already freed
        if ((*tokens)[i] != NULL) {
            free((*tokens)[i]);
        }
    }

    // Free the main array shell
    free(*tokens);

    // Set the pointer to NULL so subsequent calls are harmless
    *tokens = NULL;
}
