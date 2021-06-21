#ifndef _ComponentFontToMesh_h_
#define _ComponentFontToMesh_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentColorMesh.h>
#include <mini-gl-engine/ComponentMesh.h>
#include <mini-gl-engine/ComponentMaterial.h>

namespace GLEngine {

    namespace Components {

        class ComponentFontToMesh: public Component {

        public:
            static const ComponentType Type;

            SharedPointer<ComponentMaterial> material;
            SharedPointer<ComponentMesh> mesh;

            void toMesh(openglWrapper::GLFont2Builder &builder, bool dynamic);
            
            ComponentFontToMesh();
            virtual ~ComponentFontToMesh();

            void createAuxiliaryComponents();

            void attachToTransform(Transform *t);
            void start();
            
        };

    }
}

#endif
