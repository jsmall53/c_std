#ifndef J_STD_H
#define J_STD_H

/*
 * TYPES
 * */

#include <stdlib.h>
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

void* j_std_arena_push(Arena* arena, u64 size);
void  j_std_arena_clear(Arena* arena);
void  j_std_arena_release(Arena* arena);

u64 j_std_arena_get_reserved(Arena* arena);
u64 j_std_arena_get_committed(Arena* arena);
u64 j_std_arena_get_used(Arena* arena);

/*
 * struct Employee {
 *    u64 id;
 *    String name;
 * };
 * Arena arena = {0};
 * Employee* employees = ArenaPushArray(&arena, 128, Employee);
 * employee[0].id = 1;
 * employee[0].name = PushString(&arena, "Bob");
 * 
 * ...
 * ArenaRelease(&arena); // deletes employees and strings...
 * */
// #define arena_push_array(arena, count, type) (type *)_arena_push(arena, (count) * sizeof(type), alignof(type[1]))

/*
 * ARRAY
 * */

/*
 * Example of how to define an array and use the api
 *
 *
 * typedef struct {
 *    ARRAY_HEADER
 *    u8* array;
 * } ByteArray;
 * 
 * Always call the value array field "array".
 *
 * Reserve an byte array of size 256
 * ByteArray ba;
 * j_std_array_reserve((ArrayHeader*)&ba, 256, sizeof(u8));
 *
 * Example of how to push a byte onto the array
 * void byte_array_push(ByteArray* ba, u8 val) {
    j_std_array_fit((ArrayHeader*)ba, ba->len + 1);
    ba->array[fa->len++] = val;
    return;
   }
 *
 * */

//
// Embed the header into array structs
//
#define ARRAY_HEADER struct { u64 len; u64 capacity; };

typedef struct { 
    u64 len; 
    u64 capacity; 
} ArrayHeader;

void j_std_array_reserve(ArrayHeader* header, u64 count, u64 item_size);
void j_std_array_fit(ArrayHeader* header, u64 count, u64 item_size); // uses exponential sizing for now.
void j_std_array_fit_arena(Arena* arena, ArrayHeader* header, u64 count, u64 item_size);
// @TODO: I don't *think* push can be a one size fits all function.
//        Should implement alongside array types for now.
// void* j_std_array_push(ArrayHeader* header, void* array);
// void ArrayShift(ArrayHeader* header, void* array, u64 itemSize, u64 fromIndex);

typedef struct {
    ARRAY_HEADER
    u8* array;
} ArrayU8;

typedef struct {
    ARRAY_HEADER
    i8* array;
} ArrayI8;

typedef struct {
    ARRAY_HEADER
    u16* array;
} ArrayU16;

typedef struct {
    ARRAY_HEADER
    i16* array;
} ArrayI16;

typedef struct {
    ARRAY_HEADER
    u32* array;
} ArrayU32;

typedef struct {
    ARRAY_HEADER
    i32* array;
} ArrayI32;

typedef struct {
    ARRAY_HEADER
    u64* array;
} ArrayU64;

typedef struct {
    ARRAY_HEADER
    i64* array;
} ArrayI64;

typedef struct {
    ARRAY_HEADER
    f32* array;
} ArrayF32;

typedef struct {
    ARRAY_HEADER
    f32* array;
} ArrayF64;


void j_std_array_push_u8(Arena* arena, ArrayU8* array_u8, u8 val);
void j_std_array_push_u16(Arena* arena, ArrayU16* array_u16, u16 val);
void j_std_array_push_u32(Arena* arena, ArrayU32* array_u32, u32 val);
void j_std_array_push_u64(Arena* arena, ArrayU64* array_u64, u64 val);

void j_std_array_push_i8(Arena* arena,  ArrayI8*  array_i8,  i8 val);
void j_std_array_push_i16(Arena* arena, ArrayI16* array_i16, i16 val);
void j_std_array_push_i32(Arena* arena, ArrayI32* array_i32, i32 val);
void j_std_array_push_i64(Arena* arena, ArrayI64* array_i64, i64 val);

void j_std_array_push_f32(Arena* arena, ArrayF32* array_f32, f32 val);
void j_std_array_push_f64(Arena* arena, ArrayF64* array_f64, f64 val);

/*
 * STRINGS
 * */

typedef struct {
    char* buf;
    u64 len;
} String;

#endif // J_STD_H
