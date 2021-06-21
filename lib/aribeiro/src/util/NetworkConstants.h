/// \file
#ifndef NETWORK_CONSTANTS_H
#define NETWORK_CONSTANTS_H

/*
#ifndef byte
    typedef unsigned char byte;
#endif

#ifndef uint
    typedef unsigned int uint;
#endif

#ifndef ushort
    typedef unsigned short ushort;
#endif
    */

namespace aRibeiro
{
    /// \brief Define some constants to help network programming
    ///
    /// The majority constants are related to UDP conection
    ///
    /// \author Alessandro Ribeiro
    ///
    namespace NetworkConstants
    {
    
        const int UDP_MAX_PACKET_SIZE = 0xffff;///< UDP max packet size in bytes (2bytes = 65535)
        const int UDP_HEADER_SIZE = 8;///< UDP RAW header size bytes
        const int IP_V4_HEADER_SIZE = 20;///< IPv4 RAW header size bytes
        const int UDP_IPV4_TOTAL_HEADER_SIZE = IP_V4_HEADER_SIZE + UDP_HEADER_SIZE;///< Header size in bytes when using UDP over IPv4

        //Known MTUs (Maximum transfer units)

        // Ethernet: 1500 - UDP_IPV4_TOTAL_HEADER_SIZE = 1472
        // test: ping 10.0.0.105 -f -l 1472
        const int ETHERNET_MTU = 1500;///< Ethernet maximum transfer unit (bytes). Packet greater than this is fragmented.

        // Internet: 1492 - UDP_IPV4_TOTAL_HEADER_SIZE = 1464
        // test: ping www.tp-link.com -f -l 1464
        // test: ping google.com -f -l 1464
        const int INTERNET_MTU = 1492;///< Internet maximum transfer unit (bytes). Packet greater than this is fragmented.

        // Minimum packet size: 576 - UDP_IPV4_TOTAL_HEADER_SIZE = 548
        // StackOverflow - Safest to transmit
        const int MINIMUM_MTU = 576;///< Minimum transfer unit (bytes). Used for packet lesser or equal this.

        //generates fragmentation
        const int UDP_DATA_MAX_DATAGRAM_SIZE = UDP_MAX_PACKET_SIZE - UDP_IPV4_TOTAL_HEADER_SIZE;///< UDP max user data packet size (bytes).

        const int UDP_DATA_MTU_ETHERNET = ETHERNET_MTU - UDP_IPV4_TOTAL_HEADER_SIZE;///< UDP user data packet size (bytes) to use in an Ethernet connection.
        const int UDP_DATA_MTU_INTERNET = INTERNET_MTU - UDP_IPV4_TOTAL_HEADER_SIZE;///< UDP user data packet size (bytes) to use in an Internet connection.
        const int UDP_DATA_MTU_MINIMUM = MINIMUM_MTU - UDP_IPV4_TOTAL_HEADER_SIZE;///< UDP user data packet size (bytes) to use as minimum packet size.

        const int UDP_DATA_MTU_LOCALHOST = UDP_DATA_MAX_DATAGRAM_SIZE;///< UDP user data packet size (bytes) to use in local connection.


        // SELECTED MTU TO WORK
        //const int UDP_DATA_MAXIMUM_MTU = UDP_DATA_MTU_INTERNET;
        //const int UDP_DATA_MINIMUM_MTU = UDP_DATA_MTU_MINIMUM;

        // try to avoid overlap with the OS ephemeral port 
        const int PUBLIC_PORT_START = 5001;///< Public port mapping start for unicast to avoid use OS ephemeral port 
        const int PUBLIC_PORT_END = 18884;///< Public port mapping end for unicast to avoid use OS ephemeral port 
        const int PUBLIC_PORT_MULTICAST_START = PUBLIC_PORT_END + 1;///< Public port mapping start for multicasting to avoid use OS ephemeral port 
        const int PUBLIC_PORT_MULTICAST_END = 32767;///< Public port mapping end for multicasting to avoid use OS ephemeral port 

    }
}

#endif
