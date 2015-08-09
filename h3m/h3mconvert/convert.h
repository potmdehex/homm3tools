// Created by John Åkerblom 2015-01-28

#ifndef __CONVERT_H_DEF__
#define __CONVERT_H_DEF__

#include <stdint.h>
#include <windows.h>

#define CONVERT_RESULT_SUCCESS 0
#define CONVERT_RESULT_FAIL 1
#define CONVERT_RESULT_SKIP 2

typedef int (*convert_cb_t)(const wchar_t *filename, uint32_t format, uint32_t result, void *cb_data);

int convert_to_roe(const wchar_t *src_path, const wchar_t *filename, const wchar_t *dest_path, convert_cb_t cb, void *cb_data);

#endif

