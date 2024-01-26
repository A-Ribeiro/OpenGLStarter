#pragma once

#include "common.h"
#include "all.h"

class Scene : public View
{

public:
    Scene();

    View* Init();

    void RenderAndLogic();
};