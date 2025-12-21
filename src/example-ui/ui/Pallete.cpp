#include "./Pallete.h"

namespace ui
{

    namespace Pallete
    {
        const ColorPalette Blush{
            7.0f,                          // float avatar_stroke_thickness;
            0.0f,                          // float dialog_stroke_thickness;
            0.0f,                          // float options_stroke_thickness;
            0.0f,                          // float messagebox_stroke_thickness;
            0.0f,                          // float button_stroke_thickness;
            colorFromHex("#8e8ec2ff", 1.0f), // Color bg;
            colorFromHex("#6c6c92ff", 1.0f), // Color bg_stroke
            colorFromHex("#bbbbff", 1.0f), // Color primary;
            colorFromHex("#610081", 1.0f), // Color primary_stroke;
            colorFromHex("#6192d1", 1.0f), // Color active;
            colorFromHex("#003780", 1.0f), // Color active_stroke;
            colorFromHex("#dedede", 1.0f), // Color disabled;
            colorFromHex("#404040", 1.0f), // Color disabled_stroke;
            colorFromHex("#000000", 1.0f), // Color text;
            colorFromHex("#666666", 1.0f), // Color text_disabled
            colorFromHex("#6666cc", 0.8f)  // Color scroll_gradient
        };

        const ColorPalette Purple{
            7.0f,                          // float avatar_stroke_thickness;
            0.0f,                          // float dialog_stroke_thickness;
            0.0f,                          // float options_stroke_thickness;
            0.0f,                          // float messagebox_stroke_thickness;
            0.0f,                          // float button_stroke_thickness;
            colorFromHex("#7c3aed", 1.0f), // bg
            colorFromHex("#5729a7ff", 1.0f), // bg_stroke
            colorFromHex("#a78bfa", 1.0f), // primary
            colorFromHex("#6d28d9", 1.0f), // primary_stroke
            colorFromHex("#c4b5fd", 1.0f), // active
            colorFromHex("#4c1d95", 1.0f), // active_stroke
            colorFromHex("#ede9fe", 1.0f), // disabled
            colorFromHex("#a3a3a3", 1.0f), // disabled_stroke
            colorFromHex("#f3f4f6", 1.0f), // text
            colorFromHex("#a3a3a3", 1.0f), // text_disabled
            colorFromHex("#a78bfa", 0.8f)  // scroll_gradient
        };

        const ColorPalette Orange{
            7.0f,                          // float avatar_stroke_thickness;
            0.0f,                          // float dialog_stroke_thickness;
            0.0f,                          // float options_stroke_thickness;
            0.0f,                          // float messagebox_stroke_thickness;
            0.0f,                          // float button_stroke_thickness;
            colorFromHex("#fb923c", 1.0f), // bg
            colorFromHex("#bc6c2aff", 1.0f), // bg_stroke
            colorFromHex("#fdba74", 1.0f), // primary
            colorFromHex("#ea580c", 1.0f), // primary_stroke
            colorFromHex("#ffedd5", 1.0f), // active
            colorFromHex("#c2410c", 1.0f), // active_stroke
            colorFromHex("#fff7ed", 1.0f), // disabled
            colorFromHex("#a3a3a3", 1.0f), // disabled_stroke
            colorFromHex("#1c1917", 1.0f), // text
            colorFromHex("#666666", 1.0f), // text_disabled
            colorFromHex("#fdba74", 0.8f)  // scroll_gradient
        };

        const ColorPalette Green{
            7.0f,                           // float avatar_stroke_thickness;
            0.0f,                           // float dialog_stroke_thickness;
            0.0f,                           // float options_stroke_thickness;
            0.0f,                           // float messagebox_stroke_thickness;
            0.0f,                           // float button_stroke_thickness;
            colorFromHex("#22c55e", 1.0f),  // bg
            colorFromHex("#199246ff", 1.0f),  // bg_stroke
            colorFromHex("#4ade80", 1.0f),  // primary
            colorFromHex("#16a34a", 1.0f),  // primary_stroke
            colorFromHex("#bbf7d0", 1.0f),  // active
            colorFromHex("#166534", 1.0f),  // active_stroke
            colorFromHex("#f0fdf4", 1.0f),  // disabled
            colorFromHex("#a3a3a3", 1.0f),  // disabled_stroke
            colorFromHex("#1c1917", 1.0f),  // text
            colorFromHex("#666666", 1.0f),  // text_disabled
            colorFromHex("#8effb7ff", 0.8f) // scroll_gradient
        };

        const ColorPalette Blue{
            7.0f,                          // float avatar_stroke_thickness;
            0.0f,                          // float dialog_stroke_thickness;
            0.0f,                          // float options_stroke_thickness;
            0.0f,                          // float messagebox_stroke_thickness;
            0.0f,                          // float button_stroke_thickness;
            colorFromHex("#1e40af"),     // bg
            colorFromHex("#152d7bff", 1.0f), // bg_stroke
            colorFromHex("#60a5fa", 1.0f), // primary
            colorFromHex("#1d4ed8", 1.0f), // primary_stroke
            colorFromHex("#dbeafe", 1.0f), // active
            colorFromHex("#1e40af", 1.0f), // active_stroke
            colorFromHex("#eff6ff", 1.0f), // disabled
            colorFromHex("#a3a3a3", 1.0f), // disabled_stroke
            colorFromHex("#f3f4f6", 1.0f), // text
            colorFromHex("#a3a3a3", 1.0f), // text_disabled
            colorFromHex("#60a5fa", 0.8f)  // scroll_gradient
        };

        const ColorPalette Dark{
            7.0f,                          // float avatar_stroke_thickness;
            0.0f,                          // float dialog_stroke_thickness;
            0.0f,                          // float options_stroke_thickness;
            0.0f,                          // float messagebox_stroke_thickness;
            0.0f,                          // float button_stroke_thickness;
            colorFromHex("#202020ff"),     // Color bg;
            colorFromHex("#444444", 1.0f), // Color bg_stroke
            colorFromHex("#444444", 1.0f), // Color primary;
            colorFromHex("#888888", 1.0f), // Color primary_stroke;
            colorFromHex("#666666", 1.0f), // Color active;
            colorFromHex("#aaaaaa", 1.0f), // Color active_stroke;
            colorFromHex("#303030", 1.0f), // Color disabled;
            colorFromHex("#505050", 1.0f), // Color disabled_stroke;
            colorFromHex("#e0e0e0", 1.0f), // Color text;
            colorFromHex("#a3a3a3", 1.0f), // text_disabled
            colorFromHex("#444444", 0.8f)  // Color scroll_gradient
        };

    }

}
