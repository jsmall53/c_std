#include <stdio.h>
#include "platform.h"

int main(void) {
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

    return 0;
}

