/*
 * get_ip.c
 *
 * Utility to get ip address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <get_ip.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
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


VALUE Getip = Qnil;

void Init_get_ip() {
    Getip = rb_define_module("Hvutils");
    rb_define_module_function(Getip, "get_ip", method_get_ip, 1);
}

VALUE method_get_ip(VALUE self, VALUE interface) {
    char* iface = StringValueCStr(interface);
    char* ip = get_iface_ip(iface);
    VALUE rc = Qnil;
    if(ip != NULL)
        rc = rb_str_new2(ip);
    return rc;
}

char *
get_iface_ip(const char *ifname) {
#if defined(__linux__)
    struct ifreq if_data;
    int sockd;
    struct sockaddr_in* ipaddr;

    if((sockd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        printf("get_iface_ip socket(): %s\n", strerror(errno));
        return NULL;
    }

    strcpy(if_data.ifr_name, ifname);

    if(ioctl(sockd, SIOCGIFADDR, &if_data) < 0) {
        printf("get_iface_ip ioctl(SIOCGIFADDR): %s\n", strerror(errno));
        close(sockd);
        return NULL;
    }

    close(sockd);

    ipaddr = (struct sockaddr_in*)&if_data.ifr_addr;

    return inet_ntoa(ipaddr->sin_addr);
#elif defined(_DARWIN_C_SOURCE)
    struct ifaddrs *ifa, *ifap;
    char *str = NULL;
    struct sockaddr_in* ipaddr;

    if (getifaddrs(&ifap) == -1) {
        printf("get_iface_ip getifaddrs(): %s", strerror(errno));
        return NULL;
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) == 0 &&
                ifa->ifa_addr->sa_family == AF_INET)
            break;
    }
    if (ifa == NULL) {
        printf("%s: no IPV4 address assigned", ifname);
        goto out;
    }

    str = inet_ntoa(((struct sockaddr_in *) ifa->ifa_addr)->sin_addr);

out:
    freeifaddrs(ifap);
    return str;
#else
    return NULL ;
#endif
}

