#pragma once

#include <string.h> //memcmp

namespace AppKit
{
    namespace GLEngine
    {

        class iRect
        {
        public:
            int x, y, w, h;

            iRect(const iRect &v)
            {
                (*this) = v;
            }

            void operator=(const iRect &v)
            {
                x = v.x;
                y = v.y;
                w = v.w;
                h = v.h;
            }

            bool operator==(const iRect &v) const
            {
                return memcmp(this, &v, sizeof(iRect)) == 0;
            }

            bool operator!=(const iRect &v) const
            {
                return memcmp(this, &v, sizeof(iRect)) != 0;
            }

            iRect()
            {
                x = y = w = h = 0;
            }
            iRect(int w, int h)
            {
                this->x = 0;
                this->y = 0;
                this->w = w;
                this->h = h;
            }
            iRect(int x, int y, int w, int h)
            {
                this->x = x;
                this->y = y;
                this->w = w;
                this->h = h;
            }
        };

    }

}