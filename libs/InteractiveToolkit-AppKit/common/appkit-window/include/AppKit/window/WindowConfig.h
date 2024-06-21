#pragma once

#include "buildFlags.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

#include "VideoMode.h"

namespace AppKit
{

    namespace Window
    {

        enum class WindowStyle : uint8_t
        {
            Borderless = 0,
            Default,
            FullScreen
        };

        struct WindowConfig
        {
            // Window Settings
            char windowName[256];
            WindowStyle windowStyle;
            VideoMode videoMode;

            WindowConfig( // Window Settings
                const char *windowName,
                WindowStyle windowStyle,
                const VideoMode &videoMode)
            {
                memset(this, 0, sizeof(WindowConfig));

                snprintf(this->windowName, 256, "%s", windowName);

                this->windowStyle = windowStyle;
                this->videoMode = videoMode;
            }

            WindowConfig()
            {
                memset(this, 0, sizeof(WindowConfig));
            }

            WindowConfig(const WindowConfig &v)
            {
                (*this) = v;
            }
        };

#if defined(APPKIT_WINDOW_GL)
        struct GLContextConfig
        {
            bool vSync;

            // GL Context Settings
            uint32_t depthBits;
            uint32_t stencilBits;
            uint32_t antialiasingLevel;
            bool sRgbCapable;

            bool coreAttribute;
            uint32_t majorVersion;
            uint32_t minorVersion;

            GLContextConfig( 
                bool vSync,

                // GL Context Settings
                uint32_t depthBits,
                uint32_t stencilBits,
                uint32_t antialiasingLevel,
                bool sRgbCapable,

                bool coreAttribute,
                uint32_t majorVersion,
                uint32_t minorVersion)
            {

                memset(this, 0, sizeof(GLContextConfig));

                this->vSync = vSync;

                // GL Context Settings
                this->depthBits = depthBits;
                this->stencilBits = stencilBits;
                this->antialiasingLevel = antialiasingLevel;
                this->sRgbCapable = sRgbCapable;

                this->coreAttribute = coreAttribute;
                this->majorVersion = majorVersion;
                this->minorVersion = minorVersion;
            }

            GLContextConfig()
            {
                memset(this, 0, sizeof(GLContextConfig));
            }

            GLContextConfig(const GLContextConfig &v)
            {
                (*this) = v;
            }
        };

#endif

    }
}
