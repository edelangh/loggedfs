
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include "utils.h"
#include <stdlib.h>

// From: http://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program

char    *get_mac_addr(char *buffer)
{
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { return (NULL); };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { return (NULL); }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        else { return (NULL); }
    }

    unsigned char   mac_address[6];
    char            *mac_str = (buffer ? buffer : (char*)malloc(19));

    if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
    snprintf(mac_str, 19, "%2X:%2X:%2X:%2X:%2X:%2X",
            mac_address[0], mac_address[1], mac_address[2],
            mac_address[3], mac_address[4], mac_address[5]);
    return mac_str;

}
