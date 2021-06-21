#ifndef Game__H
#define Game__H

#include <aribeiro/aribeiro.h>

//#include "../../windowAPI/gameAbstraction.h"
#include "common/common.h"

#include "common/RenderSystem.h"
#include "ShaderManager.h"

#include "commonDefinition.h"


#include <string>
#include <vector>

extern bool canExitApp;


#define ACTION_EXIT 3
#define ACTION_BACK 4
#define ACTION_SDR1 5
#define ACTION_SDR2 6
#define ACTION_SDR3 7
#define ACTION_SDR4 8
#define ACTION_SDR5 9

class ShaderMenu : public GUIGroup {
    GLTexture *cursor;
    GLFont *font;
    int UI_SLIDER_1,
        UI_SLIDER_2,
        UI_LABEL_1,
        UI_LABEL_2,
        UI_BTN_EXIT,

        UI_BTN_PLANEXY_div,
        UI_BTN_PLANEXY,
        UI_BTN_CUBE;

    GLTexture *bolinha;

    //AlMultiSource *hover;

    /*
      mouse tracker
    */
    vec2 mouseTracker;
    bool track;
    vec2 cameraRotationAngle;
    float distance;

    std::string shaderBranch;
    std::string shaderName;
    std::vector<std::string> shaderNames;

    /*
      shader manager
    */
    //GLTexture *fbo;
    //CGFXHelper helper;
    //CGFXHelper shaderGUI;

    std::vector<int> UI_ID;

    //std::vector<CGtechnique> shader;
    //std::vector<bool> shaderpp;

    float lerp1, lerp2;

    bool postProcessingMode;
    /*
      object to draw
    */
    int drawObject;//0 - cube; 1 - planexy
public:
    ShaderMenu(GLTexture *cursor, GLFont *fnt, const char * shaderBranch) {

        lerp1 = 0.5f;
        lerp2 = 0.5f;

        this->shaderBranch = shaderBranch;

        bolinha = GLTexture::loadFromFile("resources/images/bolinha.png");

        font = fnt;
        this->cursor = cursor;
        //this->hover = hover;
        mouseTracker = vec2(0, 0);
        track = false;
        cameraRotationAngle = vec2(0, 0);
        distance = 0;

        //helper.activeFirstTechnique();

        drawObject = 0;
        postProcessingMode = false;

        //helper.setSampler(helper.getParameter("framebuffer"), fbo.getColorTexture()->getHandle(), false);


    }

    virtual ~ShaderMenu(){
        setNullAndDelete(bolinha);
    }

    void setLerp1(float v) {
        RenderSystem *render = RenderSystem::getSingleton();
        ShaderManager *shaderManager = render->shaderManager;

        shaderManager->conedepth = v;

        shaderManager->laplaceIntensity = v;
        //shaderManager->laplaceBlend = v;

        shaderManager->blurSteps = (int)(v * 5.0f + 0.5f);

        shaderManager->brightness = v;
        //shaderManager->contrast = v;

        shaderManager->blend = v;

        shaderManager->frequency = v;

        shaderManager->normalStrength = v;
    }

    void setLerp2(float v) {
        RenderSystem *render = RenderSystem::getSingleton();
        ShaderManager *shaderManager = render->shaderManager;

        //shaderManager->conedepth = v;

        //shaderManager->laplaceIntensity = v;
        shaderManager->laplaceBlend = v;

        //shaderManager->blurSteps = (int)(v * 10.0f + 1.0f + 0.5f);

        //shaderManager->brightness = v;
        shaderManager->contrast = v;

        //shaderManager->blend = v;

        //shaderManager->frequency = v;
    }

    void resizeWindow(const int w, const int h) {
        if (this->w == w && this->h == h) return;
        GUIGroup::resizeWindow(w, h);
        clearAll();

        RenderSystem *render = RenderSystem::getSingleton();
        ShaderManager *shaderManager = render->shaderManager;

        shaderManager->framebuffer->setSize(w, h);
        shaderManager->framebufferBlur->setSize(w, h);
        shaderManager->framebufferNeighbor = vec2( 1.0f / (float)w, 1.0f / (float)h);

        /*
          Slider1
        */
        vec2 position = vec2(w - (20), h - (20 + 12));

        float lrp = lerp1;
        setLerp1(lrp);
        char tmp[10]; sprintf(tmp, "%03.0f", lrp*100.0f);

        UI_LABEL_1 = addLabel(font,
            tmp,
            position + vec2(-200, 0),
            false, false, 0.4f);

        UI_SLIDER_1 = addSlider(position + vec2(-200, -12),
            position + vec2(0, -12),
            20, bolinha);

        ((Slider*)getInterface(UI_SLIDER_1))->setLerp(lrp);

        /*
          Slider2
        */
        position = position + vec2(0, -(20 + 20));

        lrp = lerp2;
        setLerp2(lrp);
        sprintf(tmp, "%03.0f", lrp*100.0f);

        UI_LABEL_2 = addLabel(font,
            tmp,
            position + vec2(-200, 0),
            false, false, 0.4f);
        UI_SLIDER_2 = addSlider(position + vec2(-200, -12),
            position + vec2(0, -12),
            20, bolinha);

        ((Slider*)getInterface(UI_SLIDER_2))->setLerp(lrp);



        //adicionando botoes
        position = vec2(20, h - (20 + font->getLineHeight() / 2));

        UI_BTN_EXIT = addLabelBtn(font,
            "BACK",
            position, vec2(200, 0),
            true, false, 0.4f);

        LabelBtn* lbtn = (LabelBtn*)getInterface(UI_BTN_EXIT);

        UI_ID.clear();

        //shader.clear();
        //shaderpp.clear();

        shaderNames = shaderManager->getInsideGroupList( shaderBranch.c_str() );

        for (int i = 0; i < shaderNames.size(); i++) {
            const char* name = shaderNames[i].c_str();

            //position = position + vec2(0, -font->getLineHeight() / 2 - 20);
            //position = position + vec2(0, - 20);

            UI_ID.push_back(
                addLabelBtn(font,
                    name,
                    position, vec2(200, 0),
                    true, false, 0.4f)
            );

            //
            // update position according the quad dimension of the button...
            //
            LabelBtn* lbtn_new = ((LabelBtn*)getInterface(UI_ID[UI_ID.size() - 1]));

            position += vec2(0, -(lbtn->getQuadDimension().y + lbtn_new->getQuadDimension().y )*0.5f - 20);
            lbtn_new->setPosition(position);

            lbtn = lbtn_new;

        }

        shaderName = shaderNames[0];//activate the first shader of this group
        postProcessingMode = shaderManager->isPostProcessing(shaderName);



        //add primitive
        position = vec2(w - 20 - 100 - 50, (20 + font->getLineHeight() / 2));

        UI_BTN_PLANEXY_div = addLabelBtn(font,
            "XY_DIV",
            position, vec2(100, 0),
            true, false, 0.4f);

        position = position + vec2(0, (20 + font->getLineHeight() / 2));

        UI_BTN_PLANEXY = addLabelBtn(font,
            "PLANEXY",
            position, vec2(100, 0),
            true, false, 0.4f);

        position = position + vec2(0, (20 + font->getLineHeight() / 2));

        UI_BTN_CUBE = addLabelBtn(font,
            "CUBE",
            position, vec2(100, 0),
            true, false, 0.4f);

    }
    bool performAction(const int ID, Event &e) {
        //    if(ID == label2) return false;
        switch (e.type) {
        case MOUSE_PRESS: {
            if (ID == UI_BTN_EXIT) {
                setActionReturn(ACTION_BACK);
                return true;
            }
            else if (ID == UI_BTN_PLANEXY_div) {
                drawObject = 2;
            }
            else if (ID == UI_BTN_PLANEXY) {
                drawObject = 1;
            }
            else if (ID == UI_BTN_CUBE) {
                drawObject = 0;
            }
            else {
                RenderSystem *render = RenderSystem::getSingleton();
                ShaderManager *shaderManager = render->shaderManager;

                for (unsigned int i = 0; i < UI_ID.size(); i++) {
                    if (UI_ID[i] == ID) {
                        shaderName = shaderNames[i];

                        //shaderManager->setActiveShader(shaderNames[i]);
                        postProcessingMode = shaderManager->isPostProcessing(shaderNames[i]);
                    }
                }
            }
            //hover->play();
            break;
        }
        case LERP_CHANGED: {
            if (ID == UI_SLIDER_1) {
                char tmp[1024];
                sprintf(tmp, "%03.0f", e.lerpChanged.newLerp*100.0f);
                ((Label*)getInterface(UI_LABEL_1))->setText(tmp);

                setLerp1(e.lerpChanged.newLerp);
            }
            else if (ID == UI_SLIDER_2) {
                char tmp[1024];
                sprintf(tmp, "%03.0f", e.lerpChanged.newLerp*100.0f);
                ((Label*)getInterface(UI_LABEL_2))->setText(tmp);

                setLerp2(e.lerpChanged.newLerp);
            }
            break;
        }
        case ET_NON_SPECIFIED:case MOUSE_RELEASE:case KEY_PRESS:case KEY_RELEASE:case SET_FOCUS:
            break;
        }
        return true;
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {

        RenderSystem *render = RenderSystem::getSingleton();
        ShaderManager *shaderManager = render->shaderManager;


        glEnable(GL_DEPTH_TEST);

        render->projection.top = projection_perspective_rh_negative_one( 45.0f, (float)w / (float)h, 0.01f, 100);

        render->model.top =
            translate(0, 0, -5 + distance) *
            rotate(DEG2RAD(cameraRotationAngle.y), 1, 0, 0) *
            rotate(DEG2RAD(cameraRotationAngle.x), 0, 1, 0);


        //shaderManager->setupShaderParametersAndDraw();

        if (postProcessingMode) {

            shaderManager->framebuffer->enable();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderManager->setActiveShader("GLShaderTextureColor");
            switch (drawObject) {
                case 0:
                    drawCube();
                    break;//cube
                case 1:
                    drawXYplane();
                    break;//planexy
                case 2:
                    drawXYplane(20);
                    break;//planexy_div
            }

            //shaderManager->framebuffer->copyFrameBuffer();

            GLFramebufferObject::disable();

            shaderManager->setActiveShader(shaderName);

            glDisable(GL_DEPTH_TEST);

            render->projection.top = mat4::IdentityMatrix;
            render->model.top = mat4::IdentityMatrix;

            drawNormalizedMappedQuad();

        }
        else {

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderManager->setActiveShader(shaderName);

            switch (drawObject) {
            case 0:
                drawCube();
                break;//cube
            case 1:
                drawXYplane();
                break;//planexy
            case 2:
                drawXYplane(20);
                break;//planexy_div
            }
        }


        static unsigned int frameCount = 0;
        frameCount++;
        static unsigned int lastTime = 0;
        static float fps = 0;
        if (diffSystime(lastTime, time_ms) > 1000) {
            lastTime = time_ms;
            fps = (fps + (float)frameCount)*0.5;
            frameCount = 0;
        }

        GUIGroup::render(time_ms, false);

        render->projection.top = mat4::IdentityMatrix;
        render->model.top = translate(-1, -1, 0) * scale(screenRes_inv.x, screenRes_inv.y, 1);

        char txt[32];
        sprintf(txt, "fps: %.2f", fps);

        //render->print(font, txt, vec2(w / 2, h - font->getLineHeight() - 20), vec4(1.0f), 0.5f, 0.0f, 1.0f);
        render->print(font, txt, vec2(w / 2.0, +font->getLineHeight() + 20), vec4(1.0f), 0.5f, 0.0f, 0.25f);

        render->drawTexture_center(cursor, mousepos);

        glEnable(GL_DEPTH_TEST);
    }
    bool keyDown(const int k) {
        GUIGroup::keyDown(k);
        if (k == sf::Keyboard::Escape|| k == sf::Keyboard::Return)
            setActionReturn(ACTION_BACK);
        if (k == sf::Keyboard::Add || k == sf::Keyboard::Equal)
            distance += 1;
        else if (k == sf::Keyboard::Subtract || k == sf::Keyboard::Dash)
            distance -= 1;

        /*
        if (k == sf::Keyboard::P){
            RenderSystem *render = RenderSystem::getSingleton();
            ShaderManager *shaderManager = render->shaderManager;
            shaderManager->framebuffer->copyFrameBuffer();
            std::string name = shaderName;
            name.erase(std::remove(name.begin(), name.end(), '\n'), name.end());
            shaderManager->framebuffer->writeToPNG(( name + std::string(".png")).c_str());
        }
         */

        return true;
    }
    void setMousePos(const vec2 &pos) {
        GUIGroup::setMousePos(pos);
        if (track) {
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
    bool mouseDown(const int btn, const vec2 &pos) {
        if (btn == 3)//wheel up
            distance += 0.4;
        if (btn == 4)//wheel down
            distance -= 0.4;
        if (!GUIGroup::mouseDown(btn, pos)) {
            track = true;
            mouseTracker = pos;
        }
        return true;
    }
    bool mouseUp(const int btn, const vec2 &pos) {
        GUIGroup::mouseUp(btn, pos);
        track = false;
        return true;
    }

    SSE2_CLASS_NEW_OPERATOR
};



class MainMenu : public GUIGroup {
    GLTexture *cursor;
    GLFont *font;
    int UI_BTN_EXIT,
        UI_BTN_ILUMINATION,
        UI_BTN_DETAIL,
        UI_BTN_RADIOMETRIC,
        UI_BTN_FILTERS,
        UI_BTN_TEXTURE,

        UI_BTN_PLANEXY_div,
        UI_BTN_PLANEXY,
        UI_BTN_CUBE;

    //AlMultiSource *hover;
public:
    MainMenu(GLTexture *cursor, GLFont *fnt) {
        font = fnt;
        this->cursor = cursor;
    }
    void resizeWindow(const int w, const int h) {
        if (this->w == w && this->h == h) return;
        GUIGroup::resizeWindow(w, h);
        clearAll();

        vec2 position = vec2(w, h)*0.5 + vec2(0, font->getLineHeight() * 0.5f)*3.5;// 2.5;

        UI_BTN_ILUMINATION = addLabelBtn(font, "Ilumination", position, vec2(200, 0), true, true, 0.5f);
        position = position + vec2(0, -font->getLineHeight() / 2 - 20);

        UI_BTN_DETAIL = addLabelBtn(font, "Detail", position, vec2(200, 0), true, true, 0.5f);
        position = position + vec2(0, -font->getLineHeight() / 2 - 20);

        UI_BTN_RADIOMETRIC = addLabelBtn(font, "Radiometric", position, vec2(200, 0), true, true, 0.5f);
        position = position + vec2(0, -font->getLineHeight() / 2 - 20);

        UI_BTN_FILTERS = addLabelBtn(font, "Filters", position, vec2(200, 0), true, true, 0.5f);
        position = position + vec2(0, -font->getLineHeight() / 2 - 20);

        UI_BTN_TEXTURE = addLabelBtn(font, "Texture", position, vec2(200, 0), true, true, 0.5f);
        position = position + vec2(0, -font->getLineHeight() / 2 - 20);

        UI_BTN_EXIT = addLabelBtn(font, "EXIT", position, vec2(200, 0), true, true, 0.5f);

    }
    bool performAction(const int ID, Event &e) {
        //    if(ID == label2) return false;
        switch (e.type) {
        case MOUSE_PRESS:
            if (ID == UI_BTN_EXIT) setActionReturn(ACTION_EXIT);
            if (ID == UI_BTN_ILUMINATION) setActionReturn(ACTION_SDR1);
            if (ID == UI_BTN_DETAIL) setActionReturn(ACTION_SDR2);
            if (ID == UI_BTN_RADIOMETRIC) setActionReturn(ACTION_SDR3);
            if (ID == UI_BTN_FILTERS) setActionReturn(ACTION_SDR4);
            if (ID == UI_BTN_TEXTURE) setActionReturn(ACTION_SDR5);
            //hover->play();
            break;
        default: break;
        }
        return true;
    }
    void render(const unsigned int time_ms, const bool showCursor = true) {

        RenderSystem *render = RenderSystem::getSingleton();
        ShaderManager *shaderManager = render->shaderManager;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        static unsigned int frameCount = 0;
        frameCount++;
        static unsigned int lastTime = 0;
        static float fps = 0;
        if (diffSystime(lastTime, time_ms) > 1000) {
            lastTime = time_ms;
            fps = (fps + (float)frameCount)*0.5;
            frameCount = 0;
        }

        render->projection.top = mat4::IdentityMatrix;
        render->model.top = translate(-1, -1, 0) * scale(screenRes_inv.x, screenRes_inv.y, 1);


        GUIGroup::render(time_ms, false);
        const char* str = "Program to demonstrate some shaders effects\n"
            "Author: Alessandro Ribeiro da Silva\n"
            "Site: http://alessandroribeiro.thegeneralsolution.com\n"
            "Email: alessandro.ribeiro.silva@gmail.com";


        render->print(font, str, vec2(w / 2.0, h / 2.0 + font->getLineHeight()*5.5f*0.5f), vec4(1.0f), 0.5f, 0.0f, 0.4f);

        char txt[32];
        sprintf(txt, "fps: %.2f", fps);

        render->print(font, txt, vec2(w / 2.0, + font->getLineHeight() + 20), vec4(1.0f), 0.5f, 0.0f, 0.25f);

        render->shader->enable();
        render->shader->setColor(vec4(1.0f));
        render->drawTexture_center(cursor, mousepos);
        glEnable(GL_DEPTH_TEST);
    }
    bool keyDown(const int k) {
        GUIGroup::keyDown(k);
        if (k == sf::Keyboard::Escape || k == sf::Keyboard::Return)
            setActionReturn(ACTION_EXIT);
        return true;
    }
    void setMousePos(const vec2 &pos) {
        GUIGroup::setMousePos(pos);
    }
    bool mouseDown(const int btn, const vec2 &pos) {
        GUIGroup::mouseDown(btn, pos);
        return true;
    }
    bool mouseUp(const int btn, const vec2 &pos) {
        GUIGroup::mouseUp(btn, pos);
        return true;
    }

    SSE2_CLASS_NEW_OPERATOR

};

class Game {
    vec2 cursorPos,
        dimension;
    int w, h;
    GLTexture *cursor;
    Interface *activeInterface;
    GLFont *font;

public:
    Game(int w, int h) {
        cursor = GLTexture::loadFromFile("resources/images/cursor.png");

        //load font
        {
            Font* font_raw = Font::LoadFromBasof("resources/font/BenjaminGothic.basof");
            font = font_raw->createGLFont();
            setNullAndDelete(font_raw);
        }


        dimension = vec2(w, h);
        this->w = w;
        this->h = h;


        RenderSystem::getSingleton();

        //glPixelZoom(0.4,0.4);
        activeInterface = new MainMenu(cursor, font);//new ShaderMenu(&cursor, &hover, &font);

        resizeWindow(w, h);
    }
    ~Game() {
        if (activeInterface != NULL) delete activeInterface;
        activeInterface = NULL;

        setNullAndDelete(cursor);
        setNullAndDelete(font);

    }
    void resizeWindow(const int w, const int h) {
        dimension = vec2(w, h);
        this->w = w;
        this->h = h;
        glViewport(0, 0, w, h);
        if (activeInterface != NULL)
            activeInterface->resizeWindow(w, h);
    }
    void idleDraw(const unsigned int time_ms) {
        if (activeInterface != NULL) {
            int actionID = activeInterface->actionReturn();
            if (actionID >= 0) {
                //pode fazer alguma coisa com o gerenciador de interfaces
                activeInterface->setActionReturn(-1);//reset do action antigo
                //trocar de interface

                switch (actionID) {
                case ACTION_SDR1:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ShaderMenu(cursor, font, "ilumination");
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_SDR2:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ShaderMenu(cursor, font, "detail");
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_SDR3:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ShaderMenu(cursor, font, "radiometric");
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_SDR4:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ShaderMenu(cursor, font, "filters");
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_SDR5:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new ShaderMenu(cursor, font, "texture");
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_BACK:
                    if (activeInterface != NULL) delete activeInterface;
                    activeInterface = NULL;
                    activeInterface = new MainMenu(cursor, font);
                    activeInterface->resizeWindow(w, h);
                    activeInterface->setMousePos(cursorPos);
                    break;
                case ACTION_EXIT:
                    canExitApp = true;
                    break;
                }

            }
            activeInterface->render(time_ms);
        }
    }
    void keyDown(const int key) {
        if (activeInterface != NULL)
            activeInterface->keyDown(key);
    }
    void keyUp(const int key) {
        if (activeInterface != NULL)
            activeInterface->keyUp(key);
    }
    void mouseDown(const int btn) {
        if (activeInterface != NULL)
            activeInterface->mouseDown(btn, cursorPos);
    }
    void mouseUp(const int btn) {
        if (activeInterface != NULL)
            activeInterface->mouseUp(btn, cursorPos);
    }
    void mouseMove(const int x, const int y) {
        //cursorPos = clamp(cursorPos + vec2(x,y), vec2(0,0), dimension);
        cursorPos = vec2(x, dimension.y - y);
        if (activeInterface != NULL)
            activeInterface->setMousePos(cursorPos);
    }

    SSE2_CLASS_NEW_OPERATOR
};

#endif
