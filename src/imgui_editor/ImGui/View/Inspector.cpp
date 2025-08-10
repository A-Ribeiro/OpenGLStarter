#include "Inspector.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

const ViewType Inspector::Type = "Inspector";

Inspector::Inspector() : View(Inspector::Type)
{

    componentCreator[ AppKit::GLEngine::Components::ComponentMaterial::Type ] = [](std::shared_ptr<AppKit::GLEngine::Component> _comp){
        return InspectorImGuiComponent::CreateShared<InspectorImGuiComponent_Material>( std::dynamic_pointer_cast<AppKit::GLEngine::Components::ComponentMaterial>(_comp) );
    };

    componentCreator[ AppKit::GLEngine::Components::ComponentMesh::Type ] = [](std::shared_ptr<AppKit::GLEngine::Component> _comp){
        return InspectorImGuiComponent::CreateShared<InspectorImGuiComponent_Mesh>( std::dynamic_pointer_cast<AppKit::GLEngine::Components::ComponentMesh>(_comp) );
    };

    componentCreator[ AppKit::GLEngine::Components::ComponentMeshWrapper::Type ] = [](std::shared_ptr<AppKit::GLEngine::Component> _comp){
        return InspectorImGuiComponent::CreateShared<InspectorImGuiComponent_MeshWrapper>( std::dynamic_pointer_cast<AppKit::GLEngine::Components::ComponentMeshWrapper>(_comp) );
    };


}
Inspector::~Inspector()
{
    clearComponents();
}

void Inspector::addComponent(std::shared_ptr<InspectorImGuiComponent> v)
{
    components.push_back(v);
}
void Inspector::removeComponent(std::shared_ptr<InspectorImGuiComponent> v)
{
    for (auto it = components.begin(); it != components.end(); it++)
    {
        if ((*it) == v)
        {
            components.erase(it);
            return;
        }
    }
}
void Inspector::clearComponents()
{
    // for (auto &v : components)
    //     if (v != nullptr)
    //         delete v;
    components.clear();
}

void Inspector::setSelectedNode(std::shared_ptr<AppKit::GLEngine::Transform> t) {
    if (selectedNode == t)
        return;
    selectedNode = t;

    clearComponents();
    
    if (selectedNode == nullptr)
        return;
    
    addComponent(InspectorImGuiComponent::CreateShared<InspectorImGuiComponent_Transform>(selectedNode));

    for (auto &item: selectedNode->getComponents()){
        auto it = componentCreator.find(item->getType());
        if (it == componentCreator.end()){
            auto gui_component = InspectorImGuiComponent::CreateShared<InspectorImGuiComponent_Unknown>( item );
            addComponent(gui_component);
        } else {
            addComponent(it->second(item));
        }
    }


}

View *Inspector::Init()
{
    // addComponent(new InspectorImGuiComponent_Transform());
    // addComponent(new InspectorImGuiComponent_Transform());

    ImGuiMenu::Instance()->AddMenu(
        0,
        "Window/Inspector", "", [this]()
        { printf("Window/Inspector\n"); },
        &this->active);
    return this;
}

void Inspector::RenderAndLogic()
{
    if (!active)
        return;
    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Inspector", nullptr, flags))
    {
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

        // check component state update

        for (int i = 0; i < (int)components.size(); i++)
            components[i]->renderAndLogic(i);
    }
    else
    {
        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }
    ImGui::End();
    ImGui::PopStyleVar();

    computeOnHoverAndOnFocus();
}
