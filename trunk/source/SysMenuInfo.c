/*-------------------------------------------------------------

 detect_settings.c -- detects various system settings

 Copyright (C) 2008 tona
 Copyright (C) 2014 JoostinOnline
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

float GetSysMenuNintendoVersion(u32 sysVersion)
{
	float ninVersion = 0.0;

	switch (sysVersion)
	{
		case 33:
			ninVersion = 1.0f;
			break;

		case 97:
		case 128:
		case 130:
			ninVersion = 2.0f;
			break;

		case 162:
			ninVersion = 2.1f;
			break;

		case 192:
		case 193:
		case 194:
			ninVersion = 2.2f;
			break;

		case 224:
		case 225:
		case 226:
			ninVersion = 3.0f;
			break;

		case 256:
		case 257:
		case 258:
			ninVersion = 3.1f;
			break;

		case 288:
		case 289:
		case 290:
			ninVersion = 3.2f;
			break;

		case 352:
		case 353:
		case 354:
		case 326:
			ninVersion = 3.3f;
			break;

		case 384:
		case 385:
		case 386:
			ninVersion = 3.4f;
			break;

		case 390:
			ninVersion = 3.5f;
			break;

		case 416:
		case 417:
		case 418:
			ninVersion = 4.0f;
			break;

		case 448:
		case 449:
		case 450:
		case 454:
		case 54448: // mauifrog's custom version
		case 54449: // mauifrog's custom version
		case 54450: // mauifrog's custom version
		case 54454: // mauifrog's custom version
			ninVersion = 4.1f;
			break;

		case 480:
		case 481:
		case 482:
		case 486:
			ninVersion = 4.2f;
			break;

		case 512:
		case 513:
		case 514:
		case 518:
		case 544:
		case 545:
		case 546:
			ninVersion = 4.3f;
			break;
	}

	return ninVersion;
}

char GetSysMenuRegion(u32 sysVersion) {
	char SysMenuRegion;
	switch(sysVersion)
	{
		case 1:  //Pre-launch
		case 97: //2.0U
		case 193: //2.2U
		case 225: //3.0U
		case 257: //3.1U
		case 289: //3.2U
		case 353: //3.3U
		case 385: //3.4U
		case 417: //4.0U
		case 449: //4.1U
		case 54449: // mauifrog 4.1U
		case 481: //4.2U
		case 513: //4.3U
		case 545:
			SysMenuRegion = 'U';
			break;
		case 130: //2.0E
		case 162: //2.1E
		case 194: //2.2E
		case 226: //3.0E
		case 258: //3.1E
		case 290: //3.2E
		case 354: //3.3E
		case 386: //3.4E
		case 418: //4.0E
		case 450: //4.1E
		case 54450: // mauifrog 4.1E
		case 482: //4.2E
		case 514: //4.3E
		case 546:
			SysMenuRegion = 'E';
			break;
		case 128: //2.0J
		case 192: //2.2J
		case 224: //3.0J
		case 256: //3.1J
		case 288: //3.2J
		case 352: //3.3J
		case 384: //3.4J
		case 416: //4.0J
		case 448: //4.1J
		case 54448: // mauifrog 4.1J
		case 480: //4.2J
		case 512: //4.3J
		case 544:
			SysMenuRegion = 'J';
			break;
		case 326: //3.3K
		case 390: //3.5K
		case 454: //4.1K
		case 54454: // mauifrog 4.1K
		case 486: //4.2K
		case 518: //4.3K
			SysMenuRegion = 'K';
			break;
		default:
			SysMenuRegion = getSystemMenuRegionFromContent();
			break;
	}
	return SysMenuRegion;
}

// Get the system menu version from TMD
u32 GetSysMenuVersion(void)
{
	static u64 TitleID ATTRIBUTE_ALIGN(32) = 0x0000000100000002LL;
	static u32 tmdSize ATTRIBUTE_ALIGN(32);

	// Get the stored TMD size for the system menu
	if (ES_GetStoredTMDSize(TitleID, &tmdSize) < 0) return false;

	signed_blob *TMD = (signed_blob *)memalign(32, (tmdSize+32)&(~31));
	memset(TMD, 0, tmdSize);

	// Get the stored TMD for the system menu
	if (ES_GetStoredTMD(TitleID, TMD, tmdSize) < 0) return false;

	// Get the system menu version from TMD
	tmd *rTMD = (tmd *)(TMD+(0x140/sizeof(tmd *)));
	u32 version = rTMD->title_version;

	free(TMD);

	// Return the system menu version
	return version;
}