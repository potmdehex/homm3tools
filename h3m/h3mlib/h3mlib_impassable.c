// Created by John Åkerblom 2015-01-24
// Ported (mostly copy paste) from hlm (by John Åkerblom) C++ code

#include "h3mlib.h"
#include "h3mlib_ctx.h"

struct PASS_CB_CTX {
    size_t map_size;
    uint8_t *array;
    uint8_t value;
};

// This callback is for marking tiles where we have placed an obstacle that are
// impassable so that we know there is no gap there
static int _pass_cb(int x, int y, int z, void *cb_data)
{
    struct PASS_CB_CTX *ctx = (struct PASS_CB_CTX *)cb_data;

    // TODO could check z here as well
    if (x < 0 || y < 0 || x >= (int)ctx->map_size || y >= (int)ctx->map_size) {
        return 0;
    }

    ctx->array[H3M_2D_TO_1D(ctx->map_size, x, y, z)] = ctx->value;

    return 0;
}

void h3m_impassable_oa_mark(h3mlib_ctx_t ctx, int x, int y, int z, int oa_index,
    uint8_t *impassable, size_t map_size)
{
    struct PASS_CB_CTX pass_ctx = { ctx->h3m.bi.any.map_size, impassable, 1 };
    uint64_t passability =
        *(uint64_t *)ctx->h3m.oa.entries[oa_index].body.passable 
        | *(uint64_t *)ctx->h3m.oa.entries[oa_index].body.active;
    h3m_analyze_passability(passability, x, y, z, NULL, NULL, _pass_cb,
        &pass_ctx);
}

void h3m_active_oa_mark(h3mlib_ctx_t ctx, int x, int y, int z, int oa_index,
    uint8_t *active, size_t map_size)
{
    struct PASS_CB_CTX pass_ctx = { ctx->h3m.bi.any.map_size, active, 1 };
    uint64_t passability =
        *(uint64_t *)ctx->h3m.oa.entries[oa_index].body.active;
    passability = ~passability;
    h3m_analyze_passability(passability, x, y, z, NULL, NULL, _pass_cb,
        &pass_ctx);
}

void h3m_impassable_mark(h3mlib_ctx_t ctx, int x, int y, int z,
    uint64_t passability, uint8_t *impassable, size_t map_size)
{
    struct PASS_CB_CTX pass_ctx = { ctx->h3m.bi.any.map_size, impassable, 1 };
    h3m_analyze_passability(passability, x, y, z, NULL, NULL, _pass_cb,
        &pass_ctx);
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
    h3m_object_move(_1x1ctx->h3m, _1x1ctx->od_index, _1x1ctx->x + x_diff,
        _1x1ctx->y + y_diff, _1x1ctx->z);
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
static void _get_room(uint8_t *impassable, size_t size, int x_start,
    int y_start, int x_max, int y_max, int z, struct Room *room)
{
    int x = 0;
    int y = 0;
    int y_min = -1 * y_max;
    int x_min = -1 * x_max;

    memset(room, 0, sizeof(*(room)));

    // See how far LEFT we can go within max dimensions before hitting a tile that is not impassable. 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (x = -1;
        x > x_min && ((x_start + x) < 0
            || impassable[H3M_2D_TO_1D(size, x_start + x, y_start, z)]); --x) {
    }
    room->left = (-1 != x) ? x : 0;
    room->left *= -1;

    if (room->left > 0) {
        room->left_up = (-1 != x) ? -1 : 0;
        room->left_down = (-1 != x) ? 1 : 0;

        // See for how far UP we can go, for each tile we already found out was free to the LEFT.
        // If coord is outside of map edge, consider it okay for impassable without looking at the array.
        for (y = -1; y > y_min; --y) {
            int y_cur = y_start + y;
            for (x = 0;
                x > x_min && (((x_start + x) < 0 || y_cur < 0)
                    || impassable[H3M_2D_TO_1D(size, x_start + x, y_cur, z)]);
                --x) {
            }
            if (x * -1 != room->left) {
                break;
            }
            room->left_up = y - 1;
        }

        // See for how far DOWN we can go, for each tile we already found out was free to the LEFT
        // If coord is outside of map edge, consider it okay for impassable without looking at the array.
        for (y = 1; y <= y_max; ++y) {
            for (x = 0;
                x > x_min && (((x_start + x) < 0
                        || ((size_t) (y_start + y)) >= size)
                    || impassable[H3M_2D_TO_1D(size, x_start + x, y_start + y,
                            z)]); --x) {
            }
            if (x * -1 != room->left) {
                break;
            }
            room->left_down = y;
        }
    }

    room->left_up *= -1;

    // See how far RIGHT we can go within max dimensions before hitting a tile that is not impassable. 
    // If coord is outside of map edge, consider it okay for impassable without looking at the array.
    for (x = 1;
        x < x_max && (((size_t) (x_start + x) >= size)
            || impassable[H3M_2D_TO_1D(size, x_start + x, y_start, z)]); ++x) {
    }
    room->right = x;

    if (room->right > 0) {
        room->right_up = (1 != x) ? -1 : 0;
        room->right_down = (1 != x) ? 1 : 0;

        // See for how far UP we can go, for each tile we already found out was free to the RIGHT 
        // If coord is outside of map edge, consider it okay for impassable without looking at the array.
        for (y = -1; y > y_min; --y) {
            int y_cur = y_start + y;
            for (x = 0;
                x < x_max && (((size_t) (x_start + x) >= size || y_cur < 0)
                    || impassable[H3M_2D_TO_1D(size, x_start + x, y_cur, z)]);
                ++x) {
            }
#if 0

            for (x = 0;
                x < x_max && ((((size_t) (x_start + x) >= size)
                        || (y_start + y) < 0)
                    || impassable[H3M_2D_TO_1D(size, x_start + x, y_start + y,
                            z)]); ++x) {
            }
#endif
            if (x != room->right) {
                break;
            }
            room->right_up = y - 1;
        }

        // See for how far DOWN we can go, for each tile we already found out was free to the RIGHT 
        // If coord is outside of map edge, consider it okay for impassable without looking at the array.
        for (y = 1; y <= y_max; ++y) {
            for (x = 0;
                x < x_max && ((((size_t) (x_start + x) >= size)
                        || ((size_t) (y_start + y)) >= size)
                    || impassable[H3M_2D_TO_1D(size, x_start + x, y_start + y,
                            z)]); ++x) {
            }
            if (x != room->right) {
                break;
            }
            room->right_down = y;
        }
    }

    room->right_up *= -1;
}

// Some 1x1 do not have their impassable tile in the bottom right corner,
// so we have to move them
static int _ensure_1x1_correct(h3mlib_ctx_t h3m, int x, int y, int z,
    int od_index, uint64_t passability)
{
    struct _1x1ctx_t _1x1ctx = { h3m, od_index, x, y, z };
    return h3m_analyze_passability(passability, x, y, z, NULL, NULL,
        _move_1x1_cb, (void *)&_1x1ctx);
}

// Does the same thing as Obstacle Tool in map editor:
// Places random obstacles on tiles that have been marked, making them all
// impassable without leaving any gaps
void h3m_impassable_fill(h3mlib_ctx_t ctx, uint8_t *impassable,
    size_t map_size)
{
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int z = 0;
    unsigned int z_max = 1;     //1 + h3m->h3m.bi.any.has_two_levels;
    uint64_t passability = 0;
    int x_dim = 0;
    int x_max_dim = 0;
    int y_dim = 0;
    int y_max_dim = 0;
    int od_index = 0;
    struct Room room = { 0 };
    int move_right = 0;
    int move_down = 0;
    int placed[H3M_MAX_SIZE * H3M_MAX_SIZE * 2] = { 0 };

#if 1
    // TODO: current algorithm below does not place any objects with bottom
    // right corner outside of map, unlike the map editor Obstacle Tool. If the
    // bottom/right edge are marked impassable, big objects should be placed
    // outside just like with map editors Obstacle Tool

    // First pass: place random obstacles, possibly leaving gaps

    for (z = 0; z < z_max; ++z) {
        for (x = 0, y = 0; y < map_size; (++x >= map_size) ? x = 0, ++y : 1) {
            move_right = 0;
            move_down = 0;

            if (!impassable[H3M_2D_TO_1D(map_size, x, y, z)]) {
                continue;
            }
            // 7x3 is biggest known obstacle, lakes. We use 5x3 as max dims
            _get_room(impassable, map_size, x, y, 5, 3, z, &room);

            // If there is more room in other directions that up up and left (direction
            // in which all objects go from their bottom right corner), take max dims
            // from those directions and mark for moving in them
            if (room.right > room.left) {
                x_max_dim = room.right;
                move_right = 1;
                // Set max y dimension, mark for move down if we are using down y dimension
                y_max_dim = (room.right_down > room.right_up) ? move_down =
                    1, room.right_down : room.right_up;
            } else {
                x_max_dim = room.left;
                // Set max y dimension, mark for move down if we are using down y dimension
                y_max_dim = (room.left_down > room.left_up) ? move_down =
                    1, room.left_down : room.left_up;
            }

            if (0 == x_max_dim || 0 == y_max_dim) {
                continue;
            }

            h3m_add_obstacle(ctx, x, y, z, x_max_dim, y_max_dim,
                H3MLIB_TERRAIN_NATIVE, &x_dim, &y_dim, &od_index, &passability);

            // Move object if it was marked for move above. The reason we move it after adding
            // is that the actual dimensions, which are used as offset for the move, are picked at random by h3m_add_obstacle
            if (move_down || move_right) {
                // Move object, setting new x/y if moving right/down (x,y are still used to get squares that are now)
                // impassable below by h3m_analyze_passability)
                h3m_object_move(ctx, od_index, (move_right ? x +=
                        (x_dim - 1) : x), (move_down ? y +=
                        (y_dim - 1) : y), 0);
            }
            // Fix 1x1 objects not based in bottom right corner
            if (1 == x_dim && 1 == y_dim) {
                _ensure_1x1_correct(ctx, x, y, z, od_index, passability);
            }
            // Analyze passability, marking any tiles that are now impassable in the placed arrray
            h3m_analyze_passability(passability, x, y, z, NULL, NULL, _pass_cb,
                (void *)placed);
        }
    }
#endif

    // Second pass: fill any gaps with 1x1 obstacles
    for (z = 0; z < z_max; ++z) {
        for (x = 0, y = 0; y < map_size; (++x >= map_size) ? x = 0, ++y : 1) {
            if (!impassable[H3M_2D_TO_1D(map_size, x, y, z)]
                || placed[H3M_2D_TO_1D(map_size, x, y, z)]) {
                continue;
            }

            h3m_add_obstacle(ctx, x, y, z, 1, 1, H3MLIB_TERRAIN_NATIVE, NULL,
                NULL, &od_index, &passability);
            // Fix 1x1 objects not based in bottom right corner
            _ensure_1x1_correct(ctx, x, y, z, od_index, passability);
        }
    }
}
