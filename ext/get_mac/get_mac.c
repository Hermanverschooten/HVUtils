/*
 * get_mac.c
 *
 * Utility to get mac address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <get_mac.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <string.h>
#include <errno.h>
#include <net/if_dl.h>

VALUE GetMac = Qnil;

void Init_get_mac() {
    GetMac = rb_define_module("GetMac");
    rb_define_module_function(GetMac, "string", method_get_string, 1);
}

VALUE method_get_string(VALUE self, VALUE interface) {
    char* iface = StringValueCStr(interface);
    char* mac = get_iface_mac(iface);
    VALUE rc = Qnil;
    if(mac != NULL)
        rc = rb_str_new2(mac);
    free(mac);
    return rc;
}

char *
get_iface_mac(const char *ifname) {
#ifdef OS_UNIX
    int r, s;
    struct ifreq ifr;
    char *hwaddr, mac[13];

    strcpy(ifr.ifr_name, ifname);

    s = socket(AF_PACKET, SOCK_DGRAM, 0);
    if (-1 == s) {
        printf("get_iface_mac socket: %s", strerror(errno));
        return NULL;
    }

    r = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (r == -1) {
        printf("get_iface_mac ioctl(SIOCGIFHWADDR): %s", strerror(errno));
        close(s);
        return NULL;
    }

    hwaddr = ifr.ifr_hwaddr.sa_data;
    close(s);
    snprintf(mac, sizeof (mac), "%02X%02X%02X%02X%02X%02X",
            hwaddr[0] & 0xFF,
            hwaddr[1] & 0xFF,
            hwaddr[2] & 0xFF,
            hwaddr[3] & 0xFF,
            hwaddr[4] & 0xFF,
            hwaddr[5] & 0xFF
            );

    return strdup(mac);
#elif defined(_DARWIN_C_SOURCE)
    struct ifaddrs *ifa, *ifap;
    const char *hwaddr;
    char mac[13], *str = NULL;
    struct sockaddr_dl *sdl;

    if (getifaddrs(&ifap) == -1) {
        printf("getifaddrs(): %s", strerror(errno));
        return NULL;
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) == 0 &&
                ifa->ifa_addr->sa_family == AF_LINK)
            break;
    }
    if (ifa == NULL) {
        printf("%s: no link-layer address assigned", ifname);
        goto out;
    }
    sdl = (struct sockaddr_dl *) ifa->ifa_addr;
    hwaddr = LLADDR(sdl);
    snprintf(mac, sizeof (mac), "%02X%02X%02X%02X%02X%02X",
            hwaddr[0] & 0xFF, hwaddr[1] & 0xFF,
            hwaddr[2] & 0xFF, hwaddr[3] & 0xFF,
            hwaddr[4] & 0xFF, hwaddr[5] & 0xFF);

out:
    freeifaddrs(ifap);
    return strdup(mac);
#else
    return NULL ;
#endif
}


