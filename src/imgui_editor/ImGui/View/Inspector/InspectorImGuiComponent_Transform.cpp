#include "InspectorImGuiComponent_Transform.h"
#include "../all.h"
#include "../../ImGuiManager.h"

#include <appkit-gl-engine/Transform.h>

InspectorImGuiComponent_Transform::InspectorImGuiComponent_Transform(std::shared_ptr<AppKit::GLEngine::Transform> _transform):
    InspectorImGuiComponent("transform") {

    transform = _transform;
}

void InspectorImGuiComponent_Transform::childRenderAndLogic() {

    //ImGui::BeginGroup();

    pos = transform->getLocalPosition();
    quat = transform->getLocalRotation();
    scale = transform->getLocalScale();

    if (ImGui::DragFloat3Inverted("Pos", pos.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Pos Changed...\n");
        transform->setLocalPosition(pos);
    }

    if (ImGui::DragFloat4Inverted("Quat", quat.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Quat Changed...\n");
        transform->setLocalRotation(quat);
    }
    if (ImGui::DragFloat3Inverted("Scale", scale.array, 0.001f, 0, 0, "%.3f")) {
        //printf("Value Scale Changed...\n");
        transform->setLocalScale(scale);
    }

    //ImGui::EndGroup();
    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    //groupHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
    ImGui::Separator();

}

InspectorImGuiComponent_Transform::~InspectorImGuiComponent_Transform(){
    transform = nullptr;
}
