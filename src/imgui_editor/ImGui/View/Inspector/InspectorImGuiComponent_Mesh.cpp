#include "InspectorImGuiComponent_Mesh.h"
#include "../all.h"
#include "../../ImGuiManager.h"

#include <appkit-gl-engine/Components/Core/ComponentMesh.h>

InspectorImGuiComponent_Mesh::InspectorImGuiComponent_Mesh(std::shared_ptr<AppKit::GLEngine::Components::ComponentMesh> _mesh):
    InspectorImGuiComponent("mesh") {

    mesh = _mesh;
}

void InspectorImGuiComponent_Mesh::childRenderAndLogic() {

    ImGui::PushTextWrapPos();
    ImGui::Text("[%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s]",
        (mesh->pos.size())?"aPosition":"",
        (mesh->normals.size())?", aNormal":"",
        (mesh->tangent.size())?", aTangent":"",
        (mesh->binormal.size())?", aBinormal":"",

        (mesh->uv[0].size())?", aUV0":"",
        (mesh->uv[1].size())?", aUV1":"",
        (mesh->uv[2].size())?", aUV2":"",
        (mesh->uv[3].size())?", aUV3":"",
        (mesh->uv[4].size())?", aUV4":"",
        (mesh->uv[5].size())?", aUV5":"",
        (mesh->uv[6].size())?", aUV6":"",
        (mesh->uv[7].size())?", aUV7":"",

        (mesh->color[0].size())?", aColor0":"",
        (mesh->color[1].size())?", aColor1":"",
        (mesh->color[2].size())?", aColor2":"",
        (mesh->color[3].size())?", aColor3":"",
        (mesh->color[4].size())?", aColor4":"",
        (mesh->color[5].size())?", aColor5":"",
        (mesh->color[6].size())?", aColor6":"",
        (mesh->color[7].size())?", aColor7":"",

        (mesh->skin_index.size())?", aSkinIndex":"",
        (mesh->skin_weights.size())?", aSkinWeight":""
    );
    ImGui::PopTextWrapPos();

    ImGui::Text("Vertices count: %u", (uint32_t)mesh->pos.size());
    ImGui::Text("Indices count: %u", (uint32_t)mesh->indices.size());
    ImGui::Text("Bones count: %u", (uint32_t)mesh->bones.size());
    ImGui::Text("always_clone: %u", (uint32_t)mesh->always_clone);

    

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
