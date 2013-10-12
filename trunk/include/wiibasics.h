/*-------------------------------------------------------------
 
wiibasics.h -- basic Wii initialization and functions
 
Copyright (C) 2008 tona
Unless other credit specified
 
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.
 
Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:
 
1.The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.
 
2.Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.
 
3.This notice may not be removed or altered from any source
distribution.
 
-------------------------------------------------------------*/

#ifndef _WII_BASICS_H_
#define _WII_BASICS_H_


// Turn upper and lower into a full title ID
#define TITLE_ID(x,y)		(((u64)(x) << 32) | (y))
// Get upper or lower half of a title ID
#define TITLE_UPPER(x)		((u32)((x) >> 32))
// Turn upper and lower into a full title ID
#define TITLE_LOWER(x)		((u32)(x))

// be functions from segher's wii.git
u16 be16(const u8 *p);
u32 be32(const u8 *p);
u64 be64(const u8 *p);
u64 be34(const u8 *p);

// Do basic Wii init: Video, console, WPAD
void basicInit(void);

// Do our custom init: Identify and initialized ISFS driver
void miscInit(void);

void IdentSysMenu(void);

// Clean up after ourselves (Deinit ISFS)
void miscDeInit(void);

// Scan the pads and return buttons
u32 getButtons(void);

u32 wait_anyKey(void);

u32 wait_key(u32 button);

void hex_print_array16(const u8 *array, u32 size);

/* Reads a file from ISFS to an array in memory */
s32 ISFS_ReadFileToArray (const char *filepath, u8 *filearray, u32 max_size, u32 *file_size);

/* Writes from an array in memory to a file with ISFS */
s32 ISFS_WriteFileFromArray (const char *filepath, const u8 *filearray, u32 array_size, u32 ownerID, u16 groupID, u8 attr, u8 own_perm, u8 group_perm, u8 other_perm);

bool yes_or_no(void);

#endif
