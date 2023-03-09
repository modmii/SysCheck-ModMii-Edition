#include <gccore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <network.h>
#include <dirent.h>
#include <unistd.h>
#include <fat.h>

#include "update.h"
#include "fatMounter.h"
#include "gecko.h"
#include "http.h"
#include "ssl.h"
#include "tools.h"

extern http_res result;

s32 downloadSyscheckFile(const char* update_dir, const char* fileName)  {
	int ret = 0;
	char buf[128] = {0};
	u32 http_status;
	u8* outbuf;
	u32 length;

	snprintf(buf, sizeof(buf), "https://raw.githubusercontent.com/modmii/SysCheck-ModMii-Edition/master/SysCheckME/%s", fileName);

	ret = http_request(buf, 1 << 31);
	if (!ret)
	{
		int i;
		for (i = 0; i < 10; i++) {
			ret = http_request(buf, 1 << 31);
			gprintf("result = %i\n", result);
			if (ret) break;
			if (i >= 10) {
				gprintf("Error making http request\n");
				return -1;
			}
		}
	}

	ret = http_get_result(&http_status, &outbuf, &length);
	gprintf("http_get_result returned %i\n", ret);

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
	if (outbuf) free(outbuf);
	return 0;
}

s32 updateApp(void) {
	int ret = net_init();
	ssl_init();
	char update_dir[25];
	char *version;
	sprintf(update_dir, "%s:/apps/SysCheckME/", arguments.USB ? "usb" : "sd");
	mkdir("/apps", S_IWRITE|S_IREAD); // attempt to make dir
	mkdir("/apps/SysCheckME", S_IWRITE|S_IREAD); // attempt to make dir
	chdir(update_dir);
	
	if (ret < 0) {
		net_deinit();
		return ret;
	}
	u32 http_status;
	u8* outbuf;
	u32 length;

	ret = http_request("https://raw.githubusercontent.com/modmii/SysCheck-ModMii-Edition/master/Version.txt", 1 << 31);
	if (!ret)
	{
		gprintf("Error making http request\n");
		return -1;
	}

	ret = http_get_result(&http_status, &outbuf, &length);
	version = (char*)calloc(length, sizeof(char));
	strncpy(version, (char*)outbuf, length);
	gprintf("ret = %i, http_status = %u, outbuf = %s, length = %u, version = %s\n", ret, http_status, (char*)outbuf, length, version+8); 
	if (!strncmp(version, "Version=", sizeof("Version=") - 1))
	{
		int latest_version = atoi(version + sizeof("Version=") - 1);
		gprintf("INT: %i\n", latest_version);
		free(version);
		if (latest_version > REVISION) {
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
			return 2;
		}

	} else {
		net_deinit();
		free(version);
		return -3;
	}
	if (outbuf) free(outbuf);
	net_deinit();
	return ret;
}
