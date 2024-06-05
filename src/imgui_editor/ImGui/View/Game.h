#pragma once

#include "common.h"
#include "all.h"

class Game : public View
{

public:
    static const ViewType Type;

    Game();

    View* Init();

    void RenderAndLogic();
};