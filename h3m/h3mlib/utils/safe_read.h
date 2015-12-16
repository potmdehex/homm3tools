// Created by John Åkerblom 2014-11-23

#ifndef __SAFE_READ_H_DEF__
#define __SAFE_READ_H_DEF__

#ifdef __DEBUG
#define SAFE_CHECK_NOT_PASSED() *(int *)0xF17EBAD = 0xC0DEDEAD;
#else
#define SAFE_CHECK_NOT_PASSED() return 1;
#endif

/**
 * For debugging: simply crash if a read/alloc cannot safely be performed
 *
 * AKA: How To Write Safe Code In 20xx: A Complete Guide To How Dereferencing 
 * Kernel Space Pointers In Usermode Will Improve The Safety Of Your Code
**/

#define SAFE_READ_N(DEST, N, PAR) \
    if (PAR->offset + N <= PAR->raw_size) \
    { \
        memcpy(DEST, &PAR->raw[PAR->offset], N); \
    } \
    else \
    { \
        SAFE_CHECK_NOT_PASSED() \
    } \
    PAR->offset += N;

#define SAFE_READ_SIZEOF(DEST, PAR) \
    SAFE_READ_N(DEST, sizeof(*DEST), PAR)

#define SAFE_READ_SIZEOF_BUF(DEST, PAR) \
    SAFE_READ_N(DEST, sizeof(DEST), PAR)

#define SAFE_CHECK_N(N, MAX) \
    if (N > MAX) \
    { \
        SAFE_CHECK_NOT_PASSED() \
    }

#define SAFE_ALLOC_N(P, N, MAX) \
    if (N <= MAX) \
    { \
        P = calloc(1, N); \
    } \
    else \
    { \
        SAFE_CHECK_NOT_PASSED() \
    }

#define SAFE_READ_STRING(SIZE, MAX, STR, PAR) \
    SAFE_READ_SIZEOF(&SIZE, parsing) \
    if (0 != SIZE) \
    { \
        SAFE_ALLOC_N(STR, SIZE + 1, MAX + 1) \
        SAFE_READ_N(STR, SIZE, parsing) \
    }

#define SAFE_READ_COMMON_STRING(STRUCT, MAX, PAR) \
    { \
        size_t size = 0; \
        SAFE_READ_SIZEOF(&size, PAR) \
        if (0 != size) \
        { \
            SAFE_ALLOC_N(STRUCT, sizeof(STRUCT->size) + size + 1, MAX + 1) \
            STRUCT->size = size; \
            SAFE_READ_N(STRUCT->data, STRUCT->size, PAR) \
        } \
    }

#endif
