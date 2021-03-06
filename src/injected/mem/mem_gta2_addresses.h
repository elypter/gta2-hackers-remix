#pragma once
#include <wchar.h>

/*
        Before reverse engineering new addresses,
        check this thread first:
        http://gtamp.com/forum/viewtopic.php?t=700

        All addresses in this file were found by
        robotanarchy, except for the following.

        Credit to Sektor (gtamp.com):
                GTA2_MPADDR_KILLS
                GTA2_MPADDR_POINTS
*/

char *EXE_OFFSET;
char MP_PLAYER_ID; // 0...5

//
// Addresses that work in both single- (SP) and multiplayer (MP)
//

#define GTA2_ADDR_PLAYER_IN_VEHICLE (char *)0x5e20bc

// See also: native/cmap/cmap_gta2_controls.h
#define GTA2_ADDR_MOVEMENT (uint16_t *)0x5ecacc

// this is just for checking if the pointer is valid yet
#define GTA2_ADDR_STRING_TABLE_BASE (EXE_OFFSET + 0x001E81DC)

// table entry format (similar to in the GXT files):
//		4 bytes: absolute (!) pointer to the unicode text in RAM
// 		8 bytes: entry name
#define GTA2_ADDR_STRING_TABLE                                                 \
  (*((char **)GTA2_ADDR_STRING_TABLE_BASE) + 0x1C98)
#define GTA2_ADDR_STRING_TABLE_ALT                                             \
  (*((char **)(EXE_OFFSET + 0x001E85B0)) + 0x1C98)
#define GTA2_ADDR_STRING_OFFSET (***((char ****)(EXE_OFFSET + 0x00281550)))

// http://git.io/g2hr-rumble-byte
#define GTA2_ADDR_RUMBLE (char *)0x665770

//
// Different for SP and MP
//

// NOTE: this one only works for the first game started.
// (but G2HR implements its own menu, so that's not an issue :))
#define GTA2_ADDRSP_SCORE (int *)(*((char **)(EXE_OFFSET + 0x1E8DC0)) + 0x188)

#define GTA2_ADDRMP_SCORE (int *)((char *)0x5EC50C + MP_PLAYER_ID * 0x04);
#define GTA2_ADDRMP_KILLS (char *)0x5EC4BA + MP_PLAYER_ID * 0x0A;
#define GTA2_ADDRMP_POINTS (char *)0x5EC500 + MP_PLAYER_ID * 0x02;

/*
        GTA2 Text adresses all have the folowing layout:
                TIMER 0x00 TEXT_AS_UNICODE
        Example for TIMER=90 and the string HELLO:
                63 00 48 00 65 00 6C 00 6C 00 6F 00
        The timer automatatically runs down from the specified value.

        We'll need to find real pointers, these don't work.
*/
// #define GTA2_ADDR_TEXT_BASE (*(void**)0x00282F40)
// #define GTA2_OFFSET_TEXT_ITEM 0x27b8
