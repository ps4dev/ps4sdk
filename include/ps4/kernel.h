#pragma once

#define _KERNEL
#define _STANDALONE
#define __BSD_VISIBLE 1

#include <sys/cdefs.h>
#undef offsetof
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kthread.h>

// these are not to be included directly, for now
#include <ps4/kernel/base.h>
#include <ps4/kernel/cache.h>
#include <ps4/kernel/cache_global.h>
#include <ps4/kernel/descriptor.h>
#include <ps4/kernel/function.h>
#include <ps4/kernel/memory.h>
#include <ps4/kernel/privilege.h>
#include <ps4/kernel/protection.h>
#include <ps4/kernel/register.h>
#include <ps4/kernel/socket.h>
#include <ps4/kernel/symbol.h>
#include <ps4/kernel/system_call.h>
#include <ps4/kernel/thread.h>
#include <ps4/kernel/uart.h>

#include <ps4/error.h>
#include <ps4/exception.h>
#include <ps4/expression.h>
