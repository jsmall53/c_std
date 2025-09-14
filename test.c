#include <stdio.h>
#include <assert.h>

#include "j_std.h"

typedef struct Node Node;

void test_basic_mem();
void test_arena_push();
void test_arena_clear();
void test_arena_release();

void test_array_basic();
void test_array_structs();

void example_aggs();

int main(void) {
    test_arena_push();
    test_arena_clear();
    test_arena_release();

    test_array_basic();
    test_array_structs();

    example_aggs();
    return 0;
}

void test_basic_mem() {
    usize commited = 0;
    i64 page = page_size();
    void* base_mem = mem_reserve(MEGABYTES(64));

    void* first_page = mem_commit(base_mem, page * 2);
    commited += page * 2;

    void* second_page = mem_commit(base_mem + commited, page);
    commited += page;

    void* third_page = mem_commit(base_mem + commited, page);
    commited += page;

    printf("base_mem: %x, first_page: %x, second_page: %x, third_page: %x, usage: %x\n", 
            base_mem, first_page, second_page, third_page, commited);
}

typedef struct Node {
    Node* next;
} Node;

void test_arena_push() {
    Arena arena = {0};
    void* first = j_std_arena_push(&arena, KILOBYTES(4));
    assert(first != NULL);
    assert(first == arena.base);

    void* second = j_std_arena_push(&arena, KILOBYTES(4));
    assert(second != NULL);
    assert(second == (void*)((u8*)arena.base + KILOBYTES(4)));

    Arena arena2 = {0};
    Node* node1 = (Node*)j_std_arena_push(&arena2, sizeof(Node));
    assert(node1 != 0);
    assert(node1->next == 0);

    Node* node2 = (Node*)j_std_arena_push(&arena2, sizeof(Node));
    assert(node2 != 0);
    assert(node2->next == 0);
    node2->next = node1;

    Node* node3 = (Node*)j_std_arena_push(&arena2, sizeof(Node));
    assert(node3 != 0);
    assert(node3->next == 0);
    node3->next = node2;

    Node* iter = node3;
    assert(iter->next == node2);
    iter = iter->next;
    assert(iter->next == node1);
    iter = iter->next;
    assert(iter->next == NULL);
    iter = iter->next;
    assert(iter == NULL);
    printf("test_arena_push passed.\n");

    j_std_arena_release(&arena);
    j_std_arena_release(&arena2);
}

void test_arena_clear() {
    Arena arena = {0};
    void* first = j_std_arena_push(&arena, KILOBYTES(4));
    assert(arena.base != NULL);
    assert(first != NULL);
    assert(first == arena.base);
    assert(j_std_arena_get_used(&arena) == 0x1000);

    i64 committed = arena.committed;
    i64 reserved  = arena.reserved;
    j_std_arena_clear(&arena);
    assert(arena.used == 0);
    assert(j_std_arena_get_committed(&arena) == committed);
    assert(j_std_arena_get_reserved(&arena) == reserved);

    j_std_arena_release(&arena);
    printf("test_arena_clear passed.\n");
}

void test_arena_release() {
    Arena arena = {0};
    void* ptr = j_std_arena_push(&arena, KILOBYTES(4));
    assert(arena.base != NULL);
    assert(arena.reserved > 0);

    j_std_arena_release(&arena);
    assert(arena.base == NULL);
    assert(arena.reserved == 0);
    assert(arena.committed == 0);
    assert(arena.used == 0);

    printf("test_arena_release passed.\n");
}

void test_array_basic() {
    Arena arena = {0};
    ArrayF32 fa = {0};
    j_std_array_reserve((ArrayHeader*)&fa, 24, sizeof(f32));
    assert(fa.capacity == 24);

    j_std_array_push_f32(&arena, &fa, 3.14);
    j_std_array_push_f32(&arena, &fa, 3.15);
    j_std_array_push_f32(&arena, &fa, 3.16);

    assert(fa.len == 3);
    assert(fa.capacity == 24);

    printf("test_array_basic passed\n");
}

typedef struct {
    u64 id;
    char* name;
} Data;

J_ARRAY_TYPE(ArrayData, Data);

void array_push_data(Arena* arena, ArrayData* an, Data val) {
    j_std_array_fit_arena(arena, (ArrayHeader*)an, an->len + 1, sizeof(Data));
    // printf("%i\n", an->len);
    // printf("%i\n", an->array[an->len++].id);
    an->array[an->len++] = val;
}

void test_array_structs() {
    Arena arena = {0};
    Data data;
    data.id = 42;
    data.name = "Bob";
    ArrayData na = {0};

    assert(na.len == 0);
    array_push_data(&arena, &na, data);
    assert(na.capacity >= 1);
    assert(na.len == 1);
    data.id = 0;
    assert(na.array[0].id == 42);

    j_std_arena_release(&arena);
    printf("test_array_structs passed\n");
}

typedef struct {
    ArrayF32 open;
    ArrayF32 high;
    ArrayF32 low;
    ArrayF32 close;
    ArrayU64 volume;
} AggsArray;

void example_aggs() {
    AggsArray aa = {0};

    j_std_array_reserve((ArrayHeader*)&aa.open, 32, sizeof(f32));
    j_std_array_reserve((ArrayHeader*)&aa.close, 32, sizeof(f32));
    j_std_array_reserve((ArrayHeader*)&aa.high, 32, sizeof(f32));
    j_std_array_reserve((ArrayHeader*)&aa.low, 32, sizeof(f32));
    j_std_array_reserve((ArrayHeader*)&aa.volume, 32, sizeof(u64));
}
