#ifndef _Button_H_
#define _Button_H_

#include <mini-gl-engine/ComponentMaterial.h>
#include <mini-gl-engine/ComponentFontToMesh.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {

    class _SSE2_ALIGN_PRE Button{
    public:
        std::string id;
        std::string rendered_text;
        Transform *root;
        Components::ComponentFontToMesh *componentFontToMesh;
        Components::ComponentMaterial *materialBackground;
        openglWrapper::GLFont2Builder *fontBuilder;
        
        aRibeiro::collision::AABB aabb;
        
        //alignment
        int position;
        bool left;
        
        bool selected;
        
        Button(int _position, bool _left, 
            const std::string &_id, 
            const std::string &_text, 
            openglWrapper::GLFont2Builder *_fontBuilder);
        
        void update(const aRibeiro::vec3 &mousePosition);
        
        void updateText(const std::string &newText);
        
        void resize(const sf::Vector2i &size);
        
        Transform *getTransform();

        static aRibeiro::vec3 App2MousePosition(bool normalized = false) {
            GLEngine::AppBase * app = GLEngine::Engine::Instance()->app;
            aRibeiro::vec2 windowSize_2 = aRibeiro::vec2(app->WindowSize.value.x,app->WindowSize.value.y) * 0.5f;
            aRibeiro::vec3 mousePosition = aRibeiro::vec3(app->MousePos - windowSize_2,0.0f);

            if (normalized){
                if (windowSize_2.x > windowSize_2.y){
                    mousePosition.x /= windowSize_2.y;
                    mousePosition.y /= windowSize_2.y;
                } else {
                    mousePosition.x /= windowSize_2.x;
                    mousePosition.y /= windowSize_2.x;
                }
                //printf("mouse %f %f\n", mousePosition.x, mousePosition.y);
            }

            return mousePosition;
        }

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
