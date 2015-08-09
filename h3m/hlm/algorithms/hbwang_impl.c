// Created by John Åkerblom 2014-01-08

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ standard lib unsafe
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h> // http://nothings.org/stb_image.c
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> // http://nothings.org/stb/stb_image_write.h
#define STB_HERRINGBONE_WANG_TILE_IMPLEMENTATION
#include <stb_hbwang.h>


void hbwang_gen_from_file(const char *image_file, int size, unsigned char *output, int output_size)
{
    unsigned char *data;
    int ts_w, ts_h;
    stbhw_tileset ts;

    data = stbi_load(image_file, &ts_w, &ts_h, NULL, 3);

    stbhw_build_tileset_from_image(&ts, data, ts_w * 3, ts_w, ts_h);
    free(data);
    data = malloc(3 * size * size);

    stbhw_generate_image(&ts, NULL, data, size * 3, size, size);
    stbi_write_png("test_map.png", size, size, 3, data, size * 3);

    for (int x = 0, y = 0; y < size; (++x >= size) ? x = 0, ++y : 1) {
        int idx = (y + (x * output_size));
        output[idx] = !data[3 * x + (y * size * 3)];
    }
    free(data);
    stbhw_free_tileset(&ts);
}
