#pragma once

int ps4PayloadStubUtilReturnGet(uint8_t *stub, register_t *value);
int ps4PayloadStubUtilReturnSet(uint8_t *stub, register_t value);
int ps4PayloadStubUtilReturnIs(uint8_t *stub);
int ps4PayloadStubUtilReturnPatch(uint8_t *stub, register_t value);
int ps4PayloadStubUtilReturnSize(register_t value, size_t *size);
int ps4PayloadStubUtilJumpGet(uint8_t *stub, register_t *value);
int ps4PayloadStubUtilJumpSet(uint8_t *stub, register_t value);
int ps4PayloadStubUtilJumpIs(uint8_t *stub);
int ps4PayloadStubUtilJumpPatch(uint8_t *stub, register_t value);
int ps4PayloadStubUtilJumpSize(register_t value, size_t *size);
int ps4PayloadStubUtilJumpHookStage0Is(uint8_t *stub);
int ps4PayloadStubUtilJumpHookStage0Patch(uint8_t *stub, register_t value);
int ps4PayloadStubUtilJumpHookStage0Get(uint8_t *stub, register_t *value);
int ps4PayloadStubUtilJumpHookStage0Set(uint8_t *stub, register_t value);
int ps4PayloadStubUtilJumpHookStage0Size(register_t value, size_t *size);
