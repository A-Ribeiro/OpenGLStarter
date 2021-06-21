#include "PressReleaseDetector.h"

namespace aRibeiro {

    PressReleaseDetector::PressReleaseDetector() {
        down = false;
        up = false;
        pressed = false;
    }

    void PressReleaseDetector::setState(bool v) {
        down = !pressed && v;
        up = pressed && !v;
        pressed = v;
        if (down)
            OnDown();
        if (up)
            OnUp();
    }

}
