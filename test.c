#include <stdio.h>
#include <assert.h>

#include "platform.h"
#include "j_arena.h"

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
    Arena arena;
    void* first = arena_push(&arena, KILOBYTES(4));
    assert(first != NULL);
    assert(first == arena.base);

    void* second = arena_push(&arena, KILOBYTES(4));
    assert(second != NULL);
    assert(second == (void*)((u8*)arena.base + KILOBYTES(4)));

    Arena arena2;
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
}

int main(void) {
    test_arena_push();
    return 0;
}


