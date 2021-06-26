#ifndef Game__H
#define Game__H

#include <aribeiro/aribeiro.h>

#include "bloom/BloomPowerOfTwo.h"

using namespace aRibeiro;

#include <string>
#include <vector>

#include "RenderSystem.h"

extern bool canExitApp;
extern sf::RenderWindow *currentWindow;

#include "common/common.h"

class BloomInterface: public GUIGroup {

    GLTexture *earth;
    vec2 mouseTracker;
    bool track;
    vec2 cameraRotationAngle;

    GLPostProcessPowerOfTwo *postProcess;
    BloomPowerOfTwo *bloom;

    GLTexture *bolinha;
    GLFont *font;

    int
    idSliderThreshold,
    idSliderIntensity;
public:
    BloomInterface() {

        //load font
        {
            Font* font_raw = Font::LoadFromBasof("resources/font/BenjaminGothic.basof");
            font = font_raw->createGLFont();
            setNullAndDelete(font_raw);
        }

        earth = GLTexture::loadFromFile("resources/earth.png");

        bolinha = GLTexture::loadFromFile("resources/bolinha.png");

        mouseTracker = vec2(0, 0);
        track = false;
        cameraRotationAngle = vec2(0, 0);

        postProcess = new GLPostProcessPowerOfTwo();
        bloom = new BloomPowerOfTwo();

        postProcess->pipeline.push_back(bloom);

        /*
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        postProcess->pipeline.push_back(bloom);
        //*/

    }

    virtual ~BloomInterface() {
        setNullAndDelete(font);
        setNullAndDelete(earth);
        setNullAndDelete(bolinha);
        setNullAndDelete(bloom);
        setNullAndDelete(postProcess);
    }

    virtual void resizeWindow(const int w, const int h) {

        postProcess->resize(w,h);

        glViewport(0, 0, w, h);

        RenderSystem * render = RenderSystem::getSingleton();
        render->projection.top = projection_perspective_rh_negative_one(60.0f, (float)w / (float)h, 0.01f, 100.0f);

        GUIGroup::resizeWindow(w,h);

        clearAll();

        idSliderThreshold = addSlider(vec2(w-40-100,h-40), vec2(w-40,h-40), 20.0f, bolinha);
        idSliderIntensity = addSlider(vec2(w-40-100,h-40-40), vec2(w-40,h-40-40), 20.0f, bolinha);

        ((Slider*)getInterface(idSliderThreshold))->setLerp(bloom->threshold);
        ((Slider*)getInterface(idSliderIntensity))->setLerp(bloom->intensity);

        //labels
        AABB aabb = font->computeBoundsJustBox("Threshold:");
        addLabel(font, "Threshold:", vec2(w-40-100 - 5 - (aabb.max_box.x - aabb.min_box.x) * 0.25f ,h-40-4), false, false, 0.25f);

        aabb = font->computeBoundsJustBox("Intensity:");
        addLabel(font, "Intensity:", vec2(w-40-100 - 5 - (aabb.max_box.x - aabb.min_box.x) * 0.25f ,h-40-40-4-3), false, false, 0.25f);


        const char* str =
            "Post Processing ( Bloom FX )\n"
            "http://alessandroribeiro.thegeneralsolution.com";

        addLabel(font, str, vec2( 20 , 40 ), true, false, 0.3f);

    }

    virtual bool performAction(const int ID, Event &e) {
        if (ID ==idSliderThreshold )
            bloom->threshold = e.lerpChanged.newLerp;
        else if (ID == idSliderIntensity)
            bloom->intensity = e.lerpChanged.newLerp;
        return true;
    }

    virtual void render(const unsigned int time_ms, const bool showCursor = true) {

        static unsigned int previoustime = time_ms;
        unsigned int delta = time_ms - previoustime;
        previoustime = time_ms;

        postProcess->beginDraw();

        RenderSystem * render = RenderSystem::getSingleton();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render->model.top = translate(0, 0, -4.0f) *
        rotate(DEG2RAD(cameraRotationAngle.y), 1, 0, 0) *
        rotate(DEG2RAD(cameraRotationAngle.x), 0, 1, 0);

        //render->drawTexture(earth, AABB(vec2(-1), vec2(1)));

        render->model.push();
        render->model.top *= rotate(DEG2RAD(148.200241f), 0, 1, 0);
        render->drawSphere(earth, 32, 32, 1.0f);
        //render->drawAABB_Lines(collision::AABB(vec3(0), vec3(1)), vec4(1.0f));
        render->model.pop();

        //render->drawAABB_Lines(AABB(vec3(-1), vec3(1)),vec4(1.0f));

        postProcess->endDraw();

        glDisable(GL_DEPTH_TEST);
        render->projection.push();
        render->projection.top = mat4_IdentityMatrix;
        render->model.top = translate(-1, -1, 0) * scale(1.0f/centerScreen.x, 1.0f/centerScreen.y, 1);
        char fps[32];
        static float deltaacc = 0;
        deltaacc = lerp(deltaacc, ((float)delta/1000.0f), 0.1f);
        sprintf(fps, "fps: %.0f", 1.0f / deltaacc);
        render->print(font, fps, vec2(centerScreen.x,centerScreen.y*2.0f - 30.0f), vec4(1.0f), 0.5f, 0.5f, 0.3f);
        render->projection.pop();
        glEnable(GL_DEPTH_TEST);

        GUIGroup::render(time_ms);
    }

    virtual bool mouseDown(const int btn, const vec2 &pos) {

        if (!GUIGroup::mouseDown(btn, pos)) {
            track = true;
            mouseTracker = pos;
        }
        return true;
    }

    virtual bool mouseUp(const int btn, const vec2 &pos) {
        track = false;
        GUIGroup::mouseUp(btn, pos);
        return true;
    }

    virtual void setMousePos(const vec2 &pos) {

        GUIGroup::setMousePos(pos);
        //bloom->threshold = (float)pos.x / (float)(postProcess->width-1);

        if (track) {
            if (mouseTracker.x < 0 || mouseTracker.y < 0)
                mouseTracker = pos;
            vec2 diff = pos - mouseTracker;

            cameraRotationAngle = cameraRotationAngle + diff * vec2(1, -1)*0.2;
            while (cameraRotationAngle.x >= 360) cameraRotationAngle.x -= 360;
            while (cameraRotationAngle.x <= -360) cameraRotationAngle.x += 360;
            while (cameraRotationAngle.y >= 360) cameraRotationAngle.y -= 360;
            while (cameraRotationAngle.y <= -360) cameraRotationAngle.y += 360;
            if (cameraRotationAngle.y > 90) cameraRotationAngle.y = 90;
            if (cameraRotationAngle.y < -90) cameraRotationAngle.y = -90;

            mouseTracker = pos;
        }
    }

    SSE2_CLASS_NEW_OPERATOR
};



class App {
    int w, h;

    vec2 cursorPos;
    Interface *currentInterface;

    GLTexture *cursor;

public:
    App(int w, int h) {
        this->w = w;
        this->h = h;

        cursor = GLTexture::loadFromFile("resources/cursor.png");

        currentInterface = new BloomInterface();

        cursorPos = vec2(-1);
        resizeWindow(w, h);
    }
    ~App() {
        setNullAndDelete(cursor);
        setNullAndDelete(currentInterface);
    }

    void resizeWindow(const int w, const int h) {
        this->w = w;
        this->h = h;

        currentInterface->resizeWindow(w,h);
    }
    void idleDraw(const unsigned int time_ms) {
        currentInterface->render(time_ms);

        RenderSystem *render = RenderSystem::getSingleton();

        
        GLboolean depthTest;
        glGetBooleanv (GL_DEPTH_TEST, &depthTest);
        
        if (depthTest)
            glDisable(GL_DEPTH_TEST);
        
        render->projection.push();
        render->projection.top = mat4_IdentityMatrix;
        render->model.top = translate(-1, -1, 0) *
                            scale(2.0f / (float)w, 2.0f / (float)h, 1);
        render->drawTexture_center(cursor, cursorPos);
        render->projection.pop();
        
        if (depthTest)
            glEnable(GL_DEPTH_TEST);

    }

    void keyDown(const int key) {
        currentInterface->keyDown(key);
    }
    void keyUp(const int key) {
        currentInterface->keyUp(key);
    }
    void mouseDown(const int btn) {
        if (cursorPos.x < 0 || cursorPos.y < 0) {
            sf::Vector2i p = sf::Mouse::getPosition(*currentWindow);
            cursorPos = vec2(p.x, h - 1 - p.y);
        }
        currentInterface->mouseDown(btn, cursorPos);
    }
    void mouseUp(const int btn) {
        if (cursorPos.x >=0 && cursorPos.y >=0)
            currentInterface->mouseUp(btn, cursorPos);
    }
    void mouseMove(const int x, const int y) {
        cursorPos = vec2(x, h - 1 - y);
        currentInterface->setMousePos(cursorPos);
    }

    SSE2_CLASS_NEW_OPERATOR

};

#endif
