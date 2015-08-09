// Created by John Åkerblom 2015-01-06

enum H3MSNAKE_INPUT
{
    H3MSNAKE_INPUT_NONE,
    H3MSNAKE_INPUT_LEFT,
    H3MSNAKE_INPUT_UP,
    H3MSNAKE_INPUT_RIGHT,
    H3MSNAKE_INPUT_DOWN,
    H3MSNAKE_INPUT_START
};

enum H3MSNAKE_STATE
{
    H3MSNAKE_STATE_ALIVE,
    H3MSNAKE_STATE_DEAD
};

int h3msnake_init(const char *filename);
int h3msnake_exit(void);
enum H3MSNAKE_STATE h3msnake_update_state(enum H3MSNAKE_INPUT input);
