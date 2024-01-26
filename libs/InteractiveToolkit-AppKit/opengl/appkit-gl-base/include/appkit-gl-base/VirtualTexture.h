#pragma once

namespace AppKit
{

    namespace OpenGL
    {


    class VirtualTexture {

    public:
        virtual void active(int id) const = 0;
        virtual void deactive(int id) const = 0;
    };

}

}