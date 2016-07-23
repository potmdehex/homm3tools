#include <Windows.h>
#include <stdio.h>

#pragma warning(disable:4996)

char dbg[256];


enum ORIENTATION
{
    HORIZONTAL = 0,
    VERTICAL = 1
};
#if 0
static RECT *_object_rect_to_rect(RECT *obj_rect)
{
    RECT tmp;
    struct OBJECT_RECT *src = (struct OBJECT_RECT *)obj_rect;
    SetRect(&tmp, src->x, src->y, src->x + src->w, src->y + src->h);
    CopyRect(obj_rect, &tmp);
    return obj_rect;
}

int __declspec(noinline) _get_outermost_parts(int this_idx, int x, int y, int w, int h, enum ORIENTATION o, RECT parts[4])
{
    POINT pt;
    //RECT parts[4];
    RECT orect = { 0 };
    int x_diff = (o == HORIZONTAL) ? 32 : 0;
    int y_diff = (o == VERTICAL) ? 32 : 0;
    int count = 0;
    bool line_started = false;
    bool inside = false;

    memset(parts, 0, sizeof(parts));
    parts[count].left = x;
    parts[count].top = y;

    for (; x < x + w && y < y + h; x += x_diff, y += y_diff) {
        inside = false;

        for (int idx : g_selection) {
            if (idx == this_idx)
                continue;

            memset(&orect, 0, sizeof(orect));
            _get_object_rect(g_coord_class, (struct OBJECT_RECT *)&orect, (uint32_t)idx);
            _object_rect_to_rect(&orect);
            SetRect(&orect, (orect.left * 32), (orect.top * 32), (orect.right * 32) + 1, (orect.bottom * 32) + 1);

            pt = { x, y };
            if (PtInRect(&orect, pt)) {
                if (line_started != false) {
                    parts[count].right = x;
                    parts[count].bottom = y;
                    count++;
                    line_started = false;
                    inside = true;
                    break;
                }
            }
        }

        if (inside == false && line_started == false) {
            parts[count].left = x;
            parts[count].top = y;
        }
    }

    if (line_started != false) {
        parts[count].right = x;
        parts[count].bottom = y;
        count++;
    }

    return count;
}

#endif