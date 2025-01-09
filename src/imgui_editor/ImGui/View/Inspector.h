#pragma once

#include "common.h"
#include "all.h"

class Inspector : public View
{
protected:

    std::vector< std::shared_ptr<InspectorImGuiComponent> > components;
    std::shared_ptr<AppKit::GLEngine::Transform> selectedNode;

public:
    static const ViewType Type;

    void addComponent(std::shared_ptr<InspectorImGuiComponent> v);
    void removeComponent(std::shared_ptr<InspectorImGuiComponent> v);
    void clearComponents();

    void setSelectedNode(std::shared_ptr<AppKit::GLEngine::Transform> t);

    Inspector();
    ~Inspector();

    View* Init();

    void RenderAndLogic();
};