#pragma once

#include "InspectorImGuiComponent.h"

namespace AppKit
{
    namespace GLEngine
    {
		class Component;
	}
}

class InspectorImGuiComponent_Unknown : public InspectorImGuiComponent {

	// MathCore::vec3f pos;
	// MathCore::quatf quat;
	// MathCore::vec3f scale;

protected:
	std::shared_ptr<AppKit::GLEngine::Component> component;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_Unknown(std::shared_ptr<AppKit::GLEngine::Component> _component);

	~InspectorImGuiComponent_Unknown();
};

