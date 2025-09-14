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


void j_std_array_push_u8(Arena* arena, ArrayU8* array_u8, u8 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_u8, array_u8->len + 1, sizeof(u8));
    array_u8->array[array_u8->len++] = val;
}

void j_std_array_push_u16(Arena* arena, ArrayU16* array_u16, u16 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_u16, array_u16->len + 1, sizeof(u16));
    array_u16->array[array_u16->len++] = val;
}

void j_std_array_push_u32(Arena* arena, ArrayU32* array_u32, u32 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_u32, array_u32->len + 1, sizeof(u32));
    array_u32->array[array_u32->len++] = val;
}

void j_std_array_push_u64(Arena* arena, ArrayU64* array_u64, u64 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_u64, array_u64->len + 1, sizeof(u64));
    array_u64->array[array_u64->len++] = val;
}

void j_std_array_push_i8(Arena* arena,  ArrayI8*  array_i8,  i8 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_i8, array_i8->len + 1, sizeof(i8));
    array_i8->array[array_i8->len++] = val;
}

void j_std_array_push_i16(Arena* arena, ArrayI16* array_i16, i16 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_i16, array_i16->len + 1, sizeof(i16));
    array_i16->array[array_i16->len++] = val;
}

void j_std_array_push_i32(Arena* arena, ArrayI32* array_i32, i32 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_i32, array_i32->len + 1, sizeof(i32));
    array_i32->array[array_i32->len++] = val;
}

void j_std_array_push_i64(Arena* arena, ArrayI64* array_i64, i64 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_i64, array_i64->len + 1, sizeof(i64));
    array_i64->array[array_i64->len++] = val;
}

void j_std_array_push_f32(Arena* arena, ArrayF32* array_f32, f32 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_f32, array_f32->len + 1, sizeof(f32));
    array_f32->array[array_f32->len++] = val;
}

void j_std_array_push_f64(Arena* arena, ArrayF64* array_f64, f64 val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)array_f64, array_f64->len + 1, sizeof(f64));
    array_f64->array[array_f64->len++] = val;
}
