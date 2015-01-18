/*
 * arp_get.h
 *
 * Utility to get mac address for a given ip.
 *
 * (c) 2015 Herman verschooten
 */
#ifndef __arp_get
#define __arp_get

VALUE method_arp_get(VALUE, VALUE, VALUE);

char * arp_get(char *, char *);
#endif
