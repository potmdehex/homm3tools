#ifndef __SETTINGS_UTILS_H_DEF__
#define __SETTINGS_UTILS_H_DEF__

#ifdef __cplusplus
extern "C"
{
#endif

#define SETTINGS_INT 0
#define SETTINGS_ASCII 1
//#define SETTINGS_UNICODE 2
//#define SETTINGS_BINARYDATA 3

int settings_get_int(const char *path, const char *name, int *value);
int settings_set_int(const char *path, const char *name, int value);

int settings_get_ascii(const char *path, const char *name, char *value, 
    int value_size);
int settings_set_ascii(const char *path, const char *name, const char *value);

int settings_get_n(const char *path, int n, const char **names, int *types,
    void **values, int *value_sizes);

int settings_set_n(const char *path, int n, const char **names, int *types,
    void **values, int *value_sizes);

#ifdef __cplusplus
}
#endif

#endif


