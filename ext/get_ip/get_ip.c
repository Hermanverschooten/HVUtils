/*
 * get_ip.c
 *
 * Utility to get ip address for a given interface.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <get_ip.h>
#include <errno.h>
#include <arpa/inet.h>
#ifdef _DARWIN_C_SOURCE
#include <ifaddrs.h>
#endif
#ifdef __linux__
#include <net/if.h>
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

#if defined(__linux__)
    struct ifreq if_data;
    int sockd;
    struct sockaddr_in* ipaddr;

    if((sockd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        rb_raise(rb_eStandardError,"%s", strerror(errno));
        return Qnil;
    }

    strcpy(if_data.ifr_name, iface);

    if(ioctl(sockd, SIOCGIFADDR, &if_data) < 0) {
        close(sockd);
        rb_raise(rb_eRuntimeError,"%s", strerror(errno));
        return Qnil;
    }

    close(sockd);

    ipaddr = (struct sockaddr_in*)&if_data.ifr_addr;

    inet_ntop(AF_INET, &(ipaddr->sin_addr), ip, INET_ADDRSTRLEN);

#elif defined(_DARWIN_C_SOURCE)
    struct ifaddrs *ifa, *ifap;
    struct sockaddr_in* ipaddr;

    if (getifaddrs(&ifap) == -1) {
        rb_raise(rb_eStandardError,"%s", strerror(errno));
        return Qnil;
    }
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (strcmp(ifa->ifa_name, iface) == 0 &&
                ifa->ifa_addr->sa_family == AF_INET)
            break;
    }
    if (ifa == NULL) {
        freeifaddrs(ifap);
        rb_raise(rb_eRuntimeError, "%s: no IPV4 address assigned", iface);
        return Qnil;
    }

    ipaddr = (struct sockaddr_in *) ifa->ifa_addr;

    inet_ntop(AF_INET, &(ipaddr->sin_addr), ip, INET_ADDRSTRLEN);

    freeifaddrs(ifap);
#else
    return Qnil;
#endif
    return rb_str_new2(ip);
}

