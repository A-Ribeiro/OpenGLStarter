#pragma once

#include "common.h"
#include "all.h"

class Scene : public View
{

public:
    static const ViewType Type;

    Scene();

    View* Init();

    void RenderAndLogic();
};