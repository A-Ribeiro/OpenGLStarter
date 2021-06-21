#include "GetFirstMacAddress.h"

#include <aribeiro/common.h>

#include <vector>

#ifdef OS_TARGET_win

#include <stdlib.h>
#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <mmsystem.h>

#include <Shlwapi.h> //shlwapi.lib
#include <Iphlpapi.h> //iphlpapi.lib

namespace aRibeiro {
    std::vector<unsigned char> getFirstMacAddress() {
        PIP_ADAPTER_INFO AdapterInfo;
        DWORD dwBufLen = sizeof(AdapterInfo);

        std::vector<unsigned char> mac_addr;

        AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));

        // Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
        if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
            free(AdapterInfo);
            AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
        }

        if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
            PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
            do {
                if (pAdapterInfo->AddressLength == 6) {
                    mac_addr.resize(pAdapterInfo->AddressLength);
                    memcpy(&mac_addr[0], pAdapterInfo->Address, pAdapterInfo->AddressLength);

                    printf("[%s:%i]\n", __FILE__, __LINE__);
                    printf("Found HW Address: ");
                    printf("%02X:%02X:%02X:%02X:%02X:%02X of: ",
                        pAdapterInfo->Address[0], pAdapterInfo->Address[1],
                        pAdapterInfo->Address[2], pAdapterInfo->Address[3],
                        pAdapterInfo->Address[4], pAdapterInfo->Address[5]);

                    printf("%s", pAdapterInfo->IpAddressList.IpAddress.String);
                    printf("\n");

                    break;
                }

                pAdapterInfo = pAdapterInfo->Next;
            } while (pAdapterInfo);
        }

        free(AdapterInfo);
        return mac_addr;
    }
}


#else


#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <net/if_dl.h>
#include <ifaddrs.h>

#include <string.h>

namespace aRibeiro {
    #define LLADDR

    std::vector<unsigned char> getFirstMacAddress() {

        std::vector<unsigned char> mac_addr;

        struct ifaddrs *ifap, *ifaptr;
        unsigned char *ptr;

        if (getifaddrs(&ifap) == 0) {
            for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
                if (((ifaptr)->ifa_addr)->sa_family == AF_INET) {

                int len = strlen((ifaptr)->ifa_name);

                if (len > 2)
                    len = 2;

                if (memcmp((ifaptr)->ifa_name, "lo", len ) == 0)
                    continue;

                    ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);

                    mac_addr.resize(6);
                    memcpy(&mac_addr[0], ptr, 6);

                    printf("[%s:%i]\n", __FILE__, __LINE__);
                    printf("%s: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                        (ifaptr)->ifa_name,
                                        *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));

                    break;

                }
            }
            freeifaddrs(ifap);
        }

        return mac_addr;
    }

}
#endif
