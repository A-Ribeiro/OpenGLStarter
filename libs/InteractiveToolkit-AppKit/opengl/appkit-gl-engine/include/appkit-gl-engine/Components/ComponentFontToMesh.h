#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/font/GLFont2Builder.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentFontToMesh : public Component
            {

            public:
                static const ComponentType Type;

                SharedPointer<ComponentMaterial> material;
                SharedPointer<ComponentMesh> mesh;

                void toMesh(AppKit::OpenGL::GLFont2Builder &builder, bool dynamic);

                ComponentFontToMesh();
                ~ComponentFontToMesh();

                void createAuxiliaryComponents();

                void attachToTransform(Transform *t);
                void start();
            };

        }
    }

}