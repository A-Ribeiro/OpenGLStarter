#include "Inspector.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const ViewType Inspector::Type = "Inspector";

Inspector::Inspector() : View(Inspector::Type)
{
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
