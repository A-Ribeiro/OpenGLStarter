#include "InspectorImGuiComponent_Transform.h"
#include "../all.h"
#include "../../ImGuiManager.h"

#include <appkit-gl-engine/Transform.h>

InspectorImGuiComponent_Transform::InspectorImGuiComponent_Transform(std::shared_ptr<AppKit::GLEngine::Transform> transform):
    InspectorImGuiComponent("transform") {

    transform_to_monitor = transform;
}

void InspectorImGuiComponent_Transform::childRenderAndLogic() {

    //ImGui::BeginGroup();

    pos = transform_to_monitor->getLocalPosition();
    quat = transform_to_monitor->getLocalRotation();
    scale = transform_to_monitor->getLocalScale();

    if (ImGui::DragFloat3Inverted("Pos", pos.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Pos Changed...\n");
        transform_to_monitor->setLocalPosition(pos);
    }

    if (ImGui::DragFloat4Inverted("Quat", quat.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Quat Changed...\n");
        transform_to_monitor->setLocalRotation(quat);
    }
    if (ImGui::DragFloat3Inverted("Scale", scale.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Scale Changed...\n");
        transform_to_monitor->setLocalScale(scale);
    }

    //ImGui::EndGroup();
    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    //groupHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
    ImGui::Separator();

}

InspectorImGuiComponent_Transform::~InspectorImGuiComponent_Transform(){
    transform_to_monitor = nullptr;
}