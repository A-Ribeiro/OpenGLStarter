#ifndef _ComponentLight_h_
#define _ComponentLight_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

//#include <mini-gl-engine/ComponentColorMesh.h>
//#include <mini-gl-engine/GLRenderState.h>
//#include <mini-gl-engine/ReferenceCounter.h>
//#include <data-model/data-model.h>

//#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {

    namespace Components {

        enum LightType {
            LightNone,
            LightSun
        };

        class _SSE2_ALIGN_PRE LightSunSetup {
        public:
            aRibeiro::vec3 color;
            float intensity;

            //computed on post-processing
            aRibeiro::vec3 worldDirection;
            aRibeiro::vec3 finalIntensity;

            LightSunSetup() {
                color = aRibeiro::vec3(1.0f);
                intensity = 1.0f;
            }

            virtual ~LightSunSetup() {
            }

            SSE2_CLASS_NEW_OPERATOR
        } _SSE2_ALIGN_POS;

        class ComponentLight : public Component {
        public:

            static const ComponentType Type;

            //material type
            LightType type;

            //parameters
            LightSunSetup sun;

            ComponentLight() :Component(ComponentLight::Type) {
                type = LightNone;
            }

            aRibeiro::vec3 getWorldDirection(bool useVisitedFlag = true) const {
                return transform[0]->getRotation(useVisitedFlag) * aRibeiro::vec3(0, 0, 1);
            }

            void postProcessing_computeLightParameters() {
                if (type == LightSun) {
                    sun.worldDirection = getWorldDirection();
                    sun.finalIntensity = sun.color * sun.intensity;
                }
            }

        };
    }
}

#endif
