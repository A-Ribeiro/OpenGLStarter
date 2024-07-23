#pragma once

// #include <appkit-gl-engine/mini-gl-engine.h>
#include <appkit-gl-base/shaders/GLShaderColor.h>

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <InteractiveToolkit/Platform/Time.h>

#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>

namespace AppKit
{
    namespace GLEngine
    {

        //DefineMethodPointer(FadeMethodPtr, void) VoidMethodCall();

        class Fade: public EventCore::HandleCallback
        {
            std::vector<MathCore::vec3f> vertex;
            AppKit::OpenGL::GLShaderColor shaderColor;

            Platform::Time *time;
            EventCore::Callback<void()> OnEndCall;

            float lrp;
            float sec;
            MathCore::vec4f colorSrc;
            MathCore::vec4f colorTarget;
            MathCore::vec4f color;
            bool reset_draw_visible;
            bool draw_visible;
            float oldTimeScale;
            bool completeOnNextFrame;

            void setColor(Platform::Time *time);

            void createScreenVertex();

            std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

        public:
            bool isFading;

            Fade(Platform::Time *_time, std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindowRegion = nullptr);
            ~Fade();

            void fadeIn(float _sec, const EventCore::Callback<void()> &_OnEndCall);

            void fadeOut(float _sec, const EventCore::Callback<void()> &_OnEndCall);

            void draw();
        };

    }
}
