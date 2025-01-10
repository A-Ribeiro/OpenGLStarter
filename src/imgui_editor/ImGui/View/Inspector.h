#pragma once

#include "common.h"
#include "all.h"

namespace AppKit
{
    namespace GLEngine
    {
        class Component;
    }
}
class Inspector : public View
{
protected:

    std::vector< std::shared_ptr<InspectorImGuiComponent> > components;
    std::shared_ptr<AppKit::GLEngine::Transform> selectedNode;

    std::unordered_map<std::string, std::function<std::shared_ptr<InspectorImGuiComponent>(std::shared_ptr<AppKit::GLEngine::Component>)>> componentCreator;


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