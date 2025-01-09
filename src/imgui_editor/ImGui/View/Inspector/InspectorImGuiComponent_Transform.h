#pragma once

#include "InspectorImGuiComponent.h"

namespace AppKit
{
    namespace GLEngine
    {
		class Transform;
	}
}

class InspectorImGuiComponent_Transform : public InspectorImGuiComponent {

	MathCore::vec3f pos;
	MathCore::quatf quat;
	MathCore::vec3f scale;

protected:
	std::shared_ptr<AppKit::GLEngine::Transform> transform_to_monitor;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_Transform(std::shared_ptr<AppKit::GLEngine::Transform> transform);

	~InspectorImGuiComponent_Transform();
};

