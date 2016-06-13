#pragma once

#include <ps4/kernel/function.h>

extern int ps4PayloadTemplateReturn64Size;
register_t ps4PayloadTemplateReturn64();
extern int ps4PayloadTemplateReturn32Size;
register_t ps4PayloadTemplateReturn32();
extern int ps4PayloadTemplateReturnZeroSize;
register_t ps4PayloadTemplateReturnZero();

extern int ps4PayloadTemplateJump64Size;
register_t ps4PayloadTemplateJump64();
extern int ps4PayloadTemplateJump32Size;
register_t ps4PayloadTemplateJump32();

extern int ps4PayloadTemplateFunctionHookPrologueSize;
register_t ps4PayloadTemplateFunctionHookPrologue();

extern int ps4PayloadTemplateSystemCallHookPrologueSize;
register_t ps4PayloadTemplateSystemCallHookPrologue();

int ps4PayloadReturnExtractValue(uint8_t *memory, int64_t *value);
int ps4PayloadReturnIsPatched(uint8_t *memory);
int ps4PayloadReturnPatch(uint8_t *memory, int64_t value);
int ps4PayloadReturnDeterminePatchSize(int64_t value, size_t *size);

int ps4PayloadJumpExtractTarget(uint8_t *memory, void **to);
int ps4PayloadJumpIsPatched(uint8_t *memory);
int ps4PayloadJumpPatch(uint8_t *memory, void *to);
int ps4PayloadJumpDeterminePatchSize(void *to, size_t *size);

int ps4PayloadFunctionHookPrologueExtractArgument(uint8_t *memory, Ps4KernelFunctionHookArgument **argument);
int ps4PayloadFunctionHookPrologueIsPatched(uint8_t *memory);
int ps4PayloadFunctionHookProloguePatch(uint8_t *memory, Ps4KernelFunctionHookArgument *argument);
int ps4PayloadFunctionHookPrologueDeterminePatchSize(size_t *size);

int ps4PayloadSystemCallHookPrologueExtractArgument(uint8_t *memory, Ps4KernelSystemCallHookArgument **argument);
int ps4PayloadSystemCallHookPrologueIsPatched(uint8_t *memory);
int ps4PayloadSystemCallHookProloguePatch(uint8_t *memory, Ps4KernelSystemCallHookArgument *argument);
int ps4PayloadSystemCallHookPrologueDeterminePatchSize(size_t *size);

int ps4PayloadHiddenInformationExtractData(uint8_t *memory, void **data, size_t *size);
int ps4PayloadHiddenInformationIsPatched(uint8_t *memory);
int ps4PayloadHiddenInformationPatch(uint8_t *memory, void *data, uint8_t size);
int ps4PayloadHiddenInformationDeterminePatchSize(size_t *size);
