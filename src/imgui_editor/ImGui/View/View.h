#pragma once

#include "common.h"
//#include "all.h"

class View {

    public:

    bool active;

    View();

    virtual View* Init();
    virtual void RenderAndLogic();
};