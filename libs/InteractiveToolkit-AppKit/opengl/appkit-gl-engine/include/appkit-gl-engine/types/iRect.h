#pragma once

#include <string.h> //memcmp

#include <InteractiveToolkit/MathCore/MathCore.h>

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

            template <typename Tx, typename Ty,
                      typename std::enable_if<
                          (std::is_integral<Tx>::value || std::is_floating_point<Tx>::value) &&
                              (std::is_integral<Ty>::value || std::is_floating_point<Ty>::value),
                          bool>::type = true>
            bool isInside(Tx x, Ty y) const
            {
                return x >= (Tx)0 && x < (Tx)w && y >= (Ty)0 && y < (Ty)h;
            }

            template <typename Tvec,
                      typename std::enable_if<
                          MathCore::MathTypeInfo<Tvec>::_is_vec::value,
                          bool>::type = true>
            bool isInside(Tvec vec) const
            {
                using _f_type = typename MathCore::MathTypeInfo<Tvec>::_type;
                return vec.x >= (_f_type)0 && vec.x < (_f_type)w && vec.y >= (_f_type)0 && vec.y < (_f_type)h;
            }

        };

    }

}