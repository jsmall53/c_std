#pragma once

#include <stdio.h>
#include "j_types.h"

#define KILOBYTES(n)  ((n) * 1024)
#define MEGABYTES(n)  (KILOBYTES(n) * 1024)
#define GIGABYTES(n)  (MEGABYTES(n) * 1024)

void* mem_reserve(i64 nbytes);
void* mem_commit(void* base, i64 nbytes);
void  mem_release(void* base, i64 nbytes);

void* jalloc(i64 nbytes);

#ifdef _WIN32
// @TODO: Impl on windows
#define WINDOWS
/*
 * Begin defining playform api for windows based operating system
 * */
#include <stdlib.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

i64 page_size() {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (i64)si.dwPageSize;
}

void* jalloc(i64 nbytes) {
    void* ptr = malloc(nbytes);
    if (!ptr) {
        perror("jalloc failed");
        return 0;
    }
    return ptr;
}
#else // !_WIN32
#define UNIX
/*
 * Begin defining platform api for unix based operating systems
 * */

// Something like this
// void *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, MAP_ANONYMOUS, 0);
// if (addr == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

#include <unistd.h>
#include <sys/mman.h>

i64 page_size() {
    return sysconf(_SC_PAGESIZE);
}

void* mem_reserve(i64 nbytes) {
    void* base = mmap(0, nbytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
    if (base == MAP_FAILED) {
        perror("failed to reserve memory.");
        return 0;
    }
    return base;
}

// Its the caller's job to ensure addr is aligned on
// page boundary. Use page_size() to get the page size.
void* mem_commit(void* addr, i64 nbytes) {
    i32 result = mprotect(addr, nbytes, PROT_READ | PROT_WRITE);
    if (result != 0) {
        perror("failed to commit memory.");
        return 0;
    }
    return addr;
}

void  mem_release(void* base, i64 nbytes) {
    munmap(base, nbytes);
}

#include <stdlib.h>
void* jalloc(i64 nbytes) {
    void* ptr = malloc(nbytes);
    if (!ptr) {
        perror("jalloc failed");
        return 0;
    }
    return ptr;
}

#endif // UNIX


