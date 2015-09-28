#include <fat.h>
#include <gccore.h>
#include <string.h>
#include <sdcard/wiisd_io.h>
#include <ogc/usbstorage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ogcsys.h>

// These are the only stable and speed is good
#define CACHE 8
#define SECTORS 64
#define BYTES_PER_READ 512

extern DISC_INTERFACE __io_usbstorage;

enum BPB
{
	BPB_jmpBoot               = 0x00,
	BPB_OEMName               = 0x03,

	// BIOS Parameter Block
	BPB_bytesPerSector        = 0x0B,
	BPB_sectorsPerCluster     = 0x0D,
	BPB_reservedSectors       = 0x0E,
	BPB_numFATs               = 0x10,
	BPB_rootEntries           = 0x11,
	BPB_numSectorsSmall       = 0x13,
	BPB_mediaDesc             = 0x15,
	BPB_sectorsPerFAT         = 0x16,
	BPB_sectorsPerTrk         = 0x18,
	BPB_numHeads              = 0x1A,
	BPB_numHiddenSectors      = 0x1C,
	BPB_numSectors            = 0x20,

	// Ext BIOS Parameter Block for FAT16
	BPB_FAT16_driveNumber     = 0x24,
	BPB_FAT16_reserved1       = 0x25,
	BPB_FAT16_extBootSig      = 0x26,
	BPB_FAT16_volumeID        = 0x27,
	BPB_FAT16_volumeLabel     = 0x2B,
	BPB_FAT16_fileSysType     = 0x36,

	// Bootcode
	BPB_FAT16_bootCode        = 0x3E,

	// FAT32 Extended Block
	BPB_FAT32_sectorsPerFAT32 = 0x24,
	BPB_FAT32_extFlags        = 0x28,
	BPB_FAT32_fsVer           = 0x2A,
	BPB_FAT32_rootClus        = 0x2C,
	BPB_FAT32_fsInfo          = 0x30,
	BPB_FAT32_bkBootSec       = 0x32,

	// Ext BIOS Parameter Block for FAT32
	BPB_FAT32_driveNumber     = 0x40,
	BPB_FAT32_reserved1       = 0x41,
	BPB_FAT32_extBootSig      = 0x42,
	BPB_FAT32_volumeID        = 0x43,
	BPB_FAT32_volumeLabel     = 0x47,
	BPB_FAT32_fileSysType     = 0x52,

	// Bootcode
	BPB_FAT32_bootCode        = 0x5A,
	BPB_bootSig_55            = 0x1FE,
	BPB_bootSig_AA            = 0x1FF
};

static const char FAT_SIG[3] = {'F', 'A', 'T'};
static bool sd_mounted = false;
static bool usb_mounted = false;

static bool _FAT_partition_isFAT(const DISC_INTERFACE* disc, sec_t startSector)
{
	uint8_t sectorBuffer[BYTES_PER_READ] = {0};
	
	if (!disc->readSectors(startSector, 1, sectorBuffer)) return false;

	// Make sure it is a valid BPB
	if ((sectorBuffer[BPB_bootSig_55] != 0x55) || (sectorBuffer[BPB_bootSig_AA] != 0xAA)) return false;

	// Now verify that this is indeed a FAT partition
	if (memcmp(sectorBuffer + BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)) && memcmp(sectorBuffer + BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG))) return false;

	// Check again for the last two cases to make sure that we really have a FAT filesystem here and won't corrupt any data
	if(memcmp(sectorBuffer + BPB_FAT16_fileSysType, "FAT", 3) != 0 && memcmp(sectorBuffer + BPB_FAT32_fileSysType, "FAT32", 5) != 0) return false;

	return true;
}

static inline uint32_t u8array_to_u32 (const uint8_t* item, int offset)
{
	return (item[offset] | (item[offset + 1] << 8) | (item[offset + 2] << 16) | (item[offset + 3] << 24));
}

sec_t GetFATPartition(const DISC_INTERFACE* disc)
{
	int i;
	uint8_t sectorBuffer[BYTES_PER_READ] = {0};
	sec_t startSector = 0;
	
	if (!disc->startup()) return 0;

	// Read first sector of disc
	if (!disc->readSectors(0, 1, sectorBuffer)) startSector = 0;

	// Make sure it is a valid MBR or boot sector
	if ((sectorBuffer[BPB_bootSig_55] != 0x55) || (sectorBuffer[BPB_bootSig_AA] != 0xAA)) startSector = 0;

	if (!memcmp(sectorBuffer+BPB_FAT16_fileSysType, FAT_SIG, sizeof(FAT_SIG)))
	{
		// Check if there is a FAT string, which indicates this is a boot sector
		startSector = 0;
	}
	else if (!memcmp(sectorBuffer+BPB_FAT32_fileSysType, FAT_SIG, sizeof(FAT_SIG)))
	{
		// Check for FAT32
		startSector = 0;
	}
	else
	{
		// This is an MBR
		// Find first valid partition from MBR
		// First check for an active partition
		for (i = 0x1BE; (i < 0x1FE) && (sectorBuffer[i] != 0x80); i+= 0x10);

		// If it find an active partition, check for FAT Partition
		if (i != 0x1FE && !_FAT_partition_isFAT(disc, u8array_to_u32(sectorBuffer, 0x8+i))) i = 0x1FE;

		// If it didn't find an active partition, search for any valid partition
		if (i == 0x1FE)
		{
			for (i = 0x1BE; i < 0x1FE; i+= 0x10)
			{
				if (sectorBuffer[i+0x04] != 0x00 && _FAT_partition_isFAT(disc, u8array_to_u32(sectorBuffer, 0x8+i))) break;
			}
		}

		if (i != 0x1FE) startSector = u8array_to_u32(sectorBuffer, 0x8+i);
	}

	disc->shutdown();

	return startSector;
}

int MountSD(void)
{
	// Close all open files write back the cache and then shutdown them
	fatUnmount("SD:/");
	
	// Mount first FAT partition
	if (fatMount("SD", &__io_wiisd, GetFATPartition(&__io_wiisd), CACHE, SECTORS)) {
		sd_mounted = true;
		return 1;
	}
	return -1;
}

void UnmountSD(void)
{
	if (!sd_mounted) return;
	// Close all open files write back the cache and then shutdown them
	fatUnmount("SD:/");
}

int MountUSB(void)
{
	s32  ret;

	/* Initialize interface */
	ret = __io_usbstorage.startup();
	if (!ret)
		return -1;

	/* Mount device */
		ret = fatMountSimple("usb", &__io_usbstorage);
	if (!ret)
		return -2;

	/* Set root directory */
	chdir("usb:/");
	usb_mounted = true;

	return 0;
}

void UnmountUSB(void)
{

	if(!usb_mounted) return;
	/* Unmount device */
	fatUnmount("usb");

	/* Shutdown interface */
	__io_usbstorage.shutdown();


}

