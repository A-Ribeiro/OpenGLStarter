#pragma once

#include "InspectorImGuiComponent.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
			class ComponentMaterial;
		}
	}
}

class InspectorImGuiComponent_Material : public InspectorImGuiComponent {

	// MathCore::vec3f pos;
	// MathCore::quatf quat;
	// MathCore::vec3f scale;

protected:
	std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> material;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_Material(std::shared_ptr<AppKit::GLEngine::Components::ComponentMaterial> _material);

	~InspectorImGuiComponent_Material();
};

