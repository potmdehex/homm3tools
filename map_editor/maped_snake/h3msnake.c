// Created by John Åkerblom 2015-01-06

#include "h3msnake.h"
#include "globals.h"

#include <h3mlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // Standard lib unsafe
    #define snprintf _snprintf
#endif

#define DIR_DOWN 0
#define DIR_LEFT 1
#define DIR_UP 2
#define DIR_RIGHT 3

#define ARENA_START 8
#define ARENA_SIZE 10

#define CAGE_START ARENA_START + ARENA_SIZE + 5
#define CAGE_SIZE 5

#define START_POS ARENA_START + 1
#define START_SIZE 4

struct NODE
{
    int od_index;
    int x;
    int y;
    struct NODE *next;
    struct NODE *prev;
};

struct APPLE
{
    int x;
    int y;
    int od_index;
};

static struct STATE
{
    h3mlib_ctx_t h3m;
    char *filename;
    unsigned char direction;
    struct NODE *head;
    struct NODE *tail;
    int size;
    int node_count;
    struct APPLE apple;
    int score;
    const char *def_apple;
    int terrain;
    int cage[36][36];
} f_state;

int has_node(int x, int y)
{
    struct NODE *n = f_state.tail;
    int ret = 0;

    for (int i = 0; i < f_state.node_count; ++i)
    {
        if (n->x == x && n->y == y)
        {
            return 1;
        }
        n = n->next;
    }

    return ret;
}

int has_apple(int x, int y)
{
    return (x == f_state.apple.x && y == f_state.apple.y);
}

void place_apple(void)
{
    int x;
    int y;

    do
    {
        x = ARENA_START + 1 + (rand() % (ARENA_SIZE - 1));
        y = ARENA_START + 1 + (rand() % (ARENA_SIZE - 1));
    } while (has_node(x, y));

    f_state.apple.x = x;
    f_state.apple.y = y;
    h3m_object_move(f_state.h3m, f_state.apple.od_index, f_state.apple.x, f_state.apple.y, 0);
}

void place_caged(void)
{
    int x = 0;
    int y = 0;

    do
    {
        x = rand() % (CAGE_SIZE - 1);
        y = rand() % (ARENA_SIZE - 1);
    } while (0 != f_state.cage[x][y] || f_state.score >= 50);
    
    h3m_object_add(f_state.h3m, f_state.def_apple, CAGE_START + x, ARENA_START + y, 0, NULL);

    f_state.cage[x][y] = 1;
}

int h3msnake_init(const char *filename)
{
    struct NODE *nodes[START_SIZE];
    int od_index = 0;

    memset(&f_state, 0, sizeof(f_state));
    h3m_init_min(&f_state.h3m, H3M_FORMAT_ROE, 36);
    h3m_write(f_state.h3m, filename);

    srand((unsigned int)time(NULL));

    f_state.filename = _strdup(filename);
    g_map_filename = f_state.filename;
    f_state.direction = DIR_RIGHT;

    for (int i = 0; i < START_SIZE; ++i)
    {
        h3m_object_add(f_state.h3m, "Serpent Fly", START_POS + i, START_POS, 0, &od_index);
        nodes[i] = calloc(1, sizeof(*(nodes[i])));
        nodes[i]->od_index = od_index;
        nodes[i]->x = START_POS + i;
        nodes[i]->y = START_POS;
    }
    f_state.head = nodes[START_SIZE - 1];
    f_state.tail = nodes[0];

    for (int i = 0; i < START_SIZE - 1; ++i)
    {
        nodes[i]->next = nodes[i + 1];
        nodes[i]->prev = (0 != -i) ? nodes[i - 1] : NULL;
    }
    nodes[0]->prev = f_state.head;
    nodes[START_SIZE - 1]->next = f_state.tail;
    f_state.size = START_SIZE;
    f_state.node_count = START_SIZE;
    //f_state.tail->prev = f_state.head;

    uint8_t terrain_types[36 * 36];
    uint8_t river_types[36 * 36];
    uint8_t road_types[36 * 36];
    uint8_t impassable[36][36];

    f_state.terrain = (rand() + 1) % 8;
    memset(terrain_types, f_state.terrain, sizeof(terrain_types));
    memset(river_types, 0, sizeof(river_types));
    memset(road_types, 0, sizeof(road_types));
    memset(impassable, 1, sizeof(impassable));

#if 1
    h3m_generate_tiles(f_state.h3m, 36, 0, (uint8_t *)terrain_types, (uint8_t *)road_types, (uint8_t *)river_types);

    const char *defs_apple[] = { "Skeleton", "Gold Golem", "Pikeman", "Gremlin", "Gnoll", "Goblin", "Troglodyte", "Imp" };
    const char *defs_wall[] = { "Skeleton Warrior", "Diamond Golem", "Halberdier", "Master Gremlin", "Gnoll Marauder", "Hobgoblin", "Infernal Troglodyte", "Familiar" };

    for (int i = ARENA_START; i < ARENA_START+ARENA_SIZE; ++i)
    {
        h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], i, ARENA_START + ARENA_SIZE, 0, NULL);
        h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START + ARENA_SIZE, i, 0, NULL);
        h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], i, ARENA_START, 0, NULL);
        h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START, i, 0, NULL);
        for (int j = ARENA_START; j < ARENA_START + ARENA_SIZE + 1; ++j)
            impassable[i][j] = 0;

        for (int j = CAGE_START; j < CAGE_START + CAGE_SIZE; ++j)
            impassable[i][j] = 0;
    }

    h3m_impassable_fill(f_state.h3m, (uint8_t *)impassable, 36);
    h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START + ARENA_SIZE, ARENA_START, 0, NULL);
    h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START, ARENA_START + ARENA_SIZE, 0, NULL);
    h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START, ARENA_START, 0, NULL);
    h3m_object_add(f_state.h3m, defs_wall[f_state.terrain], ARENA_START + ARENA_SIZE, ARENA_START + ARENA_SIZE, 0, NULL);

#endif

    f_state.def_apple = defs_apple[f_state.terrain];

    h3m_object_add(f_state.h3m, f_state.def_apple, 15, 0, 0, &f_state.apple.od_index);

    place_apple();

    return 0;
}

int h3msnake_exit(void)
{
    if (NULL != f_state.h3m)
    {
        h3m_exit(&f_state.h3m);
        f_state.h3m = NULL;
    }
    if (NULL != f_state.filename)
    {
        free(f_state.filename);
        f_state.filename = NULL;
    }

    //TODO free nodes

    return 0;
}

void _new_head(struct NODE *n, int x_new, int y_new)
{
    n->prev = f_state.head;
    n->x = x_new;
    n->y = y_new;
    f_state.head = n;
    n->prev->next = n;
}

int _update_nodes(int x_inc, int y_inc)
{
    struct NODE *n = NULL;
    int i = 0;
    int od_index = 0;
    int x_new = f_state.head->x + x_inc;
    int y_new = f_state.head->y + y_inc;

    if (has_node(x_new, y_new) || x_new < ARENA_START + 1 || y_new < ARENA_START + 1
        || x_new >= ARENA_START + ARENA_SIZE || y_new >= ARENA_START + ARENA_SIZE)
    {
        return 1;
    }

    if (f_state.node_count >= f_state.size)
    {
        _new_head(f_state.tail, x_new, y_new);
        f_state.tail = f_state.tail->next;
        h3m_object_move(f_state.h3m, f_state.head->od_index, f_state.head->x, f_state.head->y, 0);
    }
    else
    {
        n = calloc(1, sizeof(*n));
        _new_head(n, x_new, y_new);

        ++f_state.node_count;

        h3m_object_add(f_state.h3m, "Serpent Fly", n->x, n->y, 0, &n->od_index);
    }

    if (has_apple(x_new, y_new))
    {
        place_apple();
        place_caged();
        ++f_state.score;
        ++f_state.size;
    }

    return 0;
}

enum H3MSNAKE_STATE h3msnake_update_state(enum H3MSNAKE_INPUT input)
{
    enum H3MSNAKE_STATE state = H3MSNAKE_STATE_ALIVE;
    int x_inc = 0;
    int y_inc = 0;
    switch (input)
    {
    case H3MSNAKE_INPUT_START:
        ExitProcess(1);
    case H3MSNAKE_INPUT_LEFT:
        f_state.direction = DIR_LEFT;
        break;
    case H3MSNAKE_INPUT_DOWN:
        f_state.direction = DIR_DOWN;
        break;
    case H3MSNAKE_INPUT_UP:
        f_state.direction = DIR_UP;
        break;
    case H3MSNAKE_INPUT_RIGHT:
        f_state.direction = DIR_RIGHT;
        break;
    default:
        break;
    }

    //char dbg[256] = { 0 };
    //sprintf(dbg, "input: %d\n", input);
    //OutputDebugStringA(dbg);
    switch (f_state.direction)
    {
    case DIR_RIGHT:
        x_inc = 1;
        break;
    case DIR_UP:
        y_inc = -1;
        break;
    case DIR_LEFT:
        x_inc = -1;
        break;
    case DIR_DOWN:
        y_inc = 1;
        break;
    default:
        *((int *)(0xFBADC0DE)) = 0xDEADDEAD;
    }

    if (0 != _update_nodes(x_inc, y_inc))
    {
        int oa_index = 0;
        const char *def_gameover = "ava0128.def"; // Pandoras
        const char *def_score = f_state.def_apple;
        char score[12];
        int terrain = f_state.terrain;

        h3m_exit(&f_state.h3m);
        f_state.h3m = NULL;

        h3m_init_min(&f_state.h3m, H3M_FORMAT_ROE, 36);
        h3m_add_oa_by_def(f_state.h3m, def_gameover, &oa_index);
        h3m_alg_od_text(f_state.h3m, def_gameover, oa_index, 7, 1 + 6, 0, "GAME");
        h3m_alg_od_text(f_state.h3m, def_gameover, oa_index, 7, 1 + 6 + 6, 0, "OVER");
        
        snprintf(score, sizeof(score)-1, "%d", f_state.score);

        h3m_add_oa_by_name(f_state.h3m, f_state.def_apple, &oa_index);
        h3m_alg_od_text(f_state.h3m, f_state.def_apple, oa_index, 7, 1, 0, score);

        //h3m_object_add(f_state.h3m, "Angel", 5, 5, 0, NULL);

        uint8_t terrain_types[36 * 36];
        uint8_t river_types[36 * 36];
        uint8_t road_types[36 * 36];
        uint8_t impassable[36][36];

        memset(terrain_types, terrain, sizeof(terrain_types));
        memset(river_types, 0, sizeof(river_types));
        memset(road_types, 0, sizeof(road_types));
        memset(impassable, 1, sizeof(impassable));

        h3m_generate_tiles(f_state.h3m, 36, 0, (uint8_t *)terrain_types, (uint8_t *)road_types, (uint8_t *)river_types);

        for (int i = 0; i < 36; ++i)
        {
            for (int j = 6; j < 30; ++j)
                impassable[i][j] = 0;
        }

        h3m_impassable_fill(f_state.h3m, (uint8_t *)impassable, 36);

        state = H3MSNAKE_STATE_DEAD;
    }

    h3m_write(f_state.h3m, f_state.filename);

    return state;
}

