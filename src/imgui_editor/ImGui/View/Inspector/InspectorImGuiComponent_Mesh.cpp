#include "InspectorImGuiComponent_Mesh.h"
#include "../all.h"
#include "../../ImGuiManager.h"

#include <appkit-gl-engine/Components/ComponentMesh.h>

InspectorImGuiComponent_Mesh::InspectorImGuiComponent_Mesh(std::shared_ptr<AppKit::GLEngine::Components::ComponentMesh> _mesh):
    InspectorImGuiComponent("mesh") {

    mesh = _mesh;
}

void InspectorImGuiComponent_Mesh::childRenderAndLogic() {

    ImGui::Text("[Mesh]");

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

InspectorImGuiComponent_Mesh::~InspectorImGuiComponent_Mesh(){
    mesh = nullptr;
}
