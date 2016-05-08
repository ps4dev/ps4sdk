#pragma once

extern int ps4PayloadStubReturn64Size;
register_t ps4PayloadStubReturn64();
extern int ps4PayloadStubReturn32Size;
register_t ps4PayloadStubReturn32();
extern int ps4PayloadStubReturnZeroSize;
register_t ps4PayloadStubReturnZero();

extern int ps4PayloadStubJump64Size;
register_t ps4PayloadStubJump64();
extern int ps4PayloadStubJump32Size;
register_t ps4PayloadStubJump32();

extern int ps4PayloadStubJumpHookStage0Size;
register_t ps4PayloadStubJumpHookStage0();

extern int ps4PayloadJumpHookStage1Size;
register_t ps4PayloadJumpHookStage1();
