#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

// #include <appkit-gl-engine/ComponentColorMesh.h>
// #include <appkit-gl-engine/GLRenderState.h>
// #include <appkit-gl-engine/ReferenceCounter.h>
// #include <aRibeiroData/aRibeiroData.h>

// #include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            enum LightType
            {
                LightNone,
                LightSun
            };

            class LightSunSetup
            {
            public:
                MathCore::vec3f color;
                float intensity;

                // computed on post-processing
                MathCore::vec3f worldDirection;
                MathCore::vec3f finalIntensity;

                bool render_after_skybox;
                float distance;
                float radius;

                float cone_tangent;
                float cone_cos;
                float cone_sin;

                LightSunSetup()
                {
                    color = MathCore::vec3f(1.0f);
                    intensity = 1.0f;

                    render_after_skybox = false;
                    // distance = 1.0f;
                    // radius = 1.0f;
                    // tangent = 0.0f;

                    // solar info:
                    //   radius: 695.700.000 m = 695.700 km
                    //   distance: 150.000.000.000 m = 150.000.000 km

                    // earth Sun information
                    smartSunLightCalculation(695.7 * 10.0, 150000.0, 500.0f);
                }

                void smartSunLightCalculation(
                    double sunRadiusKm,
                    double sunDistanceFromEarthKm,
                    float distanceToRender)
                {

                    double tangent_d = sunRadiusKm / sunDistanceFromEarthKm;
                    cone_tangent = (float)tangent_d;

                    double hypotenuse_d = sqrt(sunRadiusKm * sunRadiusKm + sunDistanceFromEarthKm * sunDistanceFromEarthKm);

                    double cone_cos_d = sunDistanceFromEarthKm / hypotenuse_d;
                    cone_cos = (float)cone_cos_d;

                    double cone_sin_d = sunRadiusKm / hypotenuse_d;
                    cone_sin = (float)cone_sin_d;

                    distance = distanceToRender;
                    double radius_d = tangent_d * (double)distanceToRender;
                    radius = (float)radius_d;
                }

                ~LightSunSetup()
                {
                }
            };

            class ComponentLight : public Component
            {
            public:
                static const ComponentType Type;

                // material type
                LightType type;

                // parameters
                LightSunSetup sun;

                bool cast_shadow;

                ComponentLight();

                MathCore::vec3f getWorldDirection(bool useVisitedFlag = true);

                void postProcessing_computeLightParameters();
                void createDebugLines();
            };
        }
    }

}