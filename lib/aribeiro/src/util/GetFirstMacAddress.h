/// \file
#ifndef GET_FIRST_MAC_ADDRESS__H
#define GET_FIRST_MAC_ADDRESS__H

#include <aribeiro/common.h>
#include <vector>

namespace aRibeiro {

    /// \brief Query the MAC (Media Access Control) From the First Network Device Installed in the Computer
    ///
    /// This function could return an 6 bytes hardware address format.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// // result could be: 0xa0, 0xb1, 0xc2, 0xd3, 0xe4, 0xf5
    /// std::vector<unsigned char> mac = getFirstMacAddress();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    std::vector<unsigned char> getFirstMacAddress();

}


#endif
