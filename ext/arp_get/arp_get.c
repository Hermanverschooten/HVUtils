/*
 * arp_get.c
 *
 * Utility to get MAC address for a given ip.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <arp_get.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#ifdef __linux__
#include <net/if.h>
#include <netinet/in.h>
#endif


VALUE Module = Qnil;

void Init_arp_get() {
    Module = rb_define_module("Hvutils");
    rb_define_module_function(Module, "arp_get", method_arp_get, 2);
}

VALUE method_arp_get(VALUE self, VALUE interface, VALUE ipaddress) {
    char* iface = StringValueCStr(interface);
    char* ip = StringValueCStr(ipaddress);
    char* mac = arp_get(iface,ip);
    VALUE rc = Qnil;
    if(mac!= NULL)
        rc = rb_str_new2(mac);
    free(mac);
    return rc;
}

/*
 * Get the MAC address associated with the given IP address.
 */
char *
arp_get(char* iface, char *req_ip){
#if defined(__linux__)
    int s;
    struct  sockaddr_in *sin;
    struct  in_addr     ipaddr;
    struct arpreq       areq;
    char                mac[18];
    char                *reply = NULL;
    unsigned char *ptr ;

    if((s=socket(AF_INET, SOCK_DGRAM,0)) == -1){
        printf("Could not create socket in arp_get");
        return NULL;
    }

    memset(&areq, 0, sizeof(areq));
    sin = (struct sockaddr_in *) &areq.arp_pa;
    inet_aton(req_ip, &ipaddr);
    sin->sin_addr = ipaddr;
    sin->sin_family = AF_INET;

    sin = (struct sockaddr_in *) &areq.arp_ha;
    sin->sin_family= ARPHRD_ETHER;

    strncpy(areq.arp_dev,iface, 15);

    if (ioctl(s,SIOCGARP, (caddr_t) &areq)==-1){
        printf( "Unable to make ARP request");
        close(s);
        return NULL;
    }
    close(s);

    ptr = (unsigned char *) &(areq.arp_ha.sa_data);
    sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
            (ptr[0] & 0377), (ptr[1] & 0377),
            (ptr[2] & 0377), (ptr[3] & 0377),
            (ptr[4] & 0377), (ptr[5] & 0377));
    reply = strdup(mac);
    printf("arp_get: Found IP %s, mac %s",req_ip, mac);

    return reply;
#else
    return NULL;
#endif
}
