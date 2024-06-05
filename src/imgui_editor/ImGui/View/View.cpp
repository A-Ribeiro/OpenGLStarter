#include "View.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"


View::View(ViewType type) {
    this->type = type;
	active = true;
}

View* View::Init() {
	return this;
}

void View::RenderAndLogic() {

}

void View::computeOnHoverAndOnFocus() {
    if (on_hover_detector.down)
        ImGuiManager::Instance()->OnHover(this, true);
    else if (on_hover_detector.up)
        ImGuiManager::Instance()->OnHover(this, false);
    
    if (on_focus_detector.down)
        ImGuiManager::Instance()->OnFocus(this, true);
    else if (on_focus_detector.up)
        ImGuiManager::Instance()->OnFocus(this, false);
}
