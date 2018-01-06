#pragma once

#include <stdint.h>
#include <inttypes.h>

int scePadInit(void);
int scePadOpen(int userID, int, int, void *);
int scePadClose(int handle);
int scePadRead(int handle, void *data, int count);
int scePadReadState(int handle, void *data);
int scePadSetLightBar(int handle, uint32_t* rgb);
int scePadResetLightBar(int handle);

/* unreversed */
int64_t scePadResetOrientation();
int64_t scePadSetAngularVelocityDeadbandState();
int64_t scePadSetMotionSensorState();
int64_t scePadSetTiltCorrectionState();
int64_t scePadSetVibration();
