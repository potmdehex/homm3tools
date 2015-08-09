#if 0

// Windows DLLs
typedef DWORD (WINAPI *SleepEx_t)(DWORD dwMilliseconds, BOOL bAlertable);
typedef BOOL (WINAPI *QueryPerformanceCounter_t)(LARGE_INTEGER *lpPerformanceCount);
typedef HCURSOR(WINAPI *LoadCursorW_t)(HINSTANCE hInstance, LPCWSTR lpCursorName);

SleepEx_t orig_SleepEx = (SleepEx_t)NULL;
QueryPerformanceCounter_t orig_QueryPerformanceCounter = (QueryPerformanceCounter_t)NULL;
LoadCursorW_t orig_LoadCursorW = (LoadCursorW_t)NULL;

// HoMM 3 HD 2.0.exe
typedef void (__stdcall *cursor_changed_t)(int a1, int a2, int a3, int a4, int a5);
typedef void(__stdcall *save_game_t)(int a1);

cursor_changed_t orig_cursor_changed = (cursor_changed_t)NULL;
save_game_t orig_save_game = (save_game_t)NULL;

// Search pattern for cursor_changed:
//    push dword ptr [ebp+18h]
//    mov ecx, [eax + ecx * 4]
// Binary:
//    FF 75 18 8B 0C 88
orig_cursor_changed = (cursor_changed_t)((BYTE *)GetModuleHandleA(NULL) + 0xEA800);

// libegl.dll
typedef int(__stdcall *eglSwapInterval_t)(int a1, int a2);
typedef int(__stdcall *eglSwapBuffers_t)(int a1, int a2);

eglSwapInterval_t            orig_eglSwapInterval = (eglSwapInterval_t)NULL;
eglSwapBuffers_t orig_eglSwapBuffers = (eglSwapBuffers_t)NULL;

//SDL2.dll
typedef void * (__cdecl *SDL_CreateRGBSurfaceFrom_t)(void*  pixels,
    int    width,
    int    height,
    int    depth,
    int    pitch,
    uint32_t Rmask,
    uint32_t Gmask,
    uint32_t Bmask,
    uint32_t Amask);
typedef void * (__cdecl *SDL_CreateSystemCursor_t)(int id);
typedef uint32_t (__cdecl *SDL_Delay_t)(uint32_t delay);
typedef void (__cdecl *SDL_FreeSurface_t)(void * surface);
typedef uint32_t (__cdecl *SDL_GetTicks_t)(void);
typedef int (__cdecl *SDL_RenderCopy_t)(void *renderer, void *texture, void *srcrect,void *dstrect);
typedef int (__cdecl *SDL_RenderReadPixels_t)(void *renderer, void *rect, uint32_t format, void *pixels, int pitch);
typedef int (__cdecl *SDL_SaveBMP_t)(void *surface, const char *file);
typedef void (__cdecl *SDL_SetCursor_t)(void *cursor);
typedef int (__cdecl *SDL_ShowCursor_t)(int toggle);
typedef char * (__cdecl *SDL_GetError_t)(void);
typedef void * (__cdecl *SDL_CreateRGBSurface_t)(uint32_t flags, int width, int height, int depth, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
typedef void * (__cdecl *SDL_CreateSoftwareRenderer_t)(void * surface);
typedef int (__cdecl *SDL_FillRect_t)(void *dst, void *rect, uint32_t color);
typedef int (__cdecl *SDL_RenderClear_t)(void *renderer);
typedef void (__cdecl *SDL_RenderPresent_t)(void *renderer);
typedef int (__cdecl *SDL_SetRenderDrawColor_t)(void *renderer, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
typedef int (__cdecl *SDL_RenderFillRect_t)(void *renderer, void *rect);
typedef int (__cdecl *SDL_SetRenderDrawBlendMode_t)(void *renderer, int blendMode);

SDL_GetTicks_t               orig_SDL_GetTicks = (SDL_GetTicks_t)NULL;
SDL_Delay_t                  orig_SDL_Delay = (SDL_Delay_t)NULL;
SDL_ShowCursor_t             orig_SDL_ShowCursor = (SDL_ShowCursor_t)NULL;
SDL_CreateSystemCursor_t     orig_SDL_CreateSystemCursor = (SDL_CreateSystemCursor_t)NULL;
SDL_SetCursor_t              orig_SDL_SetCursor = (SDL_SetCursor_t)NULL;
SDL_RenderCopy_t             orig_SDL_RenderCopy = (SDL_RenderCopy_t)NULL;
SDL_RenderReadPixels_t       orig_SDL_RenderReadPixels = (SDL_RenderReadPixels_t)NULL;
SDL_SaveBMP_t                orig_SDL_SaveBMP = (SDL_SaveBMP_t)NULL;
SDL_FreeSurface_t            orig_SDL_FreeSurface = (SDL_FreeSurface_t)NULL;
SDL_CreateRGBSurfaceFrom_t   orig_SDL_CreateRGBSurfaceFrom = (SDL_CreateRGBSurfaceFrom_t)NULL;
SDL_GetError_t               orig_SDL_GetError = (SDL_GetError_t)NULL;
SDL_CreateRGBSurface_t       orig_SDL_CreateRGBSurface = (SDL_CreateRGBSurface_t)NULL;
SDL_CreateSoftwareRenderer_t orig_SDL_CreateSoftwareRenderer = (SDL_CreateSoftwareRenderer_t)NULL;
SDL_FillRect_t               orig_SDL_FillRect = (SDL_FillRect_t)NULL;
SDL_RenderClear_t            orig_SDL_RenderClear = (SDL_RenderClear_t)NULL;
SDL_RenderPresent_t          orig_SDL_RenderPresent = (SDL_RenderPresent_t)NULL;
SDL_SetRenderDrawColor_t     orig_SDL_SetRenderDrawColor = (SDL_SetRenderDrawColor_t)NULL;
SDL_RenderFillRect_t         orig_SDL_RenderFillRect = (SDL_RenderFillRect_t)NULL;
SDL_SetRenderDrawBlendMode_t orig_SDL_SetRenderDrawBlendMode = (SDL_SetRenderDrawBlendMode_t)NULL;

// Search pattern for save_game:
//     setnz al
//     mov [ebp - 0x220], 0x0F
// Binary:
//     0F 95 C0 C7 85 E0 FD FF FF 0F 00 00 00
orig_save_game = (void *)((BYTE *)GetModuleHandleA(NULL) + 0xC4860);


typedef void (__stdcall *get_hero_pos_t)(int a1, int a2, int a3);
get_hero_pos_t orig_get_hero_pos = (get_hero_pos_t)NULL;
void __declspec(naked) hooked_get_hero_pos(void)
{
    __asm PUSHAD
    if (f_do_revisit && !f_warp_flag)
    {
        --f_revisiting_hero->y;
    }
    __asm POPAD
    __asm JMP orig_get_hero_pos
}

#endif
