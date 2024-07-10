#include "Project.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

TreeDataType FileTreeData::Type = "FileTreeData";
ListDataType FileListData::Type = "FileListData";

bool drawFile(int id, const char *name, bool *selected, IconType icon, ImVec2 size);

const ViewType Project::Type = "Project";

Project::Project() : View(Project::Type)
{
    force_tree_selection = 0;
    uid_incrementer = 1;

    clear_tree_selection = ProjectClearMethod::None;
    clear_list_selection = ProjectClearMethod::None;


    // files.push_back(FileRef(FileRefType::File, "Very Big Name File.jpg"));
    // files.push_back(FileRef(FileRefType::File, "Another File.png"));
    // files.push_back(FileRef(FileRefType::File, "FileB.png"));
    // files.push_back(FileRef(FileRefType::Folder, "SomeFolder"));
    // files.push_back(FileRef(FileRefType::Folder, "AnotherFolder"));
    // files.push_back(FileRef(FileRefType::EmptyFolder, "SomeEmpty"));
}

View *Project::Init()
{
    visualList.Init();
    visualList.setPrefixID("FileList").
        setDragPayloadID(DRAG_PAYLOAD_ID_PROJECT_LIST).
        setDropPayload({
            DRAG_PAYLOAD_ID_HIERARCHY_TREE
            //,DRAG_PAYLOAD_ID_PROJECT_TREE
        });
    
    visualList.addItem("testa.png",nullptr);
    visualList.addItem("testb.png",nullptr);
    visualList.addItem("testc.png",nullptr);
    visualList.addItem("testd.png",nullptr);
    visualList.addItem("teste.png",nullptr);

    // creating testing node
    self_root = TreeNode::CreateShared(uid_incrementer++, nullptr, "-empty-");
    root = TreeNode::CreateShared(uid_incrementer++, 
        FileTreeData::CreateShared( ITKCommon::FileSystem::File{} ), 
        "/"
    );
    self_root->addChild(root);
    
    root->setIsRoot(true).
        setPrefixID("HierarchyTree").
        setDragPayloadID(DRAG_PAYLOAD_ID_PROJECT_TREE).
        setDropPayload({
            DRAG_PAYLOAD_ID_HIERARCHY_TREE,
            DRAG_PAYLOAD_ID_PROJECT_TREE
        });

    root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child1"));
    root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child2"));
    root->children.back()->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child2leaf"));
    root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child3"));

    ImGuiMenu::Instance()->AddMenu(
        0,
        "Window/Project", "", [this]()
        { printf("Window/Project\n"); },
        &this->active);

    // debug
    {
        OnTreeHover.add([](std::shared_ptr<TreeNode> node, bool hovered)
                    { printf("[Project][Tree] OnHover on %s: %i\n", node->getName(), hovered); });
        OnTreeSingleClick.add([](std::shared_ptr<TreeNode> node)
                          { printf("[Project][Tree] OnSingleClick on %s\n", node->getName()); });
        OnTreeDoubleClick.add([](std::shared_ptr<TreeNode> node)
                          { printf("[Project][Tree] OnDoubleClick on %s\n", node->getName()); });
        OnTreeExpand.add([](std::shared_ptr<TreeNode> node)
                     { printf("[Project][Tree] OnExpand on %s\n", node->getName()); });
        OnTreeCollapse.add([](std::shared_ptr<TreeNode> node)
                       { printf("[Project][Tree] OnCollapse on %s\n", node->getName()); });
        OnTreeSelect.add([](std::shared_ptr<TreeNode> node)
                     {
            if (node == NULL) {
                printf("[Project][Tree] OnSelect on NULL\n");
            } else {
                printf("[Project][Tree] OnSelect on %s\n", node->getName());
            } });
        OnTreeDragDrop.add([](const char* drag_payload, void *src, std::shared_ptr<TreeNode> target){
            printf("[Project][Tree] OnTreeDragDrop. drag_payload: %s\n", drag_payload);
        });
    }

    // debug
    {
        OnListHover.add([](std::shared_ptr<ListElement> node, bool hovered)
                    { printf("[Project][List] OnHover on %s: %i\n", node->getName(), hovered); });
        OnListSingleClick.add([](std::shared_ptr<ListElement> node)
                          { printf("[Project][List] OnSingleClick on %s\n", node->getName()); });
        OnListDoubleClick.add([](std::shared_ptr<ListElement> node)
                          { printf("[Project][List] OnDoubleClick on %s\n", node->getName()); });
        OnListSelect.add([](std::shared_ptr<ListElement> node)
                     {
            if (node == NULL) {
                printf("[Project][List] OnSelect on NULL\n");
            } else {
                printf("[Project][List] OnSelect on %s\n", node->getName());
            } });
        OnListDragDrop.add([](const char* drag_payload, void *src, std::shared_ptr<ListElement> target){
            printf("[Project][List] OnListDragDrop. drag_payload: %s\n", drag_payload);
        });
    }
    return this;
}

void Project::RenderAndLogic()
{
    if (!active)
        return;
    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNavInputs; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

    ImVec2 originalPadding = ImGui::GetStyle().WindowPadding;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("Project", NULL, flags))
    {
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

        auto imGuiManager = ImGuiManager::Instance();

        // display_order.push_back(ImGui::GetCurrentWindow());

        ImVec2 vMin = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
        // ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 255, 0, 255 ) );

        auto size = vMax - vMin;

        float thick_size = 8.0f * imGuiManager->GlobalScale;

        size.x -= thick_size;
        float h = size.y;
        float sz1 = size.x * 0.35f + thick_size * 0.5f;

        auto settings = CustomSettings::Instance();
        if (settings->splitterPos >= 0)
            sz1 = settings->splitterPos;

        float sz2 = size.x - sz1;
        if (sz2 < thick_size)
        {
            sz2 = thick_size;
            sz1 = size.x - sz2;
        }
        Splitter(true, thick_size, &sz1, &sz2, thick_size * 2.0f, thick_size * 2.0f, h);
        settings->splitterPos = sz1;

//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10)*imGuiManager->GlobalScale);
        ImGui::BeginChild("1", ImVec2(sz1, h), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_HorizontalScrollbar );

        if (clear_tree_selection != ProjectClearMethod::None){
            auto action = clear_tree_selection;
            clear_tree_selection = ProjectClearMethod::None;
    
            ImGuiID id_sel = ImGui::GetID("##project_sel");
            ImGui::GetStateStorage()->SetInt(id_sel, 0);

            if (action == ProjectClearMethod::ClearAndCallback)
                this->OnTreeSelect(nullptr);
        }

        if (force_tree_selection != 0){
            ImGuiID id_sel = ImGui::GetID("##project_sel");
            ImGui::GetStateStorage()->SetInt(id_sel, force_tree_selection);

            force_tree_selection = 0;
        }

        root->render("##project_sel", this);

        ImGui::EndChild();
        ImGui::PopStyleVar();


        ImGui::SameLine();
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10)*imGuiManager->GlobalScale);
        ImGui::BeginChild("2", ImVec2(sz2, h), ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        
        // // vMin = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        // // vMax = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
        // // ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 0, 255, 255 ) );

        // // ImVec2 area = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();//ImGui::GetContentRegionAvail();
        // // area = area - ImGui::GetStyle().WindowPadding - ImGui::GetStyle().FramePadding;

        // // ImGui::GetForegroundDrawList()->AddRect( ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos()+area, IM_COL32( 255, 0, 255, 255 ) );
        // // area.x -= ImGui::GetStyle().ScrollbarSize;

        // ImVec2 area = ImGui::GetContentRegionAvail();

        // ImVec2 icon_size = ImVec2(100, 80);
        // ImVec2 spacing = ImVec2(4, 4);

        // icon_size *= imGuiManager->GlobalScale;
        // spacing *= imGuiManager->GlobalScale;

        // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

        // float count_size = icon_size.x;
        // for (size_t i = 0; i < files.size(); i++)
        // {
        //     FileRef *file = &files[i];
        //     if (drawFile(i, file->name.c_str(), &file->selected, file->getIcon(), icon_size))
        //     {
        //         for (size_t j = 0; j < files.size(); j++)
        //         {
        //             if (i != j)
        //                 files[j].selected = false;
        //         }
        //     }
        //     count_size += spacing.x + icon_size.x;
        //     if (count_size > area.x)
        //     {
        //         count_size = icon_size.x;
        //     }
        //     else
        //         ImGui::SameLine();
        // }

        // ImGui::PopStyleVar();

        if (clear_list_selection != ProjectClearMethod::None){
            auto action = clear_list_selection;
            clear_list_selection = ProjectClearMethod::None;

            ImGuiID id_sel = ImGui::GetID("##proj_files_sel");
            ImGui::GetStateStorage()->SetInt(id_sel, 0);

            if (action == ProjectClearMethod::ClearAndCallback)
                this->OnListSelect(nullptr);
        }

        visualList.render("##proj_files_sel", this);

        ImGui::EndChild();
        ImGui::PopStyleVar();
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

// bool drawFile(int id, const char *name, bool *selected, IconType icon, ImVec2 size)
// {

//     // ImGui::SetWindowFontScale(0.9f);
//     ImGui::PushID(id);

//     ImVec2 pos = ImGui::GetCursorPos();
//     // ImVec2 size = ImVec2(100,80);
//     // ImVec2 spacing = ImVec2(5,5);

//     ImVec2 image_size = size * 0.4f;
//     // ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5,1.0) );

//     // ImGui::SetCursorPos(pos);
//     char aux[64];
//     sprintf(aux, "##%s_%i", name, id);
//     bool result = ImGui::Selectable(aux, selected, ImGuiSelectableFlags_None | ImGuiSelectableFlags_NoPadWithHalfSpacing, size);

//     // ImGui::PopStyleVar();
//     // ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
//     // printf("%f\n", ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
//     // ImGui::SameLine();

//     ImGui::SetCursorPos(pos + ImVec2((size.x - image_size.x) * 0.5f, 0));

//     bool is_to_stretch_image = ImGuiManager::Instance()->stretch[(int)icon];
//     AppKit::OpenGL::GLTexture *texture_ogl = ImGuiManager::Instance()->icons[(int)icon];
//     ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)texture_ogl->mTexture;
//     // float my_tex_w = (float)texture_ogl->width * 0.5f;
//     // float my_tex_h = (float)texture_ogl->height * 0.5f;
//     float my_tex_w = image_size.x;
//     float my_tex_h = image_size.y;

//     ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
//     ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right

//     ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
//     ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f); // 50% opaque white

//     if (is_to_stretch_image)
//     {

//         float aspect = my_tex_h / my_tex_w;
//         uv_max.y = aspect;

//         float c_aspect = (aspect - 1.0f) * 0.5f;
//         uv_min.y -= c_aspect;
//         uv_max.y -= c_aspect;
//     }
//     else
//     {
//         // tint_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

//         float aspect = my_tex_w / my_tex_h;
//         uv_max.x = aspect;

//         float c_aspect = (aspect - 1.0f) * 0.5f;
//         uv_min.x -= c_aspect;
//         uv_max.x -= c_aspect;
//     }

//     ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetFrameHeight() - 16.0f) * 0.5f);
//     ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);

//     float text_width = size.x;

//     ImVec2 text_size = ImGui::CalcTextSize(name, NULL, false, text_width);

//     ImVec2 aux_pos = size - text_size;
//     aux_pos.x = aux_pos.x * 0.5f;
//     aux_pos.y = aux_pos.y * 0.5f + image_size.y * 0.5f;

//     aux_pos = pos + aux_pos;

//     ImGui::SetCursorPos(aux_pos);
//     ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + text_width);
//     ImGui::Text("%s", name);
//     ImGui::PopTextWrapPos();

//     ImGui::SetCursorPos(pos);
//     ImGui::Dummy(size);
//     // ImGui::TextWrapped("fileName.jpg")

//     ImGui::PopID();

//     // ImGui::SetWindowFontScale(1.0f);

//     return result;
// }
