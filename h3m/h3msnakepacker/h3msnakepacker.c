#include <h3mlib.h>

#define DESC "Controls:\nArrow keys: move\nF5: restart\n\nBy @potmdehex, http://h3minternals.net"
#define DESC_PREFIX_COMPLETE "h3msnake2 non-HD Mod version\n"
#define DESC_PREFIX_HDMOD "h3msnake2 HD Mod version\n"

int main(void)
{
    h3mlib_ctx_t h3m = NULL;
    int od_index = 0;

    h3m_init_min(&h3m, H3M_FORMAT_ROE, H3M_SIZE_SMALL);
    h3m_object_add(h3m, "Castle", 5, 5, 0, &od_index);
    h3m_object_set_owner(h3m, od_index, 0);
    h3m_player_enable(h3m, 0);

    h3m_name_set(h3m, "h3msnake2");

    if (0 != h3m_code_set_dll(h3m, "..\\..\\..\\..\\OUTPUT\\Release\\h3complete_snakeloader.dll")) {
        h3m_exit(&h3m);
        return 1;
    }

    h3m_desc_set(h3m, DESC_PREFIX_COMPLETE DESC);
    h3m_code_set_target(h3m, H3M_CODE_TARGET_COMPLETE);
    h3m_write(h3m, "h3msnake2_h3complete.h3m");

    h3m_desc_set(h3m, DESC_PREFIX_HDMOD DESC);
    h3m_code_set_target(h3m, H3M_CODE_TARGET_HDMOD);
    h3m_write(h3m, "h3msnake2_hdmod.h3m");

    h3m_exit(&h3m);

    return 0;
}
