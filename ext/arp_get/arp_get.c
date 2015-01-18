/*
 * arp_get.c
 *
 * Utility to get MAC address for a given ip.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <arp_get.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>


VALUE Module = Qnil;

void Init_arp_get() {
    Module = rb_define_module("Hvutils");
    rb_define_module_function(Module, "arp_get", method_arp_get, 2);
}

VALUE method_arp_get(VALUE self, VALUE interface, VALUE ipaddress) {
#if defined(__linux__)
    char* iface = StringValueCStr(interface);
    char* req_ip = StringValueCStr(ipaddress);
    int s;
    struct  sockaddr_in *sin;
    struct  in_addr     ipaddr;
    struct arpreq       areq;
    char                mac[18];
    unsigned char *ptr ;

    if((s=socket(AF_INET, SOCK_DGRAM,0)) == -1){
        rb_raise(rb_eIOError, "Could not create socket in arp_get");
        return Qnil;
    }

    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    inet_pton(AF_INET, req_ip, &ipaddr);
    sin->sin_addr = ipaddr;
    sin->sin_family = AF_INET;

    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family= ARPHRD_ETHER;

    strncpy(areq.arp_dev,iface, 15);

    if (ioctl(s,SIOCGARP, (caddr_t) &areq)==-1){
        close(s);
        if(errno != ENXIO)
            rb_raise(rb_eIOError,"Unable to make ARP request, %s", strerror(errno));
        return Qnil;
    }
    close(s);

    ptr = (unsigned char *) &(areq.arp_ha.sa_data);
    sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
            (ptr[0] & 0377), (ptr[1] & 0377),
            (ptr[2] & 0377), (ptr[3] & 0377),
            (ptr[4] & 0377), (ptr[5] & 0377));
    return rb_str_new2(mac);
#else
    return Qnil;
#endif
}
