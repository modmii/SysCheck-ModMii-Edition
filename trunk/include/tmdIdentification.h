#ifndef __TMDIDENT_H__
#define __TMDIDENT_H__

#include <gctypes.h>

#define base_number 125

typedef struct {
	u32 hashes[5];
	u32 base;
	char info[0x10]; 
} iosHashes;
	
typedef struct _iosinfo_t {
	u32 magicword; //0x1ee7c105
	u32 magicversion; // 1
	u32 version; // Example: 5
	u32 baseios; // Example: 56
	char name[0x10]; // Example: d2x
	char versionstring[0x10]; // Example: beta2
} __attribute__((packed)) iosinfo_t;

extern iosHashes iosHash[base_number];

#endif