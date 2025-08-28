#include <stdio.h>
#include <assert.h>

#include "platform.h"
#include "j_arena.h"

typedef struct Node Node;

void test_basic_mem();
void test_arena_push();
void test_arena_clear();
void test_arena_release();

int main(void) {
    test_arena_push();
    test_arena_clear();
    test_arena_release();

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
    struct Node* next;
} Node;

void test_arena_push() {
    Arena arena = {0};
    void* first = arena_push(&arena, KILOBYTES(4));
    assert(first != NULL);
    assert(first == arena.base);

    void* second = arena_push(&arena, KILOBYTES(4));
    assert(second != NULL);
    assert(second == (void*)((u8*)arena.base + KILOBYTES(4)));

    Arena arena2 = {0};
    Node* node1 = arena_push_struct(&arena2, Node);
    assert(node1 != 0);
    assert(node1->next == 0);

    Node* node2 = arena_push_struct(&arena2, Node);
    assert(node2 != 0);
    assert(node2->next == 0);
    node2->next = node1;

    Node* node3 = arena_push_struct(&arena2, Node);
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

    arena_release(&arena);
    arena_release(&arena2);
}

void test_arena_clear() {
    Arena arena = {0};
    void* first = arena_push(&arena, KILOBYTES(4));
    assert(arena.base != NULL);
    assert(first != NULL);
    assert(first == arena.base);
    assert(arena.used == 0x1000);

    i64 committed = arena.committed;
    i64 reserved  = arena.reserved;
    arena_clear(&arena);
    assert(arena.used == 0);
    assert(arena.committed == committed);
    assert(arena.reserved == reserved);

    arena_release(&arena);
    printf("test_arena_clear passed.\n");
}

void test_arena_release() {
    Arena arena = {0};
    void* ptr = arena_push(&arena, KILOBYTES(4));
    assert(arena.base != NULL);
    assert(arena.reserved > 0);

    arena_release(&arena);
    assert(arena.base == NULL);
    assert(arena.reserved == 0);
    assert(arena.committed == 0);
    assert(arena.used == 0);

    printf("test_arena_release passed.\n");
}
