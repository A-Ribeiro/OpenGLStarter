#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

namespace AppKit
{

    namespace Window
    {

        struct VideoMode
        {
            uint32_t width;
            uint32_t height;
            uint32_t bitsPerPixel;

            // On MacOS (Catalina)
            //  -- bitsPerPixel must be 32... or it will not show any image on window.
            VideoMode(uint32_t width, uint32_t height, uint32_t bitsPerPixel = 32)
            {
                this->width = width;
                this->height = height;
                this->bitsPerPixel = bitsPerPixel;
            }

            VideoMode(const VideoMode &v)
            {
                *this = v;
            }

            VideoMode()
            {
                memset(this, 0, sizeof(VideoMode));
            }
        };

    }

}