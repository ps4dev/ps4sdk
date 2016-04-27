#pragma once

#include <stdint.h>
#include <sce/types/camera.h>

/* psxdev reversal */
int sceCameraClose(int handle);
int sceCameraGetConfig(int handle, SceCameraConfig *config);
int sceCameraGetDeviceConfig(int handle, SceCameraConfig *config);
int sceCameraGetDeviceInfo(int handle, SceCameraDeviceInfo *info);
int sceCameraGetFrameData(int handle, SceCameraFrameData *frame);
int sceCameraIsAttached(int index);
int sceCameraOpen(int userid, int type, int index, void *);
int sceCameraSetConfig(int handle, SceCameraConfig *config);
int sceCameraStart(int handle, SceCameraStartParameter *param);
int sceCameraStop(int handle);

/* released debug output */
int64_t sceCameraProcConfigStop();
int64_t sceCameraDevStop();

/* guessed from sceCameraSetCalibData */
int64_t sceCameraGetCalibrationData();

/* psxdev unreversed */
int64_t sceCameraAudioOpen();
int64_t sceCameraCloseByHandle();
int64_t sceCameraGetAttribute();
int64_t sceCameraGetAutoExposureGain();
int64_t sceCameraGetAutoWhiteBalance();
int64_t sceCameraGetContrast();
int64_t sceCameraGetDefectivePixelCancellation();
int64_t sceCameraGetExposureGain();
int64_t sceCameraGetGamma();
int64_t sceCameraGetHue();
int64_t sceCameraGetLensCorrection();
int64_t sceCameraGetSaturation();
int64_t sceCameraGetSharpness();
int64_t sceCameraGetWhiteBalance();
int64_t sceCameraIsValidFrameData();
int64_t sceCameraOpenByModuleId();
int64_t sceCameraSetAttribute();
int64_t sceCameraSetAutoExposureGain();
int64_t sceCameraSetAutoWhiteBalance();
int64_t sceCameraSetCalibData();
int64_t sceCameraSetConfigInternal();
int64_t sceCameraSetContrast();
int64_t sceCameraSetDefectivePixelCancellation();
int64_t sceCameraSetExposureGain();
int64_t sceCameraSetGamma();
int64_t sceCameraSetHue();
int64_t sceCameraSetLensCorrection();
int64_t sceCameraSetSaturation();
int64_t sceCameraSetSharpness();
int64_t sceCameraSetWhiteBalance();
int64_t sceCameraStartByHandle();
int64_t sceCameraStopByHandle();
