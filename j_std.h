#ifndef J_STD_H
#define J_STD_H

/*
 * TYPES
 * */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef size_t   usize;
typedef float    f32;
typedef double   f64;

/*
 * PLATFORM STUFF
 * */
// @TODO: Do I actually need this here?
#ifndef JSTD_NO_STDIO
#include <stdio.h>
#endif

#define KILOBYTES(n)  ((n) * 1024LL)
#define MEGABYTES(n)  (KILOBYTES(n) * 1024LL)
#define GIGABYTES(n)  (MEGABYTES(n) * 1024LL)

void* mem_reserve(i64 nbytes);
void* mem_commit(void* base, i64 nbytes);
void  mem_release(void* base, i64 nbytes);
void* jalloc(i64 nbytes);
i64   page_size();

/*
 * ARENA ALLOCATOR
 * */


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

typedef struct Arena {
    u64   reserved;
    u64   committed;
    u64   used;
    void* base;
} Arena;

void* j_std_arena_push(Arena* arena, u64 size);
void  j_std_arena_clear(Arena* arena);
void  j_std_arena_release(Arena* arena);

// #define arena_push_array(arena, count, type) (type *)_arena_push(arena, (count) * sizeof(type), alignof(type[1]))

/*
 * ARRAY
 * */

#define _ArrayHeader_ struct { u64 count; u64 capacity; };

typedef struct { 
    u64 count; 
    u64 capacity; 
} ArrayHeader;

void* ArrayGrow();
void ArrayShift(ArrayHeader* header, void* array, u64 itemSize, u64 fromIndex);


/*
 * STRINGS
 * */

typedef struct {
    char* buf;
    u64 len;
} String;

#endif // J_STD_H
