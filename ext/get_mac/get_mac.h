/*
 * get_mac.h
 *
 * Utility to get mac address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#ifndef __get_mac_h__
#define __get_mac_h__

#define VERSION "0.0.1"

VALUE method_get_string(VALUE, VALUE);
VALUE method_get_numeric(VALUE, VALUE);
char *get_iface_mac(const char *ifname);

#endif
