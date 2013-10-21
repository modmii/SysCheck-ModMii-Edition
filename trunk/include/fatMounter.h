#ifndef _FATMOUNTER_H_
#define _FATMOUNTER_H_

#ifdef __cplusplus
extern "C"
{
#endif

// Prototypes
int MountSD(void);
void UnmountSD(void);
int MountUSB(void);
void UnmountUSB(void);

#ifdef __cplusplus
}
#endif

#endif
