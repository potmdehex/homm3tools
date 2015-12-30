// Created by John Åkerblom 2014-11-17
#include "../hlm.h"

#include <h3mlib.h>

#include <memory>
#include <cstdlib>
#include <time.h>

void _terrain_conv(const Hlm *hlm, h3mlib_ctx_t h3m)
{
    std::unique_ptr<uint8_t[]> terrain;
    std::unique_ptr<uint8_t[]> roads;
    std::unique_ptr<uint8_t[]> rivers;

    size_t size = hlm->m_size * hlm->m_size * (hlm->m_has_underground? 2 : 1);

    terrain = std::make_unique<uint8_t[]>(size);
    roads = std::make_unique<uint8_t[]>(size);
    rivers = std::make_unique<uint8_t[]>(size);

    for (size_t i = 0, x = 0, y = 0, z = 0; i < hlm->m_size * hlm->m_size; ++i) {
        terrain[i] = (uint8_t)hlm->m_terrain[z][x][y];
        roads[i] = (uint8_t)hlm->m_roads[z][x][y];
        rivers[i] = (uint8_t)hlm->m_rivers[z][x][y];

        if (++x >= hlm->m_size) {
            if (++y >= hlm->m_size) {
                y = 0;
                ++z;
            }
            x = 0;
        }
    }

    h3m_generate_tiles(h3m, hlm->m_size, 0, terrain.get(), roads.get(), rivers.get());
}

#if 0
    // Some thoughts for a sophisticated obstacle tool algorithm here: 
    //1. Decide on object group to use, e.g Mountain/Oak Trees/Pine Trees etc.
    //   Weighted towards choosing the exact same type as adjacent tiles 
    //   (e.g Oak Trees) first, same category secondarily (e.g Forest 
    //   including both Oak Trees, Pine Trees and any other trees) or thirdly a
    //   random object type. Some objects such as lakes are outliers and should
    //   not belong to an object group that can be selected by weight, i.e
    //   the only way for them to appear should be in the "thirdly" step above,
    //   randomly.
#endif

// This callback is for marking tiles where we have placed an obstacle that are
// impassable so that we know there is no gap there
int _pass_cb(int x, int y, int z, void *cb_data)
{
    auto a = (decltype(Hlm::m_obstacles) *)cb_data;

    if (x < 0 || y < 0) {
        return 0;
    }

    try {
        a->at(z).at(x).at(y) = true;
    }
    catch (std::out_of_range e) {
        // We will get here for objects that have some impassable tiles outside
        // map (e.g a 3x3 object with bottom right corner at 0, 0)
    }

    return 0;
}

struct _1x1ctx_t {
    h3mlib_ctx_t h3m;
    int od_index;
    int x;
    int y;
    int z;
};

// This callback is for moving 1x1 obstacles that do not have their 
// impassable tile in their bottom right corner
int _move_1x1_cb(int x, int y, int z, void *cb_data)
{
    struct _1x1ctx_t *_1x1ctx = (struct _1x1ctx_t *)cb_data;
    int x_diff = _1x1ctx->x - x; 
    int y_diff = _1x1ctx->y - y; 
    h3m_object_move(_1x1ctx->h3m, _1x1ctx->od_index, _1x1ctx->x + x_diff, _1x1ctx->y + y_diff, _1x1ctx->z);
    return 0;
}

struct Room {
    int left;
    int left_up;
    int left_down;
    int right;
    int right_up;
    int right_down;
};

// Retrieve how much room we have for placing an impassable obstacle at a given 
// coordinate. 
template<typename T>
static void _get_room(T& impassable, size_t size, int x_start, int y_start, int x_max, int y_max, int z, Room &room)
{
    int x = 0;
    int y = 0;
    int y_min = -1 * y_max;
    int x_min = -1 * x_max;

    memset(&room, 0, sizeof(room));

    // See how far LEFT we can go within max dimensions before hitting a tile that is not impassable. 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (x = -1; x > x_min && ((x_start + x) < 0 || impassable[z][x_start + x][y_start]); --x) {
    }
    room.left_up = (-1 != x) ? -1 : 0;
    room.left_down = (-1 != x) ? 1 : 0;
    room.left = (-1 != x) ? x : 0;

    // See for how far UP we can go, for each tile we already found out was free to the LEFT.
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (y = -1; y > y_min; --y) {
        for (x = 0; x > x_min && (((x_start + x) < 0 || (y_start + y) < 0) || impassable[z][x_start + x][y_start + y]); --x) {
        }
        if (x != room.left) {
            break;
        }
        room.left_up = y - 1;
    }

    // See for how far DOWN we can go, for each tile we already found out was free to the LEFT
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (y = 1; y <= y_max; ++y) {
        for (x = 0; x > x_min && (((x_start + x) < 0 || ((size_t)(y_start + y)) >= size) || impassable[z][x_start + x][y_start + y]); --x) {
        }
        if (x != room.left) {
            break;
        }
        room.left_down = y;
    }

    // See how far RIGHT we can go within max dimensions before hitting a tile that is not impassable. 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (x = 1; x < x_max && (((size_t)(x_start + x) >= size) || impassable[z][x_start + x][y_start]); ++x) {
    }
    room.right_up = (1 != x) ? -1 : 0;
    room.right_down = (1 != x) ? 1 : 0;
    room.right = x;

    // See for how far UP we can go, for each tile we already found out was free to the RIGHT 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (y = -1; y > y_min; --y) {
        for (x = 1; x < x_max && (((size_t)(x_start + x) >= size || (y_start + y) < 0) || impassable[z][x_start + x][y_start + y]); ++x) {
        }
        if (x != room.right) {
            break;
        }
        room.right_up = y - 1;
    }

    // See for how far DOWN we can go, for each tile we already found out was free to the RIGHT 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (y = 1; y <= y_max; ++y) {
        for (x = 1; x < x_max && (((size_t)(x_start + x) >= size || ((size_t)(y_start + y)) >= size) || impassable[z][x_start + x][y_start + y]); ++x) {
        }
        if (x != room.right) {
            break;
        }
        room.right_down = y;
    }

    // make the values that are always negative positive
    room.left *= -1;
    room.left_up *= -1;
    room.right_up *= -1;
}

// Some 1x1 do not have their impassable tile in the bottom right corner,
// so we have to move them
static int _ensure_1x1_correct(h3mlib_ctx_t h3m, int x, int y, int z, int od_index, uint64_t& passability) {
    struct _1x1ctx_t _1x1ctx = { h3m, od_index, x, y, z };
    return h3m_analyze_passability(passability, x, y, z, NULL, NULL, _move_1x1_cb, (void *)&_1x1ctx);
}

// Does the same thing as Obstacle Tool in map editor:
// Places random obstacles on tiles that have been marked, making them all
// impassable without leaving any gaps
void _obstacle_tool(const Hlm *hlm, h3mlib_ctx_t h3m)
{
    unsigned int x = 0;
    unsigned int y = 0;
    uint64_t passability = 0;
    int x_dim = 0;
    int x_max_dim = 0;
    int y_dim = 0;
    int y_max_dim = 0;
    int od_index = 0;
    struct Room room = { 0 };
    bool move_right = false;
    bool move_down = false;
    // Create zero-initialized copy of hlm->m_obstacles here,
    // in which we will set true for any tiles that become impassable after
    // we place objects
    decltype(Hlm::m_obstacles) placed{};
#if 1
    // TODO: current algorithm below does not place any objects with bottom
    // right corner outside of map, unlike the map editor Obstacle Tool. If the
    // bottom/right edge are marked impassable, big objects should be placed
    // outside just like with map editors Obstacle Tool

    // First pass: place random obstacles, possibly leaving gaps
    for (x = 0, y = 0; y < hlm->m_size; (++x >= hlm->m_size) ? x = 0, ++y : 1) {
        move_right = false;
        move_down = false;

        if (!hlm->m_obstacles[0][x][y]) {
            continue;
        }

        // 7x3 is biggest known obstacle, lakes. We use 5x3 as max dims
        _get_room(hlm->m_obstacles, hlm->m_size, x, y, 5, 3, 0, room);

        // If there is more room in other directions that up up and left (direction
        // in which all objects go from their bottom right corner), take max dims
        // from those directions and mark for moving in them
        if (room.right > room.left) {
            x_max_dim = room.right;
            move_right = true;
            // Set max y dimension, mark for move down if we are using down y dimension
            y_max_dim = (room.right_down > room.right_up)? move_down = true, room.right_down : room.right_up;
        } else {
            x_max_dim = room.left;
            // Set max y dimension, mark for move down if we are using down y dimension
            y_max_dim = (room.left_down > room.left_up)? move_down = true, room.left_down : room.left_up;
        }

        h3m_add_obstacle(h3m, x, y, 0, x_max_dim, y_max_dim, H3MLIB_TERRAIN_NATIVE, &x_dim, &y_dim, &od_index, &passability);

        // Move object if it was marked for move above. The reason we move it after adding
        // is that the actual dimensions, which are used as offset for the move, are picked at random by h3m_add_obstacle
        if (move_down || move_right) {
            // Move object, setting new x/y if moving right/down (x,y are still used to get squares that are now)
            // impassable below by h3m_analyze_passability)
            h3m_object_move(h3m, od_index, (move_right? x+= (x_dim - 1) : x), (move_down? y+= (y_dim - 1) : y), 0);
        }

        // Fix 1x1 objects not based in bottom right corner
        if (1 == x_dim && 1 == y_dim) {
            _ensure_1x1_correct(h3m, x, y, 0, od_index, passability);
        }

        // Analyze passability, marking any tiles that are now impassable in the placed arrray
        h3m_analyze_passability(passability, x, y, 0, NULL, NULL, _pass_cb, (void *)&placed);
    }
#endif

    // Second pass: fill any gaps with 1x1 obstacles
    for (x = 0, y = 0; y < hlm->m_size; (++x >= hlm->m_size)? x = 0, ++y : 1) {
        if (!hlm->m_obstacles[0][x][y] || placed[0][x][y]) {
            continue;
        }
        
        h3m_add_obstacle(h3m, x, y, 0, 1, 1, H3MLIB_TERRAIN_NATIVE, NULL, NULL, &od_index, &passability);
        // Fix 1x1 objects not based in bottom right corner
        _ensure_1x1_correct(h3m, x, y, 0, od_index, passability);
    }
}

void _object_conv(const Hlm *hlm, h3mlib_ctx_t h3m)
{
    _obstacle_tool(hlm, h3m);

    for (auto o : hlm->m_map_heroes) {
        const auto hero = o.get();
        int od_index = 0;

        h3m_object_add(h3m, 
            o.get()->m_name.c_str(), 
            o.get()->m_x,
            o.get()->m_y,
            o.get()->m_z,
            &od_index);

        h3m_object_set_owner(h3m, od_index, (int)hero->m_owner);
        h3m_player_enable(h3m, (int)hero->m_owner);
        if (hero->m_has_random_creatures) {
            h3m_object_fill_random_creatures(h3m, od_index);
        }
        else if (hero->m_has_creatures) {
            const char *names[] ={
                hero->m_creatures[0].name.c_str(),
                hero->m_creatures[1].name.c_str(),
                hero->m_creatures[2].name.c_str(),
                hero->m_creatures[3].name.c_str(),
                hero->m_creatures[4].name.c_str(),
                hero->m_creatures[5].name.c_str(),
                hero->m_creatures[6].name.c_str()
            };
            int quantities[] = {
                hero->m_creatures[0].quantity,
                hero->m_creatures[1].quantity,
                hero->m_creatures[2].quantity,
                hero->m_creatures[3].quantity,
                hero->m_creatures[4].quantity,
                hero->m_creatures[5].quantity,
                hero->m_creatures[6].quantity
            };
            h3m_object_set_creatues(h3m, od_index, names, quantities);
        }
    }

    for (auto o : hlm->m_map_monsters) {
        const auto monster = o.get();
        int od_index = 0;

        h3m_object_add(h3m,
            o.get()->m_name.c_str(),
            o.get()->m_x,
            o.get()->m_y,
            o.get()->m_z,
            &od_index);

        h3m_object_set_quantitiy(h3m, od_index, monster->m_quantity);
        h3m_object_set_disposition(h3m, od_index, (enum H3M_DISPOSITION)monster->m_disposition);
    }

    for (auto o : hlm->m_map_generic) {
        const auto object = o.get();

        h3m_object_add(h3m,
            o.get()->m_name.c_str(),
            o.get()->m_x,
            o.get()->m_y,
            o.get()->m_z,
            NULL);
    }
}

h3mlib_ctx_t HlmToH3M(const Hlm *hlm)
{
    h3mlib_ctx_t h3m = NULL;
    
    srand((unsigned int)time(NULL));

    h3m_init_min(&h3m, H3M_FORMAT_ROE, hlm->m_size);

    _terrain_conv(hlm, h3m);

    _object_conv(hlm, h3m);

    return h3m;
}
