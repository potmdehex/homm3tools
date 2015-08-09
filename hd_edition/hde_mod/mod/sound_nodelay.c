// Created by John Åkerblom 2015-04-21, from hooked.c from 2015-01-29

#include "common.h"
#include "sound_nodelay.h"

#include <hook_utils.h>

#include <string.h>

// SDL2_mixer.dll
typedef int (__cdecl *Mix_PlayChannelTimed_t)(int channel, void *chunk, int loops, int ticks);
typedef int (__cdecl *Mix_AllocateChannels_t)(int numchans);
typedef int (__cdecl *Mix_Playing_t)(int channel);
typedef int (__cdecl *Mix_HaltChannel_t)(int channel);
typedef int (__cdecl *Mix_FadeInMusic_t)(void *music, int loops, int ms);
typedef void *(__cdecl *Mix_LoadMUS_t)(const char *file);

Mix_PlayChannelTimed_t orig_Mix_PlayChannelTimed = (Mix_PlayChannelTimed_t)NULL;
Mix_AllocateChannels_t orig_Mix_AllocateChannels = (Mix_AllocateChannels_t)NULL;
Mix_Playing_t orig_Mix_Playing = (Mix_Playing_t)NULL;
Mix_HaltChannel_t orig_Mix_HaltChannel = (Mix_HaltChannel_t)NULL;
Mix_FadeInMusic_t orig_Mix_FadeInMusic = (Mix_FadeInMusic_t)NULL;
Mix_LoadMUS_t orig_Mix_LoadMUS = (Mix_LoadMUS_t)NULL;

#define MUSIC_CHANNEL 2

static void *f_music_combat;

struct Mix_PlayChannelTimed_ctx
{
    int channel;
    void *chunk;
    int loops;
    int ticks;
};

struct Mix_FadeInMusic_ctx
{
    void *music;
    int loops;
    int ms;
};

// TODO: could have a persistent thread here that receives events
// for sounds to play
static DWORD WINAPI _MixPlayChannelTimed_thread(LPVOID lpParameter)
{
    struct Mix_PlayChannelTimed_ctx *ctx = (struct Mix_PlayChannelTimed_ctx *)lpParameter;
    int channel;

    for (channel = 16; channel < 100; ++channel)
    {
        if (0 == orig_Mix_Playing(channel))
        {
            break;
        }
    }

    orig_Mix_PlayChannelTimed(channel, ctx->chunk, ctx->loops, ctx->ticks);

    free(ctx);

    ExitThread(0);
}

// TODO: could have a persistent thread here that receives events
// for sounds to play
static DWORD WINAPI _MixFadeInMusic_thread(LPVOID lpParameter)
{
    struct Mix_FadeInMusic_ctx *ctx = (struct Mix_FadeInMusic_ctx *)lpParameter;

    if (ctx->music == f_music_combat)
    {
        while (orig_Mix_Playing(MUSIC_CHANNEL) || orig_Mix_Playing(MUSIC_CHANNEL + 16))
            Sleep(10);
    }
       

    orig_Mix_FadeInMusic(ctx->music, ctx->loops, ctx->ms);

    free(ctx);

    ExitThread(0);
}

void hooked_Mix_PlayChannelTimed(int channel, void *chunk, int loops, int ticks)
{
    if (0 == loops && channel != 2)
    {
        struct Mix_PlayChannelTimed_ctx *ctx = calloc(1, sizeof(*ctx));

        ctx->channel = channel;
        ctx->chunk = chunk;
        ctx->loops = loops;
        ctx->ticks = ticks;

        CloseHandle(CreateThread(NULL, 0, _MixPlayChannelTimed_thread, (LPVOID)ctx, 0, NULL));
    }
    else
    {
        orig_Mix_PlayChannelTimed(channel, chunk, loops, ticks);
    }
}

static int hooked_Mix_Playing(int channel)
{
    if (MUSIC_CHANNEL == channel)
    {
        return 0;
    }

    return orig_Mix_Playing(channel);
}

static int hooked_Mix_HaltChannel(int channel)
{
    if (MUSIC_CHANNEL == channel)
    {
        return 0;
    }

    return orig_Mix_HaltChannel(channel);
}

static int hooked_Mix_FadeInMusic(void *music, int loops, int ms)
{
    struct Mix_FadeInMusic_ctx *ctx = calloc(1, sizeof(*ctx));
    ctx->music = music;
    ctx->loops = loops;
    ctx->ms = ms;

    CloseHandle(CreateThread(NULL, 0, _MixFadeInMusic_thread, ctx, 0, NULL));

    return 0;
}

void *hooked_Mix_LoadMUS(const char *file)
{
    if (NULL != strstr(file, "/Combat01.")
        || NULL != strstr(file, "/Combat02.")
        || NULL != strstr(file, "/Combat03.")
        || NULL != strstr(file, "/Combat04."))
    {
        f_music_combat = orig_Mix_LoadMUS(file);
        return f_music_combat;
    }

    return orig_Mix_LoadMUS(file);
}

void sound_nodelay_init(void)
{
    hook_trampoline_dis_x86_by_name("SDL2_mixer.dll", "Mix_PlayChannelTimed", &hooked_Mix_PlayChannelTimed, (void**)&orig_Mix_PlayChannelTimed);
    hook_trampoline_dis_x86_by_name("SDL2_mixer.dll", "Mix_Playing", &hooked_Mix_Playing, (void**)&orig_Mix_Playing);
    hook_trampoline_dis_x86_by_name("SDL2_mixer.dll", "Mix_HaltChannel", &hooked_Mix_HaltChannel, (void**)&orig_Mix_HaltChannel);
    hook_trampoline_dis_x86_by_name("SDL2_mixer.dll", "Mix_FadeInMusic", &hooked_Mix_FadeInMusic, (void**)&orig_Mix_FadeInMusic);
    hook_trampoline_dis_x86_by_name("SDL2_mixer.dll", "Mix_LoadMUS", &hooked_Mix_LoadMUS, (void**)&orig_Mix_LoadMUS);

    orig_Mix_AllocateChannels = (void *)GetProcAddress(GetModuleHandleA("SDL2_mixer.dll"), "Mix_AllocateChannels");

    // Need them extra channels
    orig_Mix_AllocateChannels(100);
}
