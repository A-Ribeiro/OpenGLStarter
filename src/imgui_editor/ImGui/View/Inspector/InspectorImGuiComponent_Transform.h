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
	std::shared_ptr<AppKit::GLEngine::Transform> transform;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_Transform(std::shared_ptr<AppKit::GLEngine::Transform> _transform);

	~InspectorImGuiComponent_Transform();
};

