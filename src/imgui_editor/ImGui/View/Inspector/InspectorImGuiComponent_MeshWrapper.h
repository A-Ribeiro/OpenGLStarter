#pragma once

#include "InspectorImGuiComponent.h"

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
			class ComponentMeshWrapper;
		}
	}
}

class InspectorImGuiComponent_MeshWrapper : public InspectorImGuiComponent {

	// MathCore::vec3f pos;
	// MathCore::quatf quat;
	// MathCore::vec3f scale;

protected:
	std::shared_ptr<AppKit::GLEngine::Components::ComponentMeshWrapper> mesh_wrapper;

	void childRenderAndLogic();
public:

	InspectorImGuiComponent_MeshWrapper(std::shared_ptr<AppKit::GLEngine::Components::ComponentMeshWrapper> _mesh_wrapper);

	~InspectorImGuiComponent_MeshWrapper();
};

