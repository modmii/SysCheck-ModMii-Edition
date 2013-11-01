#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ogcsys.h>
#include <stdarg.h>
#include <network.h>
#include <ogc/machine/processor.h>
#include <ogc/conf.h>
#include <wiiuse/wpad.h>
#include <ctype.h>

#include "sys.h"
#include "video.h"
#include "SysMenuInfo.h"

#include "ticket_dat.h"
#include "tmd_dat.h"

#include "mload.h"
#include "title.h"
#include "sha1.h"
#include "gecko.h"
#include "http.h"
#include "gui.h"
#include "languages.h"
#include "fatMounter.h"
#include "thread.h"

// Variables
bool NandInitialized = false;
bool debug = false;

void logfile(const char *format, ...)
{
  if (!debug) return;
  MountSD();
  char buffer[4096];
  //char temp[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  FILE *f;
  //sprintf(temp, "SD:/sysCheckDebug.log");
  f = fopen("SD:/sysCheckDebug.log", "a");
  fputs(buffer, f);
  fclose(f);
  va_end (args);
  UnmountSD();
}

/**
* A simple structure to keep track of the size of a malloc()ated block of memory
*/
struct block
{
	u32 size;
	unsigned char *data;
};

const struct block emptyblock = { 0, NULL };

void *allocate_memory(u32 size)
{
	return memalign(32, (size+31)&(~31) );
}

static void disable_memory_protection() {
	write32(MEM_PROT, read32(MEM_PROT) & 0x0000FFFF);
}

static u32 apply_patch(char *name, const u8 *old, u32 old_size, const u8 *patch, u32 patch_size, u32 patch_offset) {
	u8 *ptr_start = (u8*)*((u32*)0x80003134), *ptr_end = (u8*)0x94000000;
	u32 found = 0;
	u8 *location = NULL;
	while (ptr_start < (ptr_end - patch_size)) {
		if (!memcmp(ptr_start, old, old_size)) {
			found++;
			location = ptr_start + patch_offset;
			u8 *start = location;
			u32 i;
			for (i = 0; i < patch_size; i++) {
				*location++ = patch[i];
			}
			DCFlushRange((u8 *)(((u32)start) >> 5 << 5), (patch_size >> 5 << 5) + 64);
			ICInvalidateRange((u8 *)(((u32)start) >> 5 << 5), (patch_size >> 5 << 5) + 64);
		}
		ptr_start++;
	}
	return found;
}

static const u8 di_readlimit_old[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x0A, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x7E, 0xD4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08
};
static const u8 di_readlimit_patch[] = { 0x7e, 0xd4 };

const u8 isfs_permissions_old[] = { 0x42, 0x8B, 0xD0, 0x01, 0x25, 0x66 };
const u8 isfs_permissions_patch[] = { 0x42, 0x8B, 0xE0, 0x01, 0x25, 0x66 };
static const u8 setuid_old[] = { 0xD1, 0x2A, 0x1C, 0x39 };
static const u8 setuid_patch[] = { 0x46, 0xC0 };
const u8 es_identify_old[] = { 0x28, 0x03, 0xD1, 0x23 };
const u8 es_identify_patch[] = { 0x00, 0x00 };
const u8 hash_old[] = { 0x20, 0x07, 0x23, 0xA2 };
const u8 hash_patch[] = { 0x00 };
const u8 new_hash_old[] = { 0x20, 0x07, 0x4B, 0x0B };
const u8 addticket_vers_check[] = { 0xD2, 0x01, 0x4E, 0x56 };
const u8 addticket_patch[] = { 0xE0 };
const u8 es_set_ahbprot_pattern[] = { 0x68, 0x5B, 0x22, 0xEC, 0x00, 0x52, 0x18, 0x9B, 0x68, 0x1B, 0x46, 0x98, 0x07, 0xDB };
const u8 es_set_ahbprot_patch[]   = { 0x01 };


u32 IOSPATCH_Apply(void) {
	u32 count = 0;
	s32 ret = 0;

	if (HAVE_AHBPROT) {
		disable_memory_protection();
		ret = apply_patch("es_set_ahbprot", es_set_ahbprot_pattern, sizeof(es_set_ahbprot_pattern), es_set_ahbprot_patch, sizeof(es_set_ahbprot_patch), 25);
	}
	if (ret) {
		IOS_ReloadIOS(IOS_GetVersion());
	} else {
		return 0;
	}

	if (HAVE_AHBPROT) {
		disable_memory_protection();
		//count += apply_patch("di_readlimit", di_readlimit_old, sizeof(di_readlimit_old), di_readlimit_patch, sizeof(di_readlimit_patch), 12);
		count += apply_patch("isfs_permissions", isfs_permissions_old, sizeof(isfs_permissions_old), isfs_permissions_patch, sizeof(isfs_permissions_patch), 0);
		//count += apply_patch("es_setuid", setuid_old, sizeof(setuid_old), setuid_patch, sizeof(setuid_patch), 0);
		//count += apply_patch("es_identify", es_identify_old, sizeof(es_identify_old), es_identify_patch, sizeof(es_identify_patch), 2);
		//count += apply_patch("hash_check", hash_old, sizeof(hash_old), hash_patch, sizeof(hash_patch), 1);
		//count += apply_patch("new_hash_check", new_hash_old, sizeof(new_hash_old), hash_patch, sizeof(hash_patch), 1);
		//count += apply_patch("add ticket patch", addticket_vers_check, sizeof(addticket_vers_check), addticket_patch, sizeof(addticket_patch), 0);
		count += apply_patch("es_set_ahbprot", es_set_ahbprot_pattern, sizeof(es_set_ahbprot_pattern), es_set_ahbprot_patch, sizeof(es_set_ahbprot_patch), 25);
	}
	return count;
}

u32 es_set_ahbprot(void) {
	disable_memory_protection();
	return apply_patch("es_set_ahbprot", es_set_ahbprot_pattern, sizeof(es_set_ahbprot_pattern), es_set_ahbprot_patch, sizeof(es_set_ahbprot_patch), 25);
}

bool checkISFSinRAM(void) {
	disable_memory_protection();
	bool ret = true;
	u8 *ptr_start = (u8*)*((u32*)0x80003134), *ptr_end = (u8*)0x94000000;
	while (ptr_start < (ptr_end - sizeof(isfs_permissions_old))) {
		if (!memcmp(ptr_start, isfs_permissions_old, sizeof(isfs_permissions_old))) {
			ret = false;
		}
		ptr_start++;
	}
	return ret;
}

int NandStartup(void)
{
	if (NandInitialized)
		return 1;

	int ret = ISFS_Initialize();

	NandInitialized = (ret == ISFS_OK);

	sleep(1);

	return ret;
}

void NandShutdown(void)
{
	if (!NandInitialized)
		return;

	ISFS_Deinitialize();

	NandInitialized = false;
}