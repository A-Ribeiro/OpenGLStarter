////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
//
// Raspberry Pi dispmanx implementation
// Copyright (C) 2016 Andrew Mickelson
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/VideoModeImpl.hpp>
#include <bcm_host.h>
#include <stdio.h>

namespace sf
{
namespace priv
{

static int GCD(int a, int b)
{
    int Remainder;

    while( b != 0 )
    {
        Remainder = a % b;
        a = b;
        b = Remainder;
    }

    return a;
}

////////////////////////////////////////////////////////////
std::vector<VideoMode> VideoModeImpl::getFullscreenModes()
{
    std::vector<VideoMode> modes;
    modes.push_back(getDesktopMode());

    int gcd = GCD(modes[0].width,modes[0].height);
    int aspectx = modes[0].width/gcd;
    int aspecty = modes[0].height/gcd;

    printf("[VideoModeImpl] aspect: %i:%i\n", aspectx, aspecty);
    printf("[VideoModeImpl] Fullscreen Resolutions:\n");

    VideoMode vm = modes[0];
    if (aspectx == 4 && aspecty == 3)
    {
        int res[] =
        {
            6400, 4800,
            4096, 3072,
            3200, 2400,
            2800, 2100,
            //2732, 2048,
            2560, 1920,
            2304, 1728,
            2048, 1536,
            1920, 1440,
            1856, 1392,
            1792, 1344,
            1600, 1200,
            1440, 1080,
            1400, 1050,
            1280, 960,
            1152, 864,
            //1120, 832,
            1024, 768,
            960, 720,
            832, 624,
            800, 600,
            640, 480,
            512, 384,
            400, 300,
            384, 288,
            320, 240,
            256, 192,
            160, 120
        };

        printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
        for(int i=0; i < sizeof(res)/sizeof(int) ; i+=2)
        {
            vm.width = res[i];
            vm.height = res[i+1];
            if (vm.width < modes[modes.size()-1].width){
                modes.push_back(vm);
                printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            }
        }
    }
    else if (aspectx == 16 && aspecty == 9)
    {
        //https://en.wikipedia.org/wiki/16:9_aspect_ratio
        int res[] =
        {
            7680, 4320, // 8K UHD
            5120, 2880, // 5k
            4096, 2304,
            3840, 2160, // 4K UHD
            3200, 1800, //QHD+
            2880, 1620,
            2560, 1440, // QHD
            2048, 1152,
            1920, 1080, // fullhd
            1600, 900, // HD+
            //1366, 768, // WXGA
            1280, 720, // HD
            1024, 576,
            960, 540, // qHD
            //848, 480,
            800, 450,
            768, 432,
            640, 360, // nHD
            //426, 240,
            256, 144 // YouTube 144p
        };

        printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
        for(int i=0; i < sizeof(res)/sizeof(int) ; i+=2)
        {
            vm.width = res[i];
            vm.height = res[i+1];
            if (vm.width < modes[modes.size()-1].width){
                modes.push_back(vm);
                printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            }
        }
    }
    else if (aspectx == 8 && aspecty == 5) // 16:10
    {
        //https://en.wikipedia.org/wiki/16:10_aspect_ratio
        int res[] =
        {
            2560, 1600, // WQXGA
            1920, 1200, // WUXGA
            1680, 1050, // WSXGA+
            1440, 900, // WXGA+
            1280, 800, // WXGA
            1024, 640,
            640, 400,
            320, 200,
            160, 100
        };

        printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
        for(int i=0; i < sizeof(res)/sizeof(int) ; i+=2)
        {
            vm.width = res[i];
            vm.height = res[i+1];
            if (vm.width < modes[modes.size()-1].width){
                modes.push_back(vm);
                printf("[VideoModeImpl]   %i:%i\n", vm.width, vm.height);
            }
        }
    }


    return modes;
}


////////////////////////////////////////////////////////////
VideoMode VideoModeImpl::getDesktopMode()
{
    static bool initialized=false;

    if (!initialized)
    {
        bcm_host_init();
        initialized=true;
    }

    uint32_t width( 0 ), height( 0 );
    graphics_get_display_size( 0 /* LCD */, &width, &height );
    return VideoMode(width, height);
}

} // namespace priv

} // namespace sf
