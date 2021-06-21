#include "Fade.h"

#include <mini-gl-engine/mini-gl-engine.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {

    void Fade::setColor(aRibeiro::PlatformTime *time) {

        if (completeOnNextFrame) {
            AppBase *app = Engine::Instance()->app;
            app->OnUpdate.remove(this, &Fade::setColor);
            if (reset_draw_visible)
                draw_visible = false;
            isFading = false;
            time->timeScale = oldTimeScale;

            if (OnEndCall != NULL)
                OnEndCall();
            return;
        }

        lrp = aRibeiro::move(lrp, 1.0f, time->unscaledDeltaTime/sec );
        color = lerp( colorSrc, colorTarget, lrp );

        //textTransform->setLocalPosition( lerp(src, target,easeOutExpo(0.0f, 1.0f, lrp)) );
        if (lrp == 1.0f){
            completeOnNextFrame = true;
        }

    }

    void Fade::createScreenVertex() {
        vertex.clear();

        vertex.push_back(aRibeiro::vec3(-1,-1,0));
        vertex.push_back(aRibeiro::vec3(1,1,0));
        vertex.push_back(aRibeiro::vec3(1,-1,0));


        vertex.push_back(aRibeiro::vec3(-1,-1,0));
        vertex.push_back(aRibeiro::vec3(-1,1,0));
        vertex.push_back(aRibeiro::vec3(1,1,0));
    }

    Fade::Fade(aRibeiro::PlatformTime *_time) {
        time = _time;
        createScreenVertex();
        isFading = false;
        draw_visible = false;
        completeOnNextFrame = false;
    }

    Fade::~Fade() {
        AppBase *app = Engine::Instance()->app;
        app->OnUpdate.remove(this, &Fade::setColor);
    }

    void Fade::fadeIn(float _sec, const FadeMethodPtr &_OnEndCall) {

        if (!isFading)
            oldTimeScale = time->timeScale;

        time->timeScale = 0.0f;

        OnEndCall = _OnEndCall;
        GLEngine::AppBase *app = GLEngine::Engine::Instance()->app;
        app->OnUpdate.remove(this, &Fade::setColor);
        app->OnUpdate.add(this,&Fade::setColor );
        lrp = 0.0f;
        sec = _sec;
        colorSrc = aRibeiro::vec4(0,0,0,0);
        colorTarget = aRibeiro::vec4(0,0,0,1);
        color = lerp( colorSrc, colorTarget, lrp );
        reset_draw_visible = false;
        draw_visible = true;
        isFading = true;
        completeOnNextFrame = false;
    }

    void Fade::fadeOut(float _sec, const FadeMethodPtr &_OnEndCall) {

        if (!isFading)
            oldTimeScale = time->timeScale;

        time->timeScale = 0.0f;

        OnEndCall = _OnEndCall;
        GLEngine::AppBase *app = GLEngine::Engine::Instance()->app;
        app->OnUpdate.remove(this, &Fade::setColor);
        app->OnUpdate.add(this,&Fade::setColor );
        lrp = 0.0f;
        sec = _sec;
        colorTarget = aRibeiro::vec4(0,0,0,0);
        colorSrc = aRibeiro::vec4(0,0,0,1);
        color = lerp( colorSrc, colorTarget, lrp );
        reset_draw_visible = true;
        draw_visible = true;
        isFading = true;
        completeOnNextFrame = false;
    }

    void Fade::draw() {

        if (!draw_visible)
            return;

        GLEngine::GLRenderState *renderstate = GLEngine::GLRenderState::Instance();

        //openglWrapper::GLShader* oldShader = renderstate->CurrentShader;
        GLEngine::DepthTestType oldDepthTest = renderstate->DepthTest;
        bool oldDepthTestEnabled = renderstate->DepthWrite;
        GLEngine::BlendModeType oldBlendMode = renderstate->BlendMode;

        renderstate->CurrentShader = &shaderColor;
        renderstate->DepthTest = GLEngine::DepthTestDisabled;
        renderstate->DepthWrite = false;
        renderstate->BlendMode = GLEngine::BlendModeAlpha;

        shaderColor.setMatrix( aRibeiro::mat4() );
        shaderColor.setColor( color );

        // direct draw commands
        OPENGL_CMD(glEnableVertexAttribArray(shaderColor.vPosition));
        OPENGL_CMD(glVertexAttribPointer(shaderColor.vPosition, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertex[0]));

        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

        OPENGL_CMD(glDisableVertexAttribArray(shaderColor.vPosition));

        //renderstate->CurrentShader = oldShader;
        renderstate->DepthTest = oldDepthTest;
        renderstate->DepthWrite = oldDepthTestEnabled;
        renderstate->BlendMode = oldBlendMode;
    }

}


