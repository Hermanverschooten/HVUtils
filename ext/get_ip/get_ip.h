/*
 * get_ip.h
 *
 * Utility to get ip address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#ifndef __get_ip
#define __get_ip

VALUE method_get_ip(VALUE, VALUE);

int get_iface_ip(const char *,char *);

#endif
