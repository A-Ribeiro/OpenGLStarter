#include "Project.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

bool drawFile(int id, const char* name, bool *selected, IconType icon, ImVec2 size );


Project::Project() : View()
{
	uid_incrementer = 1;

    files.push_back(FileRef(FileRefType::File, "Very Big Name File.jpg"));
    files.push_back(FileRef(FileRefType::File, "Another File.png"));
    files.push_back(FileRef(FileRefType::File, "FileB.png"));
    files.push_back(FileRef(FileRefType::Folder, "SomeFolder"));
    files.push_back(FileRef(FileRefType::Folder, "AnotherFolder"));
    files.push_back(FileRef(FileRefType::EmptyFolder, "SomeEmpty"));
}

View* Project::Init()
{

	// creating testing node
	root = HierarchyTreeNode(uid_incrementer++, IconType::Small_Folder_Filled, "/");
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_Folder_Empty, "child1"));
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_Folder_Filled, "child2"));
	root.children[root.children.size() - 1].children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_Folder_Empty, "child2leaf"));
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_Folder_Empty, "child3"));


	ImGuiMenu::Instance()->AddMenu(
		"Window/Project", "", [this]()
		{ printf("Window/Project\n"); },
		&this->active);
	return this;
}

void Project::RenderAndLogic()
{
	if (!active)
		return;
	auto flags = ImGuiWindowFlags_NoCollapse; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

	ImVec2 originalPadding = ImGui::GetStyle().WindowPadding;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Project", NULL, flags))
	{
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
		ImGui::BeginChild("1", ImVec2(sz1, h), true, ImGuiWindowFlags_HorizontalScrollbar);
		// vMin = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
		// vMax = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
		// ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 0, 255, 255 ) );

		bool deselect_all = false;
		if (ImGui::IsWindowHovered())
		{
			if (ImGui::IsMouseClicked(0) || ImGui::IsKeyDown(ImGuiKey_Escape))
			{
				// printf("Clicked on begin...\n");
				deselect_all = true;
			}
		}

		ImGuiID id_sel = ImGui::GetID("##project_sel");
		int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

		bool any_click_occured = false;
		root.renderRecursive(id_sel, selected_UID, &any_click_occured);// , & time);
		if (any_click_occured)
			deselect_all = false;

		if (deselect_all)
		{
			// printf("reset selection...\n");
			// ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
			ImGui::GetStateStorage()->SetInt(id_sel, 0);
		}


		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, originalPadding);
		ImGui::BeginChild("2", ImVec2(sz2, h), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		// vMin = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
		// vMax = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();
		// ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 0, 255, 255 ) );

        //ImVec2 area = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();//ImGui::GetContentRegionAvail();
        //area = area - ImGui::GetStyle().WindowPadding - ImGui::GetStyle().FramePadding;
        
        //ImGui::GetForegroundDrawList()->AddRect( ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos()+area, IM_COL32( 255, 0, 255, 255 ) );
        //area.x -= ImGui::GetStyle().ScrollbarSize;

        ImVec2 area = ImGui::GetContentRegionAvail();


        ImVec2 icon_size = ImVec2(100,80);
        ImVec2 spacing = ImVec2(4,4);

        icon_size *= imGuiManager->GlobalScale;
        spacing *= imGuiManager->GlobalScale;


ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

        float count_size = icon_size.x;
        for(size_t i=0;i<files.size();i++){
            FileRef *file = &files[i];
            if (drawFile(i, file->name.c_str(), &file->selected, file->getIcon(), icon_size )){
                for(size_t j=0;j<files.size();j++){
                    if (i!=j)
                        files[j].selected = false;
                }
            }
            count_size += spacing.x + icon_size.x;
            if (count_size > area.x){
                count_size = icon_size.x;
            }else
                ImGui::SameLine();
        }

ImGui::PopStyleVar();

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

bool drawFile(int id, const char* name, bool *selected, IconType icon, ImVec2 size ) {

    //ImGui::SetWindowFontScale(0.9f);
    ImGui::PushID(id);

    ImVec2 pos = ImGui::GetCursorPos();
    //ImVec2 size = ImVec2(100,80);
    //ImVec2 spacing = ImVec2(5,5);

    ImVec2 image_size = size * 0.4f;
    //ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5,1.0) );

    //ImGui::SetCursorPos(pos);
    char aux[64];
    sprintf(aux, "##%s_%i", name, id);
    bool result = ImGui::Selectable(aux, selected, ImGuiSelectableFlags_None | ImGuiSelectableFlags_NoPadWithHalfSpacing, size);

    //ImGui::PopStyleVar();
    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    //printf("%f\n", ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
    //ImGui::SameLine();

    ImGui::SetCursorPos(pos + ImVec2((size.x-image_size.x)*0.5f,0));

    bool is_to_stretch_image = ImGuiManager::Instance()->stretch[ (int)icon ];
    AppKit::OpenGL::GLTexture* texture_ogl = ImGuiManager::Instance()->icons[ (int)icon ];
	ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)texture_ogl->mTexture;
	//float my_tex_w = (float)texture_ogl->width * 0.5f;
	//float my_tex_h = (float)texture_ogl->height * 0.5f;
    float my_tex_w = image_size.x;
    float my_tex_h = image_size.y;
	
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right

    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f); // 50% opaque white

    if (is_to_stretch_image){

        float aspect = my_tex_h/my_tex_w;
        uv_max.y = aspect;

        float c_aspect = (aspect - 1.0f) * 0.5f;
        uv_min.y -= c_aspect;
        uv_max.y -= c_aspect;

    } else {
        //tint_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        float aspect = my_tex_w/my_tex_h;
        uv_max.x = aspect;

        float c_aspect = (aspect - 1.0f) * 0.5f;
        uv_min.x -= c_aspect;
        uv_max.x -= c_aspect;
    }

	
	
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ( ImGui::GetFrameHeight() - 16.0f ) * 0.5f);
	ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);


    float text_width = size.x;

    ImVec2 text_size = ImGui::CalcTextSize(name, NULL, false, text_width);

    ImVec2 aux_pos = size - text_size;
    aux_pos.x = aux_pos.x * 0.5f;
    aux_pos.y = aux_pos.y * 0.5f + image_size.y*0.5f;

    aux_pos = pos + aux_pos;

    ImGui::SetCursorPos(aux_pos);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + text_width);
    ImGui::Text("%s", name);
    ImGui::PopTextWrapPos();

    ImGui::SetCursorPos(pos);
    ImGui::Dummy(size);
    //ImGui::TextWrapped("fileName.jpg")

    ImGui::PopID();

    //ImGui::SetWindowFontScale(1.0f);

    return result;
}
