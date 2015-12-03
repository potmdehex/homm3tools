// Created by John Åkerblom 2015-01-28
#include "convert.h"

#include <Windows.h>
#include <h3clib.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ standard lib unsafe
    #define snwprintf _snwprintf
#endif

int convert_h3c(const wchar_t *src_path, const wchar_t *filename, const wchar_t *dest_path, convert_cb_t cb, void *cb_data)
{
    h3clib_ctx_t h3c = NULL;
    wchar_t fullpath[MAX_PATH] = { 0 };
    wchar_t fullpath_out[MAX_PATH] = { 0 };
    const wchar_t *version_src = L"unknown";
    const wchar_t *version_dst = L"unknown";
    enum H3M_FORMAT fm_src = 0;
    int ret = 0;
    int result = 0;

    snwprintf(fullpath, sizeof(fullpath) / sizeof(fullpath[0])-1, L"%s\\%s", src_path, filename);

    h3c_read_convert_u(&h3c, fullpath, 0, &fm_src, NULL, NULL);

    // Error check, if h3c is NULL then conversion failed
    if (NULL == h3c) {
        if (NULL != cb) {

            switch (fm_src)
            {
            case H3C_FORMAT_SOD:
            case H3C_FORMAT_CHR:
                result = CONVERT_RESULT_FAIL;
                break;
            default:
                CONVERT_RESULT_SKIP;
                break;
            }

            if (0 != (ret = cb(filename, fm_src, result, cb_data))) {
                return ret;
            }
        }

        return 0;
    }

    switch (fm_src)
    {
    case H3C_FORMAT_SOD:
        version_src = L"SoD";
        version_dst = L"CHR";
        break;
    case H3C_FORMAT_CHR:
        version_src = L"CHR";
        version_dst = L"SoD";
        break;
    default:
        break;
    }

    snwprintf(fullpath_out, sizeof(fullpath_out) / sizeof(fullpath_out[0]) - 1, 
        L"%s\\[%s-%s] %s", dest_path, version_src, version_dst, filename);

    h3c_write_u(h3c, fullpath_out);
    //h3m_compress(fullpath_out, fullpath_out);

    h3c_exit(&h3c);

    if (NULL != cb) {
        if (0 != (ret = cb(filename, fm_src, CONVERT_RESULT_SUCCESS, cb_data))) {
            return ret;
        }
    }

    return 0;
}

