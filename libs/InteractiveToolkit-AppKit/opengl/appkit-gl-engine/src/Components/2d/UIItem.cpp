#include <appkit-gl-engine/components/2d/UIItem.h>
#include <appkit-gl-engine/components/2d/ComponentUI.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            void UIItem::setType(UIItemType new_type) {
                type = new_type;
                // transform.reset();
                // self_ui.reset();
                rectangle.reset();
                sprite.reset();
                ui.reset();
            }
            UIItem::UIItem(){
                type = UIItemNone;
            }
            UIItem::UIItem(std::shared_ptr<Transform> transform,
                           std::weak_ptr<ComponentUI> self_ui)
            {
                this->transform = transform;
                this->self_ui = self_ui;
                this->type = UIItemNone;
            }

            void UIItem::removeSelf()
            {
                this->transform->removeSelf();
                if (auto component_ui = self_ui.lock())
                {
                    auto it = std::find(component_ui->items.begin(), component_ui->items.end(), *this);
                    if (it != component_ui->items.end())
                        component_ui->items.erase(it);
                }
            }

            bool UIItem::operator==(const UIItem &other) const
            {
                return // type == other.type &&
                    transform == other.transform &&
                    self_ui.lock() == other.self_ui.lock();
            }
        }
    }
}
