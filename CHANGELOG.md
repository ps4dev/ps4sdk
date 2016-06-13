# Changelog

## 0.1.0

* Changed all function signatures to return an int error code
* Unified all function names and argument order
* Switched kernel sockets to non-descriptor API (descriptors are process dependent)
* Promoted util functions to their own namespaces
* Removed kernel userland API (use ps4KernelCall or ps4KernelExecute instead to enter)
* Renamed kern API to kernel (userland takeover)
* Changed hooks to accept arbitrary signatures in addition to a syscall-like generic signature
* Added post-hooks
* Added syscall hooking
* Lock-less hooks and initial thread safety
* Altered namespaces, renamed isolate to generic and mixed to adaptive
* Initial automated docker builds
* Minor fixes
