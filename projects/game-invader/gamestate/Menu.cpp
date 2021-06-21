
#include "Menu.h"
#include <stdio.h>

#include "../SpaceInvader.h"

//#include "../OpenALCalls.h"




//#include "../textureHelper.h"
//#include <math.h>
//#include "../math/all.h"

//#include "../OpenAL/OpenALCalls.h"
//------------------------------------------------------------------------------
const char* menuStateToStr(MENU_STATES state) {
    switch (state) {
    case MS_NEW_GAME:return "new game";
    case MS_LOAD_GAME:return "load game";
    case MS_SCORES:return "scores";
    case MS_EXIT_GAME:return "exit game";
    default: break;
    }
    return "unknown state";
}
//------------------------------------------------------------------------------
void Menu::parser(MENU_TOKENS token) {
    //const int intervalLerpMenu = 100;
    switch (menuState) {
        //MS_NEW_GAME
    case  MS_NEW_GAME:
        switch (token) {
        case MT_NEXT_OPTION:
            spaceInvader->resources->sndPlayMenuChange();

            currentTimef = 0.0f;
            //initialTime = currentTime;
            menuState = MS_EXIT_GAME;//MS_LOAD_GAME;
            //finalTime = initialTime + intervalLerpMenu;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_EXIT_GAME];//MS_LOAD_GAME];
            return;
        case MT_ESCAPE:
            spaceInvader->resources->sndPlayMenuChange();
            menuState = MS_EXIT_GAME;

            currentTimef = 0.0f;

            //finalTime = initialTime + intervalLerpMenu;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_EXIT_GAME];
            return;
        default: break;
        }
        return;
        //MS_LOAD_GAME
    case MS_LOAD_GAME:
        switch (token) {
        case MT_NEXT_OPTION:
            //initialTime = currentTime;
            menuState = MS_EXIT_GAME;
            //finalTime = initialTime + intervalLerpMenu;

            currentTimef = 0.0f;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_EXIT_GAME];
            return;
        case MT_PREV_OPTION:
            //initialTime = currentTime;
            currentTimef = 0.0f;
            menuState = MS_NEW_GAME;
            //finalTime = initialTime + intervalLerpMenu;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_NEW_GAME];
            return;
        case MT_ESCAPE:
            menuState = MS_EXIT_GAME;
            //finalTime = initialTime + intervalLerpMenu;
            currentTimef = 0.0f;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_EXIT_GAME];
            return;
        default: break;
        }
        return;
        //MS_EXIT_GAME
    case MS_EXIT_GAME:
        switch (token) {
        case MT_PREV_OPTION:
            spaceInvader->resources->sndPlayMenuChange();
            //initialTime = currentTime;
            menuState = MS_NEW_GAME;//MS_LOAD_GAME;
            //finalTime = initialTime + intervalLerpMenu;
            currentTimef = 0.0f;
            initDraw = drawPos;
            finishDraw = ItemMenuPos[MS_NEW_GAME];//MS_LOAD_GAME];
            return;
        default: break;
        }
        return;
    default: break;
    }
}
//------------------------------------------------------------------------------
//#include "../menuResource/allImgs.hpp"
//------------------------------------------------------------------------------
Menu::Menu(SpaceInvader *si) :starsDraw(si) {

    spaceInvader = si;

    textureStates[MS_NEW_GAME] = spaceInvader->resources->NewGameImg;
    textureStates[MS_LOAD_GAME] = spaceInvader->resources->LoadGameImg;
    textureStates[MS_SCORES] = spaceInvader->resources->ScoreImg;
    textureStates[MS_EXIT_GAME] = spaceInvader->resources->ExitGameImg;

    /*
  textureStates[MS_NEW_GAME]  = createTexture(256,256, NewGameImg);
  textureStates[MS_LOAD_GAME] = createTexture(256,256, LoadGameImg);
  textureStates[MS_SCORES] = createTexture(256,256, ScoreImg);
  textureStates[MS_EXIT_GAME] = createTexture(256,256, ExitGameImg);

  Titulo = createTexture(256,256,TituloImg);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  OpenALSymbol = createTexture(256,256,OpenALImg);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  */

    menuState = MS_NEW_GAME;

    drawPos = initDraw = finishDraw = vec2(0, 0);
    //initialTime = finalTime = 0;

    currentTimef = 0.0f;
}
//------------------------------------------------------------------------------
Menu::~Menu() {
    /*
  freeTexture(textureStates[MS_NEW_GAME]);
  freeTexture(textureStates[MS_LOAD_GAME]);
  freeTexture(textureStates[MS_SCORES]);
  freeTexture(textureStates[MS_EXIT_GAME]);
  freeTexture(Titulo);
  freeTexture(OpenALSymbol);
  */
}
//------------------------------------------------------------------------------
//retorna true se a token de entrada foi um OK
bool Menu::inputMenu(MENU_TOKENS token) {
    if (token == MT_OK) return true;
    parser(token);
    return false;
}
//------------------------------------------------------------------------------
MENU_STATES Menu::getMenuState() {
    return menuState;
}
//------------------------------------------------------------------------------
void Menu::printState() {
    printf("  MENU OBJECT\n");
    printf("       menuState = %s\n", menuStateToStr(menuState));
}
//------------------------------------------------------------------------------
void Menu::render(float deltaTime) {

    glDisable(GL_DEPTH_TEST);

    const float intervalLerpMenu = 0.100f;

    currentTimef += deltaTime;

    if (currentTimef > intervalLerpMenu) {
        currentTimef = intervalLerpMenu;
    }

    drawPos = lerp(initDraw, finishDraw, currentTimef / intervalLerpMenu);



    spaceInvader->render->model.top *= translate(0, 0, -2);

    starsDraw.draw(deltaTime);

    //desenho das estrelas
    static float intensidade = 0.0f;// (starsDraw.getLastStarsIntensity() - 0.5)*0.3 + 0.6;

    intensidade += deltaTime * 0.1f;

    intensidade = fmod(intensidade, 2.0f);

    float intensity = 0.6f + 0.2f * absv (fmod(intensidade, 2.0f) - 1.0f);


    //desenho do menu
    //glColor3f(intensidade, intensidade, intensidade);
    spaceInvader->render->shader->enable();

    spaceInvader->render->shader->setColor(vec4(vec3(intensity),1.0f));

    spaceInvader->render->model.top *= scale(0.3, 0.3, 0.3) * translate(0, -1, 0);

    spaceInvader->render->model.push();

    //glPushMatrix();

    spaceInvader->render->model.top *= translate(-drawPos.x, -drawPos.y, 0);

    for (int i = 0; i < MS_TOTAL_STATES; i++) {
        spaceInvader->render->model.push();
        spaceInvader->render->model.top *= translate(ItemMenuPos[i].x, ItemMenuPos[i].y, 0);
        spaceInvader->render->drawTexture(textureStates[i], AABB(vec2(-1.0f), vec2(1.0f)));
        spaceInvader->render->model.pop();
    }
    //glPopMatrix();

    spaceInvader->render->model.pop();

    spaceInvader->render->shader->setColor(vec4(vec3(1.0f), 1.0f));

    //desenho do titulo
    spaceInvader->render->model.top *= translate(0, 1, 0)*scale(2, 2, 2);
    spaceInvader->render->drawTexture(spaceInvader->resources->TituloImg, AABB(vec2(-1.0f), vec2(1.0f)));

    //desenho do OpenAL
    spaceInvader->render->model.top *= translate(1.5, -1.5, 0)*scale(0.5, 0.5, 0.5);
    spaceInvader->render->drawTexture(spaceInvader->resources->OpenALImg, AABB(vec2(-1.0f), vec2(1.0f)));

    glEnable(GL_DEPTH_TEST);
}
//------------------------------------------------------------------------------
