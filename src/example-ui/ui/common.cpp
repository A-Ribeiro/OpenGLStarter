#include "./common.h"

namespace ui
{

    const char *UIEventToStr(UIEventEnum v)
    {
        switch (v)
        {
        case UIEvent_InputUp:
            return "UIEvent_InputUp";
        case UIEvent_InputDown:
            return "UIEvent_InputDown";
        case UIEvent_InputLeft:
            return "UIEvent_InputLeft";
        case UIEvent_InputRight:
            return "UIEvent_InputRight";
        case UIEvent_InputShoulderLeft:
            return "UIEvent_InputShoulderLeft";
        case UIEvent_InputShoulderRight:
            return "UIEvent_InputShoulderRight";
        case UIEvent_InputActionEnter:
            return "UIEvent_InputActionEnter";
        case UIEvent_InputActionBack:
            return "UIEvent_InputActionBack";
        case UIEvent_ScreenPush:
            return "UIEvent_ScreenPush";
        case UIEvent_ScreenPop:
            return "UIEvent_ScreenPop";
        default:
            return "Unknown Event";
        }
    }

}