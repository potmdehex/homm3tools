// Created by John Åkerblom 2015-01-24

#ifndef __META_OBJECT_PUSH_H_DEF__
#define __META_OBJECT_PUSH_H_DEF__

#include "../utils/msvc_comp_stdint.h"
#include "h3mlib_meta.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)   // for utarray.h strdup
#else
extern char *strdup(const char *);      // utarray.h references strdup, not used
#endif

#include "../../../3rdparty/uthash/src/utarray.h"

#define META_OBJECT_PUSH_PTR(DYN_POINTERS, BODY, MEMBER, SIZE, SKIP) \
    { \
        struct META_DYN_POINTER *PTR_STRUCT = calloc(1, sizeof(*PTR_STRUCT)); \
        PTR_STRUCT->body = (uint8_t *)BODY; \
        PTR_STRUCT->p = (void *)MEMBER; \
        PTR_STRUCT->dyn_size = SIZE; \
        PTR_STRUCT->member_offset = ((uint8_t *)&MEMBER - (uint8_t *)BODY); \
        PTR_STRUCT->skip = SKIP; \
        if (NULL == DYN_POINTERS) \
        { \
            utarray_new(DYN_POINTERS, &ut_ptr_icd); \
        } \
        utarray_push_back(DYN_POINTERS, &PTR_STRUCT); \
    }

#define META_OBJECT_PUSH_SKIP(DYN_POINTERS, BODY, MEMBER, SKIP) \
    { \
        struct META_DYN_POINTER *PTR_STRUCT = calloc(1, sizeof(*PTR_STRUCT)); \
        PTR_STRUCT->body = (uint8_t *)BODY; \
        PTR_STRUCT->dyn_size = -1; \
        PTR_STRUCT->member_offset = ((uint8_t *)&MEMBER - (uint8_t *)BODY); \
        PTR_STRUCT->skip = SKIP; \
        if (NULL == DYN_POINTERS) \
        { \
            utarray_new(DYN_POINTERS, &ut_ptr_icd); \
        } \
        utarray_push_back(DYN_POINTERS, &PTR_STRUCT); \
    }

#define META_OBJECT_PUSH_ARRAY(DYN_POINTERS, BODY, MEMBER, ARRAY) \
    { \
        struct META_DYN_POINTER *PTR_STRUCT = calloc(1, sizeof(*PTR_STRUCT)); \
        PTR_STRUCT->body = (uint8_t *)BODY; \
        PTR_STRUCT->p = MEMBER; \
        PTR_STRUCT->p_array = ARRAY; \
        PTR_STRUCT->member_offset = ((uint8_t *)&MEMBER - (uint8_t *)BODY); \
        if (NULL == DYN_POINTERS) \
        { \
            utarray_new(DYN_POINTERS, &ut_ptr_icd); \
        } \
        utarray_push_back(DYN_POINTERS, &PTR_STRUCT); \
    }

#endif
