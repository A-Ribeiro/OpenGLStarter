#pragma once

#include "common.h"
#include "all.h"

class Inspector : public View
{
    std::vector< InspectorImGuiComponent* > components;
public:

    void addComponent(InspectorImGuiComponent*);
    void removeComponent(InspectorImGuiComponent*);
    void clearComponents();

    Inspector();
    ~Inspector();


    View* Init();

    void RenderAndLogic();
};