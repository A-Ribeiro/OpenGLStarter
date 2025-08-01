#include "UIItem.h"
#include "ComponentUI.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            UIItem::UIItem(std::shared_ptr<Transform> transform,
                           std::weak_ptr<ComponentUI> self_ui)
            {
                this->transform = transform;
                this->self_ui = self_ui;
                this->type = UIItemNone;
            }

            void UIItem::removeSelf()
            {
                if (auto self = self_ui.lock())
                {
                    auto it = std::find(self->items.begin(), self->items.end(), *this);
                    if (it != self->items.end())
                        self->items.erase(it);
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
