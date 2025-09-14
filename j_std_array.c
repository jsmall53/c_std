#include "j_std.h"
#include <assert.h>

#define MIN_ARRAY_ALLOC  16

// void* _array_grow(Arena* arena, ArrayHeader* header, void* array, u64 count, u64 item_size, i32 clear_to_zero) {
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

void _array_fit(ArrayHeader* header, u64 size) {
}

typedef struct {
    ARRAY_HEADER
    void* array;
} VoidArray;

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
    va->array = _array_grow(header, va->array, count, item_size, false);
}

