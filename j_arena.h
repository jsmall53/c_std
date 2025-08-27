#include "j_types.h";

#define DEFAULT_ARENA_ALIGNMENT 8

typedef struct Arena {
    u64   reserved;
    u64   committed;
    u64   used;
    void* base;
} Arena;

void* _arena_push(Arena* arena, u64 size, u64 alignment, i32 clear_to_zero) {

    return NULL;
}

#define arena_push(arena, size)              _arena_push((arena), (size), DEFAULT_ARENA_ALIGNMENT, true)
#define arena_push_struct(arena, type)       (type *)_arena_push((arena), (sizeof((type))), (alignof((type))), true)
#define arena_push_array(arena, count, type) (type *)_arena_push(arena, (count) * sizeof(type), alignof(type[1]))

void arena_clear(Arena* arena);
void arena_release(Arena* arena);


