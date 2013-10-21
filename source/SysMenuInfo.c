/*-------------------------------------------------------------

 detect_settings.c -- detects various system settings

 Copyright (C) 2008 tona
 Unless other credit specified
 
 Changes by JoostinOnline

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
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <unistd.h>

#include "SysMenuInfo.h"

inline void *AllocateMemory(u32 size) {
	return memalign(32, round_up(size, 32));
}

char sanitizeRegion( char region ) {
	switch (region){
		case 'U':
		case 'E':
		case 'J':
		case 'K':
			return region;
			break;
		default:
			return 'X';
			break;
	}
}

int ctoi(char c) {
	switch(c){
		case '1': return 1;
		case '2': return 2;
		case '3': return 3;
		case '4': return 4;
		case '5': return 5;
		case '6': return 6;
		case '7': return 7;
		case '8': return 8;
		case '9': return 9;
		default:
		case '0': return 0;
	}
}

s32 __FileCmp(const void *a, const void *b)
{
	dirent_t *hdr1 = (dirent_t *)a;
	dirent_t *hdr2 = (dirent_t *)b;

	if (hdr1->type == hdr2->type)
		return strcmp(hdr1->name, hdr2->name);
	else
		return 0;
}

s32 getdir(char *path, dirent_t **ent, u32 *cnt)
{
	u32 num = 0;

	int i, j, k;

	if(ISFS_ReadDir(path, NULL, &num) != ISFS_OK) return -1;


	char *nbuf = (char *)AllocateMemory((ISFS_MAXPATH + 1) * num);
	char ebuf[ISFS_MAXPATH + 1];

	if(nbuf == NULL) return -2;

	if(ISFS_ReadDir(path, nbuf, &num) != ISFS_OK) return -3;

	*cnt = num;

	*ent = AllocateMemory(sizeof(dirent_t) * num);

	for(i = 0, k = 0; i < num; i++)
	{
		for(j = 0; nbuf[k] != 0; j++, k++)
			ebuf[j] = nbuf[k];
		ebuf[j] = 0;
		k++;

		strcpy((*ent)[i].name, ebuf);
	}

	qsort(*ent, *cnt, sizeof(dirent_t), __FileCmp);

	free(nbuf);
	return 0;
}

char getSystemMenuRegionFromContent() {
	s32 cfd;
	s32 ret;
	u32 num;
	dirent_t *list;
	char contentpath[ISFS_MAXPATH];
	char path[ISFS_MAXPATH];
	int i, j;
	u32 cnt = 0;
	u8 *buffer;
	u8 match[] = "C:\\Revolution", match2[] = "Final";
	char region = 'X';
	ISFS_Initialize();
	sprintf(contentpath, "/title/%08x/%08x/content", 1, 2);

	if (getdir(contentpath, &list, &num) < 0) return region;

	fstats filestats;
	for(cnt=0; region == 'X' && cnt < num; cnt++) {
		sprintf(path, "/title/%08x/%08x/content/%s", 1, 2, list[cnt].name);

		cfd = ISFS_Open(path, ISFS_OPEN_READ);
		if (cfd < 0) continue;

		if (ISFS_GetFileStats(cfd, &filestats) < 0) continue;
		
		buffer=(u8*)AllocateMemory(filestats.file_length);

		ret = ISFS_Read(cfd, buffer, filestats.file_length);
		ISFS_Close(cfd);
		if (ret < 0) continue;
		
		for(i = 0; i < filestats.file_length - 49; i += 1 ) {
			if(memcmp((buffer+i), match, 13) == 0) {
				for(j=0; j<30; j++){
					if( memcmp((buffer+i+j+24), match2, 5) == 0) {
						region = buffer[i+j+30];
						break;
					}
				}
				//if(sysVersion == 0.0) {
				//	int first = ctoi(buffer[i+24]);
				//	int second = ctoi(buffer[i+26]);
				//	sysVersion = first + (0.1 * second);
				//}
				break;
			}
		}
		free(buffer);
	}
	free(list);
	ISFS_Deinitialize();
	return sanitizeRegion(region);
}