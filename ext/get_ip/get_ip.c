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
    char ip[INET_ADDRSTRLEN];
    VALUE rc = Qnil;
    if(get_iface_ip(iface, ip)==0)
        rc = rb_str_new2(ip);
    return rc;
}

int
get_iface_ip(const char *ifname, char *result) {
#if defined(__linux__)
    struct ifreq if_data;
    int sockd;
    struct sockaddr_in* ipaddr;

    if((sockd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        printf("get_iface_ip socket(): %s\n", strerror(errno));
        return -1;
    }

    strcpy(if_data.ifr_name, ifname);

    if(ioctl(sockd, SIOCGIFADDR, &if_data) < 0) {
        printf("get_iface_ip ioctl(SIOCGIFADDR): %s\n", strerror(errno));
        close(sockd);
        return -1;
    }

    close(sockd);

    ipaddr = (struct sockaddr_in*)&if_data.ifr_addr;

    inet_ntop(AF_INET, &(ipaddr->sin_addr), result, INET_ADDRSTRLEN);
    return 0;

#elif defined(_DARWIN_C_SOURCE)
    struct ifaddrs *ifa, *ifap;
    struct sockaddr_in* ipaddr;

    if (getifaddrs(&ifap) == -1) {
        printf("get_iface_ip getifaddrs(): %s", strerror(errno));
        return -1;
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, ifname) == 0 &&
                ifa->ifa_addr->sa_family == AF_INET)
            break;
    }
    if (ifa == NULL) {
        printf("%s: no IPV4 address assigned", ifname);
        freeifaddrs(ifap);
        return -1;
    }

    ipaddr = (struct sockaddr_in *) ifa->ifa_addr;

    inet_ntop(AF_INET, &(ipaddr->sin_addr), result, INET_ADDRSTRLEN);

    freeifaddrs(ifap);
    return 0;
#else
    return -1 ;
#endif
}

