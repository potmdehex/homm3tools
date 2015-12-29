// Created by John Åkerblom 2015-08-08

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <h3mlib.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // snprintf unsafe
    #define snprintf _snprintf
#endif

#define H3COMPLETE_SUFFIX "[h3complete].h3m"
#define HDMOD_SUFFIX "[hdmod].h3m"

void _print_usage(void)
{
    puts("h3mdllembed <input .h3m map> <input .dll>");
    puts("Example:");
    puts("\th3mdllembed Arrogance.h3m Tower_Defense.dll");
}

int main(int argc, char **argv)
{
    char *output_file = NULL;
    size_t len = 0;
    h3mlib_ctx_t h3m = NULL;

    if (argc != 3) {
        _print_usage();
        return 1;
    }

    if (0 != h3m_read(&h3m, argv[1])) {
        printf("[!] Failed to open %s!", argv[1]);
        return 1;
    }
    
    if (0 != h3m_modembed_set_dll(h3m, argv[2])) {
        printf("[!] Failed to embed DLL %s!", argv[2]);
        h3m_exit(&h3m);
        return 1;
    }

    len = strlen(argv[1]) + sizeof(H3COMPLETE_SUFFIX);
    output_file = calloc(len, 1);
    if (output_file == NULL) {
        puts("[!] Failed to allocate memory!");
        h3m_exit(&h3m);
        return 1;
    }

    // H3Complete
    snprintf(output_file, len - 1, "%s%s", argv[1], H3COMPLETE_SUFFIX);
    h3m_modembed_set_target(h3m, H3M_MODEMBED_TARGET_COMPLETE);
    if (0 != h3m_write(h3m, output_file)) {
        printf("[!] Failed to write %s!", output_file);
        free(output_file);
        h3m_exit(&h3m);
        return 1;
    }
    printf("[+] Embedded %s into %s\n", argv[2], output_file);

    // HD Mod
    // Note same len is used here
    snprintf(output_file, len - 1, "%s%s", argv[1], HDMOD_SUFFIX);
    h3m_modembed_set_target(h3m, H3M_MODEMBED_TARGET_HDMOD);
    if (0 != h3m_write(h3m, output_file)) {
        printf("[!] Failed to write %s!", output_file);
        free(output_file);
        h3m_exit(&h3m);
        return 1;
    }
    printf("[+] Embedded %s into %s\n", argv[2], output_file);

    free(output_file);
    h3m_exit(&h3m);

    return 0;
}

