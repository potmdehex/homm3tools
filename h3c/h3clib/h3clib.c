// Created by John Åkerblom 2015-12-03

#include "h3clib.h"
#include "h3clib_ctx.h"
#include "memmem.h"

#include <h3mlib.h>
#include <gzip_utils.h>
#include <stdint.h>
#include <stdio.h>

// Campaigns can actually have more maps than this, but Chronicles have only 8
#define MAX_MAPS 8

#ifdef _MSC_VER
    #pragma warning(disable:4996)   // M$ standard lib unsafe
#endif

static const uint8_t NEEDLE_FILENAME[] = ".h3m";
static const uint8_t NEEDLE_SOD[] = { 0x1C, 0x00, 0x00, 0x00, 0x01 };
static const uint8_t NEEDLE_H3C[] = { 0x1D, 0x00, 0x00, 0x00, 0x01 };

int h3c_read_convert_mem(h3clib_ctx_t *ctx,
    uint8_t *raw, uint32_t raw_size, enum H3C_FORMAT target_format,
    enum H3C_FORMAT *source_format, h3m_error_cb_t cb_error,
    void *cb_data)
{
    // For now use a hacky way of conversion using memmem and inplace byte 
    // replacement until actual parsing is implemented. 
    // 1. change first 4 bytes
    // 2. search for each .h3m, save pointers, count amount
    // 3. search for 1c 00 00 00 01 / vice versa, replace. distance to next 1c 00 00 00 01 is map size, save
    // 4. set the map sizes
    // 5. change campaign map
    unsigned int pos = 0;
    unsigned int prev_pos = 0;
    unsigned int h3m_size = 0;
    unsigned int h3m_count = 0;
    unsigned int needle_size = 0;
    uint32_t *size_pointers[MAX_MAPS];
    const uint8_t *needle = NULL;
    int ret = 0;
    int i = 0;
    
    // Right now only SoD<->CHR conversion is supported, target opposite format
    if (*(uint32_t *)raw == H3C_FORMAT_CHR)
        target_format = H3C_FORMAT_SOD;
    else if (*(uint32_t *)raw == H3C_FORMAT_SOD)
        target_format = H3C_FORMAT_CHR;
    else {
        gu_free(raw);
        return 1;
    }

    // Save source h3c format identifier
    *source_format = *(uint32_t *)raw;

    // Set h3c format identifier
    *(uint32_t *)raw = (target_format == H3C_FORMAT_SOD) ? H3C_FORMAT_SOD
        : H3C_FORMAT_CHR;

    // Find the locations of the size values for each map
    for (i = 0; i < MAX_MAPS; ++i) {
        uint8_t *p = memmem(raw + pos, raw_size - pos, NEEDLE_FILENAME,
            sizeof(NEEDLE_FILENAME)-1);

        if (p == NULL)
            break;

        // Save pointer to this map's size
        size_pointers[i] = (uint32_t *)(p + sizeof(NEEDLE_FILENAME)-1);

        // Set new position past this map's size
        pos = p - raw + sizeof(uint32_t);
    }
    h3m_count = i;

    // Find the h3m format values and replace them
    needle = (*source_format == H3C_FORMAT_SOD)? NEEDLE_SOD : NEEDLE_H3C;
    needle_size = (*source_format == H3C_FORMAT_SOD) ? sizeof(NEEDLE_SOD) :
        sizeof(NEEDLE_H3C);
    for (i = 0; i < MAX_MAPS; ++i) {
        uint8_t *p = memmem(raw + pos, raw_size - pos, needle, needle_size);

        if (p == NULL)
            break;

        // Change the format identifier of this h3m
        *(uint32_t *)p = (target_format == H3C_FORMAT_SOD) ? H3M_FORMAT_SOD
            : H3M_FORMAT_CHR;

        // Set new position past this map's format identifier
        pos = p - raw + sizeof(uint32_t);

        // Calculate size of the previous map by comparing current pos with previous
        if (i != 0) {
            h3m_size = pos - prev_pos;
            *size_pointers[i-1] = h3m_size;
        }
        prev_pos = pos;
    }

    // Set size of last map as well, which is not set in the loop
    if (size_pointers[h3m_count - 1] != NULL) {
        *(size_pointers[h3m_count - 1]) = (raw_size - (uint32_t)pos)
            + sizeof(uint32_t);
    }

    // Set campaign map. H3C campaigns contain 5 or 8 maps, so for conversion
    // to SoD choose campaign maps with that amount of h3ms
    // Chronicles is always 0 here as it only has one campaign map
    if (target_format == H3C_FORMAT_CHR) {
        *((uint8_t *)raw + sizeof(uint32_t)) = 0;
    } 
    else {
        uint8_t *p = ((uint8_t *)raw + sizeof(uint32_t));

        switch (h3m_count)
        {
        case 5:
             *p = H3C_MAP_BIRTH_OF_A_BARBARIAN; // has 5 h3ms
            break;
        case 8:
            *p = H3C_MAP_ARMAGEDDON; // has 8 h3ms
            break;
        default:
            break;
        }   
    }

    // TODO replace Tarnum hero for CHR->SoD, and remove/replace any missing 
    // .defs

    *ctx = calloc(1, sizeof(*ctx));
    (*ctx)->raw = raw;
    (*ctx)->raw_size = raw_size;

    return 0;
}

int h3c_read_convert(h3clib_ctx_t *ctx,
    const char *filename, enum H3C_FORMAT target_format,
    enum H3C_FORMAT *source_format, h3m_error_cb_t cb_error,
    void *cb_data)
{
    uint8_t *raw = NULL;
    uint32_t raw_size = 0;
    int ret = 0;

    if (0 != (ret = gu_decompress_file_to_mem(filename, (void *)&raw,
        (long *)&raw_size))) {
        return ret;
    }

    // TODO support multiply compressed files here, otherwise only header is
    // uncompressed (h3ms are compressed separately)

    return h3c_read_convert_mem(ctx, raw, raw_size, target_format,
        source_format, cb_error, cb_data);
}

int h3c_write(h3clib_ctx_t ctx, const char *filename)
{
    FILE *f = fopen(filename, "wb");

    if (f == NULL)
        return 1;

    fwrite(ctx->raw, 1, ctx->raw_size, f);

    fclose(f);

    return 0;
}

int h3c_exit(h3clib_ctx_t *ctx)
{
    gu_free((*ctx)->raw);
    free(*ctx);
    *ctx = NULL;

    return 0;
}

#if defined _WIN32 && defined _MSC_VER

int h3c_write_u(h3clib_ctx_t ctx, const wchar_t *filename)
{
    FILE *f = _wfopen(filename, L"wb");

    if (f == NULL)
        return 1;

    fwrite(ctx->raw, 1, ctx->raw_size, f);

    fclose(f);

    return 0;
}

int h3c_read_convert_u(h3clib_ctx_t *ctx,
    const wchar_t *filename,
    enum H3C_FORMAT target_format,
    enum H3C_FORMAT *source_format, h3c_error_cb_t cb_error,
    void *cb_data)
{
    uint8_t *raw = NULL;
    uint32_t raw_size = 0;
    int ret = 0;

    if (0 != (ret = gu_decompress_file_to_mem_u(filename, (void *)&raw,
        (long *)&raw_size))) {
        return ret;
    }

    // TODO support multiply compressed files here, otherwise only header is
    // uncompressed (h3ms are compressed separately)

    return h3c_read_convert_mem(ctx, raw, raw_size, target_format,
        source_format, cb_error, cb_data);
}

#endif
