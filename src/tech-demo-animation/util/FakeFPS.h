#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        class FakeFPS
        {

            float acc;
            float framerate;
            float framerate_length;

        public:
            float deltaTime;

            FakeFPS(float fps_to_fake = 5.0f)
            {
                setFPS(fps_to_fake);
            }

            void setFPS(float _fps)
            {
                acc = 0.0f;
                framerate = _fps;
                framerate_length = 1.0f / _fps;
                deltaTime = 0.0f;
            }

            void update(float elapsed)
            {
                acc += elapsed;
                float frames_count = MathCore::OP<float>::floor(acc / framerate_length);
                acc = MathCore::OP<float>::fmod(acc, framerate_length);
                deltaTime = frames_count * framerate_length;
            }
        };
    }
}