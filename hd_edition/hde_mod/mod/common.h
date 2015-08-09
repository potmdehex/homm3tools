// Created by John Åkerblom 2015-04-20, from hooked.c from 2015-01-29

#ifndef __COMMON_H_DEF__
#define __COMMON_H_DEF__

#include <Windows.h>

#define WINDOW_TITLE "Heroes of Might & Magic III - HD Edition"

// This function makes a special stack move for move operations that are trying 
// to move the last stack in a hero's army to another hero, moving every creature
// in that stack over except 1 (normally the move is simply cancelled).
//
// stdcall to make it easier to call this function from assembly
#define ERROR_SAME_HERO 1
#define ERROR_NOT_LAST_STACK 2
#define ERROR_SINGLE_CREATURE 3
#define ERROR_DEST_SLOT_DIFFERENT 4
int __stdcall common_move_last_stack(struct HDE_HERO *src, struct HDE_HERO *dest, int src_slot, int dest_slot);

void common_init(void);

#endif
