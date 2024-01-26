#pragma once

#include "common.h"
#include "all.h"

class Game : public View
{

public:
    Game();

    View* Init();

    void RenderAndLogic();
};