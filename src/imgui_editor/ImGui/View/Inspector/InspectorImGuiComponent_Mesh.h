#pragma once

#include "InspectorImGuiComponent.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
			class ComponentMesh;
		}
	}
}

class InspectorImGuiComponent_Mesh : public InspectorImGuiComponent {

	// MathCore::vec3f pos;
	// MathCore::quatf quat;
	// MathCore::vec3f scale;

protected:
	std::shared_ptr<AppKit::GLEngine::Components::ComponentMesh> mesh;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_Mesh(std::shared_ptr<AppKit::GLEngine::Components::ComponentMesh> _mesh);

	~InspectorImGuiComponent_Mesh();
};

