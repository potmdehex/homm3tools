#include "settings_utils.h"

#ifdef _WIN32
    #include <Windows.h>
#endif

static DWORD _to_reg_type(int type)
{
    switch (type)
    {
    case SETTINGS_INT:
        return REG_DWORD;
    case SETTINGS_ASCII:
        return REG_SZ;
    default:
        break;
    }

    return -1;
}

static int _from_reg_type(DWORD reg_type)
{
    switch (reg_type)
    {
    case REG_DWORD:
        return SETTINGS_INT;
    case REG_SZ:
        return SETTINGS_ASCII;
    default:
        break;
    }

    return -1;
}


int settings_get_n(const char *path, int n, const char **names, int *types,
    void **values, int *value_sizes)
{
    HKEY key = NULL;
    DWORD reg_type = 0;
    DWORD size = 0;
    int i = 0;

    if (ERROR_SUCCESS != RegCreateKeyA(HKEY_CURRENT_USER, path, &key))
    {
        return 1;
    }

    for (i = 0; i < n; ++i)
    {
        reg_type = _to_reg_type(types[i]);
        size = (SETTINGS_INT == types[i]) ? sizeof(int) : value_sizes[i];
        RegQueryValueExA(key, names[i], NULL, &reg_type, (BYTE *)&values[i],
            &size);
    }

    RegCloseKey(key);

    return 0;
}

int settings_set_n(const char *path, int n, const char **names, int *types,
    void **values, int *value_sizes)
{
    HKEY key = NULL;
    DWORD reg_type = 0;
    DWORD size = 0;
    const BYTE *data;
    int i = 0;

    if (ERROR_SUCCESS != RegCreateKeyA(HKEY_CURRENT_USER, path, &key))
    {
        return 1;
    }

    for (i = 0; i < n; ++i)
    {
        reg_type = _to_reg_type(types[i]);
        data = (SETTINGS_INT == types[i])
            ? (const BYTE *)&values[i] : (const BYTE *)values[i];
        size = (SETTINGS_INT == types[i])
            ? sizeof(DWORD) : value_sizes[i];
        RegSetValueExA(key, names[i], 0, reg_type, data, size);
    }

    RegCloseKey(key);

    return 0;
}

int settings_get_int(const char *path, const char *name, int *value)
{
    int types = SETTINGS_INT;
    return settings_get_n(path, 1, &name, &types, (void **)value, NULL);
}

int settings_set_int(const char *path, const char *name, int value)
{
    int types = SETTINGS_INT;
    return settings_set_n(path, 1, &name, &types, (void **)&value, NULL);
}

int settings_get_ascii(const char *path, const char *name, char *value, int value_size)
{
    int types = SETTINGS_ASCII;
    return settings_get_n(path, 1, &name, &types, (void **)value, &value_size);
}

int settings_set_ascii(const char *path, const char *name, const char *value)
{
    int types = SETTINGS_ASCII;
    int sizes = (int)strlen(value);
    return settings_set_n(path, 1, &name, &types, (void **)&value, &sizes);
}
