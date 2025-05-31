/** Copyright 2022 Ryan Queiroz <ryanqueiroz@proton.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __ARENA_ALLOCATOR_H__
#define __ARENA_ALLOCATOR_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>
#include <stdio.h>

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT(x) assert(x)
#endif // ARENA_ASSERT

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC(x) malloc(x)
#endif // ARENA_MALLOC

#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE(x) free(x)
#endif

typedef struct Arena Arena;

/**
 * @brief Initializes the memory arena.
 *
 * Allocates a block of memory for the arena and sets the initial offset to zero.
 *
 * @param arena Pointer to the Arena structure to initialize.
 * @param capacity The total size of memory (in bytes) that the arena will manage.
 */
void ArenaInit(Arena* arena, size_t capacity);

/**
 * @brief Initializes the memory arena using an existing memory block.
 *
 * This function allows the user to provide a pre-allocated memory block instead of dynamically allocating memory.
 *
 * @param arena Pointer to the Arena structure to initialize.
 * @param ctx Pointer to an existing memory block that will serve as the arena's data buffer.
 * @param capacity The total size of the provided memory block in bytes.
 */
void ArenaInitCtx(Arena* arena, void* ctx, size_t capacity);

/**
 * @brief Allocates memory from the arena.
 *
 * Allocates a chunk of memory from the arena, increasing the offset accordingly.
 *
 * @param arena Pointer to the Arena from which memory is allocated.
 * @param size Number of bytes to allocate.
 * @return Pointer to the allocated memory, or NULL if there is not enough space.
 */
void* ArenaPush(Arena* arena, size_t size);

/**
 * @brief Allocates memory with alignment constraints.
 *
 * Ensures that the allocated memory starts at an address that is a multiple of the specified alignment.
 *
 * @param arena Pointer to the Arena from which memory is allocated.
 * @param size Number of bytes to allocate.
 * @param alignment The required memory alignment (must be a power of two).
 * @return Pointer to the allocated memory, or NULL if there is not enough space.
 */
void* ArenaPushAligned(Arena* arena, size_t size, size_t alignment);

/**
 * @brief Resets the memory arena.
 *
 * Resets the allocation offset, making all previously allocated memory available again.
 * Does not free the allocated memory block.
 *
 * @param arena Pointer to the Arena to reset.
 */
void ArenaReset(Arena* arena);

/**
 * @brief Deletes the memory arena.
 *
 * Frees the allocated memory and resets the arena's internal state.
 *
 * @param arena Pointer to the Arena to delete.
 */
void ArenaDelete(Arena* arena);

#ifdef ARENA_DEBUG
/**
 * @brief Gets the remaining free space in the arena.
 *
 * @param arena Pointer to the Arena.
 * @return The number of free bytes available in the arena.
 */
size_t ArenaRemaining(Arena* arena);

/**
 * @brief Gets the amount of used space in the arena.
 *
 * @param arena Pointer to the Arena.
 * @return The number of bytes that have been allocated.
 */
size_t ArenaUsed(Arena* arena);
#endif

/* === Implementation === */

#ifdef ARENA_ALLOCATOR_IMPL

typedef struct Arena {
    size_t   capacity;
    size_t   offset;
    uint8_t* data;
} Arena;

void ArenaInit(Arena* arena, size_t capacity) {
    arena->data = ARENA_MALLOC(capacity);
    ARENA_ASSERT(arena->data);
    arena->capacity = capacity;
    arena->offset   = 0;
}

void ArenaInitCtx(Arena* arena, void* ctx, size_t capacity) {
    arena->data = ctx;
    ARENA_ASSERT(arena->data);
    arena->capacity = capacity;
    arena->offset   = 0;
}

void* ArenaPush(Arena* arena, size_t size) {
    ARENA_ASSERT(arena->offset + size < arena->capacity);
    void* ptr = (void*)(arena->data + arena->offset);
    arena->offset += size;
    return ptr;
}

void* ArenaPushAligned(Arena* arena, size_t size, size_t alignment) {
    size_t current   = (size_t)arena->data + arena->offset;
    size_t aligned   = (current + alignment - 1) & ~(alignment - 1);
    size_t newOffset = aligned + size - (size_t)arena->data;

    ARENA_ASSERT(newOffset <= arena->capacity);

    arena->offset = newOffset;
    return (void*)aligned;
}

void ArenaReset(Arena* arena) {
    arena->offset = 0;
}

void ArenaDelete(Arena* arena) {
    ARENA_FREE(arena->data);
    arena->data     = NULL;
    arena->capacity = 0;
    arena->offset   = 0;
}

#ifdef ARENA_DEBUG
size_t ArenaRemaining(Arena* arena) {
    return (arena->capacity - arena->offset);
}

size_t ArenaUsed(Arena* arena) {
    return arena->offset;
}

#endif // __ARENA_ALLOCATOR_H__

#endif // ARENA_DEBUG
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
#endif // ARENA_ALLOCATOR
