#pragma once

#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/Components/ComponentFontToMesh.h>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>
#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

namespace AppKit
{
    namespace GLEngine
    {

        class Button
        {
        public:
            std::string id;
            std::string rendered_text;
            std::shared_ptr<Transform> root;
            std::shared_ptr<Components::ComponentFontToMesh> componentFontToMesh;
            std::shared_ptr<Components::ComponentMaterial> materialBackground;
            AppKit::OpenGL::GLFont2Builder *fontBuilder;

            CollisionCore::AABB<MathCore::vec3f> aabb;

            std::shared_ptr<Components::ComponentCameraOrthographic> camera;

            // alignment
            int position;
            bool left;

            bool selected;

            Button(int _position, bool _left,
                   const std::string &_id,
                   const std::string &_text,
                   AppKit::OpenGL::GLFont2Builder *_fontBuilder);

            Button(int _position, bool _left,
                   const std::string &_id,
                   const std::string &_text,
                   std::shared_ptr<Components::ComponentCameraOrthographic> camera,
                   AppKit::OpenGL::GLFont2Builder * _fontBuilder);

            void update(const MathCore::vec3f &mousePosition);

            void updateText(const std::string &newText);

            void resize(const MathCore::vec2i &size);

            std::shared_ptr<Transform> getTransform();

            // static MathCore::vec3f App2MousePosition(bool normalized = false) {
            //     AppKit::GLEngine::AppBase * app = AppKit::GLEngine::Engine::Instance()->app;
            //     MathCore::vec2f windowSize_2 = MathCore::vec2f(app->WindowSize.value.x,app->WindowSize.value.y) * 0.5f;
            //     MathCore::vec3f mousePosition = MathCore::vec3f(app->MousePos - windowSize_2,0.0f);

            //     if (normalized){
            //         if (windowSize_2.x > windowSize_2.y){
            //             mousePosition.x /= windowSize_2.y;
            //             mousePosition.y /= windowSize_2.y;
            //         } else {
            //             mousePosition.x /= windowSize_2.x;
            //             mousePosition.y /= windowSize_2.x;
            //         }
            //         //printf("mouse %f %f\n", mousePosition.x, mousePosition.y);
            //     }

            //     return mousePosition;
            // }
        };

    }

}