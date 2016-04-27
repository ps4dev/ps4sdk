#pragma once

extern int ps4PayloadSyscallCallbackSize;
int ps4PayloadSyscallCallback(struct thread *td, void *uap);

extern int ps4PayloadTruthSize;
register_t ps4PayloadTruth();

extern int ps4PayloadStubJumpSize;
register_t ps4PayloadStubJump();
