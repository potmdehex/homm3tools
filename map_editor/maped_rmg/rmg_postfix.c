// Created by John Åkerblom 2015-01-30

#include "rmg_postfix.h"

#include <h3mlib.h>
#include <h3m_structures/object_details/h3m_od_body_dynamic.h> // HACK

#include "globals.h"

#include <Windows.h> // outputdebugstringa

const char *town_defs[] = {
    "avccasx0.def",
    "avcramx0.def",
    "avctowx0.def",
    "avcinfx0.def",
    "avcnecx0.def",
    "avcdunx0.def",
    "avcftrx0.def",
    "avcstrx0.def",
    "avccast0.def",
    "avctowr0.def",
    "avcinft0.def",
    "avcnecr0.def",
    "avcftrt0.def",
    "avcstro0.def",
    "avcramp0.def",
    "avcdung0.def"
};
#define TOWN_OA_COUNT 16

struct POSTFIX_CTX
{
    uint32_t random_town_oa;
    uint32_t preset_town_oa[TOWN_OA_COUNT];
};

static int _enum_od_cb(uint8_t *x, uint8_t *y, uint8_t *z, uint32_t *oa_index, uint8_t *data, size_t data_size, void *cb_data)
{
    struct POSTFIX_CTX *ps_ctx = (struct POSTFIX_CTX *)cb_data;
    uint32_t idx = *oa_index;

    char dbg[256] = {0};

    for (unsigned int i = 0; i < TOWN_OA_COUNT; ++i)
    {
        if (idx == ps_ctx->preset_town_oa[i])
        {
            if (*data != 0xFF) // owner?
            {
                *oa_index = ps_ctx->random_town_oa;
            }
            
        }
    }

    return 0;
}

static void _postfix_init(h3mlib_ctx_t h3m, struct POSTFIX_CTX *ps_ctx)
{
    memset(ps_ctx, 0xFF, sizeof((*ps_ctx)));
    for (unsigned int i = 0; i < TOWN_OA_COUNT; ++i)
    {
        h3m_get_oa_index(h3m, town_defs[i], &ps_ctx->preset_town_oa[i]);

        //char dbg[256] = {0};
        //sprintf(dbg, "%s : %d", town_defs[i], ps_ctx->preset_town_oa[i]);
        //OutputDebugStringA(dbg);
    }
}

int rmg_postfix(const char *filename)
{
    h3mlib_ctx_t h3m = NULL;
    struct POSTFIX_CTX ps_ctx = { 0 };
    int oa_index = 0;
    
    OutputDebugStringA("@rmg_postfix");
    OutputDebugStringA(filename);

    if (0 == g_selectable_towns)
    {
        return 0;
    }

    OutputDebugStringA("@rmg_postfix fix map");

    h3m_read(&h3m, filename);

    if (NULL == h3m)
    {
        OutputDebugStringA("failed to open map");
        return 1;
    }

    _postfix_init(h3m, &ps_ctx);

    h3m_add_oa_by_def(h3m, "avcranx0.def", &ps_ctx.random_town_oa);
    h3m_enum_od(h3m, _enum_od_cb, (void *)&ps_ctx);
    h3m_towns_selectable(h3m);

    h3m_desc_append(h3m, " [h3minternals.net]");

    h3m_write(h3m, filename);
    h3m_exit(&h3m);

    return 0;
}
