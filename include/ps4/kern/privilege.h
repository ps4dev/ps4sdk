#pragma once

void ps4KernProtectionWriteDisable();
void ps4KernProtectionWriteEnable();
void ps4KernProtectionExecuteDisable();
void ps4KernProtectionExecuteEnable();
void ps4KernProtectionAllDisable();
void ps4KernProtectionAllEnable();

void ps4KernPrivilegeRoot();
int ps4KernPrivilegeUnjail();
int ps4KernPrivilegeEscalate();
