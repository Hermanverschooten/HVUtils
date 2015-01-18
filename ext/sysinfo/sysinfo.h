/*
 * sysinfo.h
 *
 * Utility to get system information
 *
 * (c) 2015 Herman verschooten
 */
#ifndef __sysinfo
#define __sysinfo

#ifdef __linux__
#include <sys/sysinfo.h>
#endif
#if defined(_DARWIN_C_SOURCE)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

VALUE method_sysinfo(VALUE);

#endif

