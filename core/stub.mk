#these export a similarly named make variable
# all syscalls here
include $(CURDIR)/syscalls.mk
# all kernel functions here
include $(CURDIR)/kerns.mk
# all libcinternal functions here
include $(CURDIR)/libcinternals.mk
# all libc functions here
#include $(CURDIR)/libcs.mk
# all libkernel functions here
include $(CURDIR)/libkernels.mk

# we externalize these three because of size and to generate mixed libs
$(eval $(call generateModule, libSceLibcInternal, $(libCInternals)))
#$(eval $(call generateModule, libc, $(libcs)))
$(eval $(call generateModule, libkernel, $(libKernels)))

# all plain modules and symbols here
$(eval $(call generateModule, libSceCamera, sceCameraAudioOpen sceCameraClose sceCameraCloseByHandle sceCameraGetAttribute sceCameraGetAutoExposureGain sceCameraGetAutoWhiteBalance sceCameraGetCalibrationData sceCameraGetConfig sceCameraGetContrast sceCameraGetDefectivePixelCancellation sceCameraGetDeviceInfo sceCameraGetExposureGain sceCameraGetFrameData sceCameraGetGamma sceCameraGetHue sceCameraGetLensCorrection sceCameraGetSaturation sceCameraGetSharpness sceCameraGetWhiteBalance sceCameraIsAttached sceCameraIsValidFrameData sceCameraOpen sceCameraOpenByModuleId sceCameraSetAttribute sceCameraSetAutoExposureGain sceCameraSetAutoWhiteBalance sceCameraSetCalibData sceCameraSetConfig sceCameraSetConfigInternal sceCameraSetContrast sceCameraSetDefectivePixelCancellation sceCameraSetExposureGain sceCameraSetGamma sceCameraSetHue sceCameraSetLensCorrection sceCameraSetSaturation sceCameraSetSharpness sceCameraSetWhiteBalance sceCameraStart sceCameraStartByHandle sceCameraStop sceCameraStopByHandle))
$(eval $(call generateModule, libSceNet, sceNetAccept sceNetBind sceNetConnect sceNetGetsockname sceNetGetsockopt sceNetHtonl sceNetHtonll sceNetHtons sceNetInetNtop sceNetInetPton sceNetListen sceNetNtohl sceNetNtohll sceNetNtohs sceNetRecv sceNetSend sceNetSetsockopt sceNetSocket sceNetSocketAbort sceNetSocketClose))
$(eval $(call generateModule, libScePad, scePadClose scePadInit scePadOpen scePadRead scePadReadState))
$(eval $(call generateModule, libSceSysmodule, sceSysmoduleIsLoaded sceSysmoduleLoadModule sceSysmoduleUnloadModule))
$(eval $(call generateModule, libSceUsbd, sceUsbdBulkTransfer sceUsbdCheckConnected sceUsbdClearHalt sceUsbdClose sceUsbdControlTransfer sceUsbdExit sceUsbdFreeConfigDescriptor sceUsbdFreeDeviceList sceUsbdGetActiveConfigDescriptor sceUsbdGetConfigDescriptor sceUsbdGetConfigDescriptorByValue sceUsbdGetDeviceDescriptor sceUsbdGetDeviceList sceUsbdInit sceUsbdInterruptTransfer sceUsbdOpen sceUsbdOpenDeviceWithVidPid sceUsbdResetDevice sceUsbdSetInterfaceAltSetting))

# no need to touch, generates non-module libs
$(eval $(call generateLibraryLarge, kern_function, libPs4Kern, none, $(kerns)))
$(eval $(call generateLibrary, syscall, libPs4Syscall, none, $(syscalls)))

# mixed libs
$(eval $(call generateLibrary, mixed_syscall, libPs4SyscallMixed, none, $(sort $(filter $(syscalls),$(kerns)))))
$(eval $(call generateLibrary, mixed_function, libPs4LibCInternalMixed, libSceLibcInternal, $(sort $(filter $(libCInternals),$(kerns)))))
#$(eval $(call generateLibrary, mixed_function, libcAndKernel, libc, $(sort $(filter $(libc),$(kerns)))))
$(eval $(call generateLibrary, mixed_function, libPs4LibKernelMixed, libkernel, $(sort $(filter $(libKernels),$(kerns)))))
