/****************************************************************************
 * Copyright (C) 2014 FIX94
 * Changes, based on revision 3, by JoostinOnline
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _WUPC_H_
#define _WUPC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct WUPCData_s {
	s16 xAxisL;
	s16 xAxisR;
	s16 yAxisL;
	s16 yAxisR;
	u32 button;
	u8 battery;
	u8 extra;
} WUPCData;

enum {
	WUPC_CHAN_0,
	WUPC_CHAN_1,
	WUPC_CHAN_2,
	WUPC_CHAN_3,
	WUPC_MAX_REMOTES
};

enum {
	WUPC_BATTERY_CRITICAL,
	WUPC_BATTERY_LOW,
	WUPC_BATTERY_MEDIUM,
	WUPC_BATTERY_HIGH,
	WUPC_BATTERY_FULL
};

enum {
	WUPC_STATE_DISABLED,
	WUPC_STATE_ENABLED
};

#define WUPC_ERR_NONE				 0
#define WUPC_ERR_NOT_READY			-1
#define WUPC_ERR_BAD_CHANNEL		-2
#define WUPC_ERR_BADCONF			-3
#define WUPC_ERR_UNKNOWN			-4

#define WUPC_EXTRA_BUTTON_RSTICK	0x01
#define WUPC_EXTRA_BUTTON_LSTICK	0x02
#define WUPC_EXTRA_CHARGING			0x04
#define WUPC_EXTRA_USBCONNECTED		0x08

// Identical to WPAD_CLASSIC_BUTTON_ values in wpad.h
#define WUPC_BUTTON_UP				(0x0001<<16)
#define WUPC_BUTTON_LEFT			(0x0002<<16)
#define WUPC_BUTTON_ZR				(0x0004<<16)
#define WUPC_BUTTON_X				(0x0008<<16)
#define WUPC_BUTTON_A				(0x0010<<16)
#define WUPC_BUTTON_Y				(0x0020<<16)
#define WUPC_BUTTON_B				(0x0040<<16)
#define WUPC_BUTTON_ZL				(0x0080<<16)
#define WUPC_BUTTON_FULL_R			(0x0200<<16)
#define WUPC_BUTTON_PLUS			(0x0400<<16)
#define WUPC_BUTTON_HOME			(0x0800<<16)
#define WUPC_BUTTON_MINUS			(0x1000<<16)
#define WUPC_BUTTON_FULL_L			(0x2000<<16)
#define WUPC_BUTTON_DOWN			(0x4000<<16)
#define WUPC_BUTTON_RIGHT			(0x8000<<16)

typedef void (*WUPCShutdownCallback)(s32 chan);

s32 WUPC_Init(void); // Call before WPAD_Init()
s32 WUPC_Disconnect(u8 chan);
void WUPC_Shutdown(void); // Call before WPAD_Shutdown()
void WUPC_SetIdleTimeout(u32 seconds);
void WUPC_SetPowerButtonCallback(WUPCShutdownCallback cb);
void WUPC_SetBatteryDeadCallback(WUPCShutdownCallback cb);
WUPCData *WUPC_Data(u8 chan);
void WUPC_Rumble(u8 chan, bool rumble);
u32 WUPC_ScanPads(void);
u32 WUPC_ButtonsUp(u8 chan);
u32 WUPC_ButtonsDown(u8 chan);
u32 WUPC_ButtonsHeld(u8 chan);
s16 WUPC_lStickX(u8 chan);
s16 WUPC_lStickY(u8 chan);
s16 WUPC_rStickX(u8 chan);
s16 WUPC_rStickY(u8 chan);
u8 WUPC_BatteryLevel(u8 chan);

#ifdef __cplusplus
}
#endif

#endif
