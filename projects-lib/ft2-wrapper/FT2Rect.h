#ifndef ft2_rect__h__
#define ft2_rect__h__

#include <aribeiro/aribeiro.h>
#include <vector>

namespace ft2Wrapper {
    
    /// \brief FT2 wrapper rectangle
    ///
    /// It holds one character metrics information.
    ///
    /// \author Alessandro Ribeiro
    ///
    class FT2Rect{
    public:
        union{
            struct{int top,left,width,height;};
            struct{int x,y,w,h;};
        };
        FT2Rect();
        FT2Rect(int _left,int _top,int _width,int _height);
    };
    
}

#endif
