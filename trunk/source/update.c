#include <gccore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <network.h>


#include "update.h"
#include "gecko.h"
#include "http.h"

s32 downloadSyscheckFile(const char* fileName)  {
	int ret = 0;
	static char buf[128];
	u32 http_status;
	u8* outbuf;
	u32 lenght;
	
	snprintf(buf, 127, "http://syscheck-hd.googlecode.com/svn/trunk/SysCheckHDE/%s", fileName);
		
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
				
	ret = http_get_result(&http_status, &outbuf, &lenght); 
		
	if (((int)*outbuf & 0xF0000000) == 0xF0000000) 
	{
		return -2;
	}
				
	sprintf(buf, "%s%s", PATH, fileName);
				
	FILE *file = fopen(buf, "w");

	if(!file)
	{
		gprintf("File Error\n");
		return -3;
	} else {
		fwrite(outbuf, lenght, 1, file);
		fclose(file);
	}
	free(outbuf);
	return 0;
}

s32 updateApp() {
	int ret = 0;
	
	ret = net_init();
	if (ret < 0)
		goto out;
	
	static char buf[128];
	u32 http_status;
	u8* outbuf;
	u32 length;
	const char	*checkStr = "Version=";

	snprintf(buf, 128, "http://syscheck-hd.googlecode.com/svn/trunk/Version.txt");
	
	ret = http_request(buf, 1 << 31);
	if (!ret) 
	{
		gprintf("Error making http request\n");
		return -1;
	}

	ret = http_get_result(&http_status, &outbuf, &length); 
	
	if (((int)*outbuf & 0xF0000000) == 0xF0000000) 
	{
		ret = -2;
		goto out;
	}
	
	if (!strncmp((char*)outbuf, checkStr, strlen(checkStr)))		
	{
		int version = atoi((char*)(outbuf + strlen(checkStr)));
		gprintf("INT: %i\n", version);
		
		if (version > REVISION) {
			ret = downloadSyscheckFile("boot.dol");
			if (ret < 0)
				goto out;
			ret = downloadSyscheckFile("meta.xml");
			if (ret < 0)
				goto out;
			ret = downloadSyscheckFile("icon.png");
			if (ret < 0)
				goto out;
		} else {
			ret = 2;
			goto out;
		}
		
	} else {
		ret = -3;
		goto out;
	}

	
out:
	net_deinit();
	return ret;
}