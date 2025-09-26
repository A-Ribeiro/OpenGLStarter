#include "./Pallete.h"

namespace ui
{

    namespace Pallete
    {
        const ColorPalette Blush{
            3.0f * 0,                      // float stroke_thickness;

            colorFromHex("#9e9ed9ff"), // Color bg;
            colorFromHex("#bbbbff", 1.0f), // Color primary;
            colorFromHex("#610081", 1.0f), // Color primary_stroke;
            colorFromHex("#6192d1", 1.0f), // Color active;
            colorFromHex("#003780", 1.0f), // Color active_stroke;
            colorFromHex("#dedede", 1.0f), // Color disabled;
            colorFromHex("#404040", 1.0f), // Color disabled_stroke;
            colorFromHex("#000000", 1.0f), // Color text;
            colorFromHex("#6666cc", 0.8f)  // Color scroll_gradient
        };
    }

}
