#pragma once

int ps4KernelDescriptorWrite(struct thread *td, int fd, const void *data, size_t size);
int ps4KernelDescriptorPrint(struct thread *td, int fd, const char *format, ...);
