#pragma once
#include <stdio.h>
#include <assert.h>

#include "platform.h"
#include "j_types.h"

#define DEFAULT_ARENA_ALIGNMENT 8

// @TODO: turn the below stuff into functional code once the 
//          base arena functionality is finished.
/*
 * Example using object pool with an arena for reference:
 *  typedef struct {
 *      Event* next;
 *      ...
 *  } Event;
 *
 *  Arena event_arena = {0};
 *  Event* first_event; Event* last_event;
 *  Event* free_event = 0;
 *
 *  Event* push_event() {
 *      Event* event = PoolAlloc(&event_arena, free_event);
 *      QueuePush(first_event, last_event, event);
 *      return event;
 *  }
 *
 *  void process_events() {
 *      Event* eventIter = firstEvent;
 *      while (eventIter != NULL) {
 *          Event* event = eventIter;
 *          eventIter = eventIter->next;
 *
 *          // Process event
 *          pool_free(free_event, event);
 *      }
 *  }
 *
 * ******************
 *  typedef struct {
 *      PoolNode* next;
 *  } PoolNode;
 *
 *  void _pool_free(PoolNode* free_node, PoolNode* node) {
 *      node->next = free_node;
 *      free_node  = node;
 *      return;
 *  }
 *
 *  void* _pool_alloc(Arena* arena, PoolNode** pool, u64 item_size, bool/b32 clear_to_zero, u32 count);
 *
 *  #define pool_alloc(arena, pool)                     _pool_alloc(arena, (Pool**)&pool, sizeof(*(pool)), true, DEFAULT_BATCH_COUNT)
 *  #define pool_alloc_ex(arena, pool, clear, count)    _pool_alloc(arena, (Pool**)&pool, sizeof(*(pool)), clear, count)
 *  #define pool_free(pool, ptr)                        (((PoolNode*)ptr)->next = (PoolNode*) pool, *((void**)&(pool)) = ptr)
 *
 * */

#define DEFAULT_ARENA_SIZE    MEGABYTES(64)

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)
#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)
#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))


typedef struct Arena {
    u64   reserved;
    u64   committed;
    u64   used;
    void* base;
} Arena;

void* _arena_push(Arena* arena, u64 size, u64 alignment, i32 clear_to_zero) {
    assert(arena != NULL);
    
    if (arena->base == NULL) {
        void* base = mem_reserve(DEFAULT_ARENA_SIZE); // is this too much by default?
        if (base == 0) {
            perror("Failed to allocate arena.");
            return 0;
        }
        i64 page   = page_size();
        u64 commit = page; // start with single page size
        base = mem_commit(base, commit);
        arena->base      = base;
        arena->reserved  = DEFAULT_ARENA_SIZE;
        arena->committed = commit;
        arena->used      = 0;
    }

    usize aligned_size = ALIGN_UP(size, alignment);
    usize total_available = arena->reserved - arena->used;
    if (total_available <= aligned_size) {
        perror("Out of memory!!!");
        return 0;
    }

    usize available = arena->committed - arena->used;
    if (aligned_size > available) {
        // commit the next page.
        i64 page = page_size();
        void* offset = (void*)((u8*)arena->base + arena->committed);
        void* next_page = mem_commit(offset, page);
        if (next_page == 0) {
            perror("Failed to commit next page.");
            return 0;
        }
        arena->committed += page;
    }

    // seems like we have enough memory to allocate this new chunk
    void* ptr = (void*)((u8*)arena->base + arena->used);
    arena->used += aligned_size;
    assert(arena->used <= arena->committed);

    printf("base: %x\nreserved: %x\ncommitted: %x\nused: %x\nalloced_ptr: %x\n\n", 
            arena->base, arena->reserved, arena->committed, arena->used, ptr);
    return ptr;
}

#define arena_push(arena, size)              _arena_push((arena), (size), DEFAULT_ARENA_ALIGNMENT, true)
#define arena_push_struct(arena, type)       (type *)_arena_push((arena), sizeof(type), alignof(type), true)
#define arena_push_array(arena, count, type) (type *)_arena_push(arena, (count) * sizeof(type), alignof(type[1]))

void arena_clear(Arena* arena);
void arena_release(Arena* arena);


