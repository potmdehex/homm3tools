#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "stb_image.h" // http://nothings.org/stb_image.c
#include "stb_image_write.h" // http://nothings.org/stb/stb_image_write.h
#include "stb_hbwang.h"

int main(int argc, char **argv)
{
    unsigned char *data;
    int xs, ys, w, h;
    stbhw_tileset ts;
    if (argc != 4) {
        fprintf(stderr, "Usage: mapgen {tile-file} {xsize} {ysize}\n"
            "generates file named 'test_map.png'\n");
        exit(1);
    }
    data = stbi_load(argv[1], &w, &h, NULL, 3);
    xs = atoi(argv[2]);
    ys = atoi(argv[3]);
    if (data == NULL) {
        fprintf(stderr, "Error opening or parsing '%s' as an image file\n", argv[1]);
        exit(1);
    }
    if (xs < 1 || xs > 1000) {
        fprintf(stderr, "xsize invalid or out of range\n");
        exit(1);
    }
    if (ys < 1 || ys > 1000) {
        fprintf(stderr, "ysize invalid or out of range\n");
        exit(1);
    }
    stbhw_build_tileset_from_image(&ts, data, w, w, h);
    free(data);
    // allocate a buffer to create the final image to
    data = malloc(xs * ys);
    //srand(time(NULL));
    stbhw_generate_image(&ts, NULL, data, xs, xs, ys);
    stbi_write_png("test_map.png", xs, ys, 3, data, xs * 3);
    //stbhw_free_tileset(&ts);
    free(data);
    return 0;
}