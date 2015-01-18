/*
 * get_mac.c
 *
 * Utility to get mac address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <get_mac.h>
#include <errno.h>
#ifdef _DARWIN_C_SOURCE
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif
#ifdef __linux__
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#endif


VALUE GetMac = Qnil;

void Init_get_mac() {
    GetMac = rb_define_module("Hvutils");
    rb_define_module_function(GetMac, "get_mac", method_get_mac, 1);
}

VALUE method_get_mac(VALUE self, VALUE interface) {
    char* ifname = StringValueCStr(interface);
    char mac[13];
#if defined(__linux__)
    int r, s;
    struct ifreq ifr;
    char *hwaddr;

    strcpy(ifr.ifr_name, ifname);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (-1 == s) {
        rb_raise(rb_eIOError, "%s", strerror(errno));
        return Qnil;
    }

    r = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (r == -1) {
        close(s);
        rb_raise(rb_eIOError, "%s", strerror(errno));
        return Qnil;
    }

    hwaddr = ifr.ifr_hwaddr.sa_data;
    close(s);

    snprintf(mac, sizeof (mac), "%02X%02X%02X%02X%02X%02X",
            hwaddr[0] & 0xFF, hwaddr[1] & 0xFF,
            hwaddr[2] & 0xFF, hwaddr[3] & 0xFF,
            hwaddr[4] & 0xFF, hwaddr[5] & 0xFF);

#elif defined(_DARWIN_C_SOURCE)
    struct ifaddrs *ifa, *ifap;
    const char *hwaddr;
    struct sockaddr_dl *sdl;

    if (getifaddrs(&ifap) == -1) {
        rb_raise(rb_eIOError, "%s", strerror(errno));
        return Qnil;
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) == 0 &&
                ifa->ifa_addr->sa_family == AF_LINK)
            break;
    }
    if (ifa == NULL) {
        freeifaddrs(ifap);
        rb_raise(rb_eIOError,"%s: no link-layer address assigned", ifname);
        return Qnil;
    }
    sdl = (struct sockaddr_dl *) ifa->ifa_addr;
    hwaddr = LLADDR(sdl);
    snprintf(mac, sizeof (mac), "%02X%02X%02X%02X%02X%02X",
            hwaddr[0] & 0xFF, hwaddr[1] & 0xFF,
            hwaddr[2] & 0xFF, hwaddr[3] & 0xFF,
            hwaddr[4] & 0xFF, hwaddr[5] & 0xFF);

    freeifaddrs(ifap);
#else
    return Qnil;
#endif
    return rb_str_new2(mac);
}

