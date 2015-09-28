#include <gccore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <network.h>
#include <dirent.h>
#include <unistd.h>

#include "update.h"
#include "gecko.h"
#include "http.h"
#include "tools.h"

s32 downloadSyscheckFile(const char* update_dir, const char* fileName)  {
	int ret = 0;
	char buf[128] = {0};
	u32 http_status;
	u8* outbuf;
	u32 length;

	snprintf(buf, sizeof(buf), "http://sourceforge.net/p/syscheck-hde/code/HEAD/tree/trunk/SysCheckHDE/%s?format=raw", fileName);

	ret = http_request(buf, 1 << 31);
	if (!ret)
	{
		int i;
		for (i = 0; i < 10; i++) {
			ret = http_request(buf, 1 << 31);
			if (ret) break;
			if (i >= 10) {
				gprintf("Error making http request\n");
				return -1;
			}
		}
	}

	ret = http_get_result(&http_status, &outbuf, &length);

	if (((int)*outbuf & 0xF0000000) == 0xF0000000)
	{
		return -2;
	}

	sprintf(buf, "%s%s", update_dir, fileName);

	FILE *file = fopen(buf, "w");

	if(!file)
	{
		gprintf("File Error\n");
		return -3;
	} else {
		fwrite(outbuf, length, 1, file);
		fclose(file);
	}
	free(outbuf);
	return 0;
}

s32 updateApp(void) {
	int ret = net_init();
	
	char update_dir[21];
	sprintf(update_dir, "%s:/apps/SysCheckHDE", arguments.USB ? "usb" : "sd");
	mkdir(update_dir,S_IWRITE|S_IREAD); // attempt to make dir
	chdir(update_dir);
	
	if (ret < 0) {
		net_deinit();
		return ret;
	}
	u32 http_status;
	u8* outbuf;
	u32 length;

	ret = http_request("http://sourceforge.net/p/syscheck-hde/code/HEAD/tree/trunk/Version.txt?format=raw", 1 << 31);
	if (!ret)
	{
		gprintf("Error making http request\n");
		return -1;
	}

	ret = http_get_result(&http_status, &outbuf, &length);

	if (!strncmp((char*)outbuf, "Version=", sizeof("Version=")))
	{
		int version = atoi((char*)(outbuf + sizeof("Version=")));
		gprintf("INT: %i\n", version);

		if (version > REVISION) {
			ret = downloadSyscheckFile(update_dir, "boot.dol");
			if (ret < 0) {
				net_deinit();
				return ret;
			}
			ret = downloadSyscheckFile(update_dir, "meta.xml");
			if (ret < 0) {
				net_deinit();
				return ret;
			}
			ret = downloadSyscheckFile(update_dir, "icon.png");
			if (ret < 0) {
				net_deinit();
				return ret;
			}
		} else {
			net_deinit();
			return -2;
		}

	} else {
		net_deinit();
		return -3;
	}
	net_deinit();
	return ret;
}
