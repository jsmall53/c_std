#include "j_std.h"
#include <assert.h>

#define MIN_ARRAY_ALLOC  16

typedef struct {
    ARRAY_HEADER
    void* array;
} VoidArray;

void* _array_grow_arena(Arena* arena, ArrayHeader* header, void* array, u64 count, u64 item_size, i32 clear_to_zero) {
    assert(header->capacity <= (SIZE_MAX - 1) / 2);
    usize new_capacity = CLAMP_MIN(header->capacity * 2, MAX(count, MIN_ARRAY_ALLOC));
    usize new_size = new_capacity * item_size;

    if (array) {
        void* new_array = j_std_arena_push(arena, new_size);
        memcpy(new_array, array, header->capacity);
        array = new_array;
    } else {
        array = j_std_arena_push(arena, new_size);
        header->len = 0;
    }
    header->capacity = new_capacity;
    return array;
}

void* _array_grow(ArrayHeader* header, void* array, u64 count, u64 item_size, i32 clear_to_zero) {
    assert(header->capacity <= (SIZE_MAX - 1) / 2);
    usize new_capacity = CLAMP_MIN(header->capacity * 2, MAX(count, MIN_ARRAY_ALLOC));
    usize new_size = new_capacity * item_size;
    if (array) {
        // realloc
        array = realloc(array, new_size);
    } else {
        // malloc
        array = malloc(new_size);
        header->len = 0;
    }
    header->capacity = new_capacity;
    return array;
}


void j_std_array_reserve(ArrayHeader* header, u64 count, u64 item_size) {
    VoidArray* va = (VoidArray*)header;
    va->array = malloc(count * item_size);
    header->len = 0;
    header->capacity = count;
    return;
}

void j_std_array_fit(ArrayHeader* header, u64 count, u64 item_size) {
    if (count <= header->capacity) {
        return;
    }

    // grow the array;
    VoidArray* va = (VoidArray*)header;
    _array_grow(header, va->array, count, item_size, false);
}

void j_std_array_fit_arena(Arena* arena, ArrayHeader* header, u64 count, u64 item_size) {
    if (count <= header->capacity) {
        return;
    }
    VoidArray* va = (VoidArray*)header;
    va->array = _array_grow_arena(arena, header, va->array, count, item_size, false);
}

#define J_ARRAY_PUSH(name, type) \
    void j_std_array_push_##type(Arena* arena, name* array_##type, type val) { \
        j_std_array_fit_arena(arena, (ArrayHeader*)array_##type, array_##type->len + 1, sizeof(type)); \
        array_##type->array[array_##type->len++] = val; \
    }

J_ARRAY_PUSH(ArrayU8, u8);
J_ARRAY_PUSH(ArrayU16, u16);
J_ARRAY_PUSH(ArrayU32, u32);
J_ARRAY_PUSH(ArrayU64, u64);
J_ARRAY_PUSH(ArrayI8,  i8);
J_ARRAY_PUSH(ArrayI16, i16);
J_ARRAY_PUSH(ArrayI32, i32);
J_ARRAY_PUSH(ArrayI64, i64);
J_ARRAY_PUSH(ArrayF32, f32);
J_ARRAY_PUSH(ArrayF64, f64);
