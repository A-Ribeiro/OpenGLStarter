#include "InspectorImGuiComponent_MeshWrapper.h"
#include "../all.h"
#include "../../ImGuiManager.h"

#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

InspectorImGuiComponent_MeshWrapper::InspectorImGuiComponent_MeshWrapper(std::shared_ptr<AppKit::GLEngine::Components::ComponentMeshWrapper> _mesh_wrapper):
    InspectorImGuiComponent("mesh wrapper") {

    mesh_wrapper = _mesh_wrapper;
}

void InspectorImGuiComponent_MeshWrapper::childRenderAndLogic() {

    ImGui::Text("[MeshWrapper]");

    //ImGui::BeginGroup();

    // pos = transform_to_monitor->getLocalPosition();
    // quat = transform_to_monitor->getLocalRotation();
    // scale = transform_to_monitor->getLocalScale();

    // if (ImGui::DragFloat3Inverted("Pos", pos.array, 0.001f, 0, 0, "%.3f")) {
    //     //printf("Value Pos Changed...\n");
    //     transform_to_monitor->setLocalPosition(pos);
    // }

    // if (ImGui::DragFloat4Inverted("Quat", quat.array, 0.001f, 0, 0, "%.3f")) {
    //     //printf("Value Quat Changed...\n");
    //     transform_to_monitor->setLocalRotation(quat);
    // }
    // if (ImGui::DragFloat3Inverted("Scale", scale.array, 0.001f, 0, 0, "%.3f")) {
    //     //printf("Value Scale Changed...\n");
    //     transform_to_monitor->setLocalScale(scale);
    // }

    //ImGui::EndGroup();
    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    //groupHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
    ImGui::Separator();

}

InspectorImGuiComponent_MeshWrapper::~InspectorImGuiComponent_MeshWrapper(){
    mesh_wrapper = nullptr;
}
