#include <stdalign.h>
#include <assert.h>

#include "j_std.h"

#define DEFAULT_ARENA_ALIGNMENT 8

#ifndef DEFAULT_ARENA_SIZE
#define DEFAULT_ARENA_SIZE    MEGABYTES(64)
#endif

void* _arena_push(Arena* arena, u64 size, u64 alignment, i32 clear_to_zero) {
    assert(arena != NULL);
    
    if (arena->base == NULL) {
        void* base = mem_reserve(DEFAULT_ARENA_SIZE); // is this too much by default?
        if (base == 0) {
            perror("Failed to allocate arena.");
            return 0;
        }
        i64 page   = page_size();
        u64 commit = page * 2; // start with 2 page size
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
        i64 page =   page_size();
        i64 commit = page * 2;
        void* offset = (void*)((u8*)arena->base + arena->committed);
        void* next_page = mem_commit(offset, commit);
        if (next_page == 0) {
            perror("Failed to commit next page.");
            return 0;
        }
        arena->committed += commit;
    }

    // seems like we have enough memory to allocate this new chunk
    void* ptr = (void*)((u8*)arena->base + arena->used);
    arena->used += aligned_size;
    assert(arena->used <= arena->committed);

    // printf("base: %x\nreserved: %x\ncommitted: %x\nused: %x\nalloced_ptr: %x\n\n", 
    //         arena->base, arena->reserved, arena->committed, arena->used, ptr);
    return ptr;
}



void* j_std_arena_push(Arena* arena, u64 size) {
    return _arena_push(arena, size, DEFAULT_ARENA_ALIGNMENT, true);
}

void j_std_arena_clear(Arena* arena) {
    assert(arena != NULL);
    // @TODO: should we zero out the memory before setting used=0?
    arena->used = 0;
    return;
}

void j_std_arena_release(Arena* arena) {
    assert(arena != NULL);
    mem_release(arena->base, arena->reserved);
    arena->base = NULL;
    arena->reserved  = 0;
    arena->committed = 0;
    arena->used      = 0;
}

u64 j_std_arena_get_reserved(Arena* arena) {
    return arena->reserved;
}

u64 j_std_arena_get_committed(Arena *arena) {
    return arena->committed;
}

u64 j_std_arena_get_used(Arena* arena) {
    return arena->used;
}
