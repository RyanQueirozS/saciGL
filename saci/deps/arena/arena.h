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

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif // ARENA_ASSERT

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC malloc
#endif // ARENA_MALLOC

#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE free
#endif

typedef struct Arena Arena;

void  ArenaInit(Arena* arena, size_t capacity);
void* ArenaAlloc(Arena* arena, size_t size);
void* ArenaAllocAligned(Arena* arena, size_t size, size_t alignment);
void  ArenaReset(Arena* arena);
void  ArenaDelete(Arena* arena);

#ifdef ARENA_DEBUG
size_t ArenaRemaining(Arena* arena);
size_t ArenaUsed(Arena* arena);
#endif // ARENA_DEBUG

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

void* ArenaAlloc(Arena* arena, size_t size) {
    ARENA_ASSERT(arena->offset + size < arena->capacity);
    void* ptr = (void*)(arena->data + arena->offset);
    arena->offset += size;
    return ptr;
}

void* ArenaAllocAligned(Arena* arena, size_t size, size_t alignment) {
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
#endif // ARENA_DEBUG

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // ARENA_ALLOCATOR

#endif // __ARENA_ALLOCATOR_H__
