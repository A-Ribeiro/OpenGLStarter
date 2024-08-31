#pragma once

#include "common.h"
//#include "all.h"

typedef const char *ViewType;

class View {

    protected:

    EventCore::PressReleaseDetector on_hover_detector;
    EventCore::PressReleaseDetector on_focus_detector;

    void computeOnHoverAndOnFocus();

    public:

    ViewType type;
    
    bool active;

    View(ViewType type);

    virtual View* Init();
    virtual void RenderAndLogic();

    virtual void Finalize() {
    }
};