/// \file
#ifndef SetNullAndDelete__H
#define SetNullAndDelete__H

#include <stdlib.h>

//
// Templates
//
namespace aRibeiro {

    /// \brief Helper function to release any pointer allocated with new operator.
    ///
    /// This function is usefull when you are working with threads and you make a NULL test of an instance.
    ///
    /// This function ensures that the variable will be set to NULL before the memory release occur.
    ///
    /// If you want to release the memory from an array allocated, you need to use the #setNullAndDeleteArray function.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2* vec2_instance = new vec2();
    ///
    /// ...
    ///
    /// setNullAndDelete( vec2_instance );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \tparam C type of element
    /// \param[out] obj the pointer to release
    ///
    template <typename C>
    void setNullAndDelete(C *&obj) {
        if (obj != NULL) {
            C * aux = obj;
            obj = NULL;
            delete aux;
        }
    }

    /// \brief Helper function to release any array pointer allocated with new[] operator.
    ///
    /// This function is usefull when you are working with threads and you make a NULL test of an instance.
    ///
    /// This function ensures that the variable will be set to NULL before the memory release occur.
    ///
    /// If you want to release the memory from a single element allocated, you need to use the #setNullAndDelete function.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// vec2* vec2_instance = new vec2[64];
    ///
    /// ...
    ///
    /// setNullAndDeleteArray( vec2_instance );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \tparam C type of element array
    /// \param[out] obj the pointer to release
    ///
    template <typename C>
    void setNullAndDeleteArray(C *&obj) {
        if (obj != NULL) {
            C * aux = obj;
            obj = NULL;
            delete[]aux;
        }
    }

}

#endif

