// Created by John Åkerblom 2015-01-28
#include "convert.h"

#include <Windows.h>
#include <h3mlib.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ standard lib unsafe
    #define snwprintf _snwprintf
#endif

int convert_to_roe(const wchar_t *src_path, const wchar_t *filename, const wchar_t *dest_path, convert_cb_t cb, void *cb_data)
{
    h3mlib_ctx_t h3m = NULL;
    wchar_t fullpath[MAX_PATH] = { 0 };
    wchar_t fullpath_out[MAX_PATH] = { 0 };
    wchar_t fullpath_patch[MAX_PATH] = { 0 };
    wchar_t *version = L"unknown";
    enum H3M_FORMAT fm_src = 0;
    int ret = 0;

    snwprintf(fullpath, sizeof(fullpath) / sizeof(fullpath[0])-1, L"%s\\%s", src_path, filename);

    h3m_read_convert_u(&h3m, fullpath, H3M_FORMAT_ROE, &fm_src, NULL, NULL, NULL, NULL);

    if (NULL == h3m)
    {
        if (NULL != cb)
        {
            if (0 != (ret = cb(filename, fm_src, (H3M_FORMAT_ROE != fm_src) ? CONVERT_RESULT_FAIL : CONVERT_RESULT_SKIP, cb_data)))
            {
                return ret;
            }
        }

        return 0;
    }

    switch (fm_src)
    {
    case H3M_FORMAT_AB:
        version = L"AB";
        break;
    case H3M_FORMAT_SOD:
        version = L"SoD";
        break;
    case H3M_FORMAT_WOG:
        version = L"WoG";
        break;
    default:
        version = L"??";
    }

    snwprintf(fullpath_out, sizeof(fullpath_out) / sizeof(fullpath_out[0])-1, L"%s\\[%s-RoE] %s", dest_path, version, filename);

    snwprintf(fullpath_patch, sizeof(fullpath_patch)-sizeof(fullpath_patch[0]) - 1, L"h3mpatches\\patch_%s", filename);

    if (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(fullpath_patch))
    {
        h3m_object_patch_u(h3m, fullpath_patch);
    }

    h3m_write_u(h3m, fullpath_out);
    //h3m_compress(fullpath_out, fullpath_out);

    h3m_exit(&h3m);

    if (NULL != cb)
    {
        if (0 != (ret = cb(filename, fm_src, CONVERT_RESULT_SUCCESS, cb_data)))
        {
            return ret;
        }
    }

    return 0;
}

