
#include "playState.h"
#ifndef playState_hpp


//#include "../Font3d.h"
//#include "./gamePlay/NavePrincipal.hpp"
#include <stdio.h>

//#include <GL/gl.h>
//#include "../GLee/glee.h"


//------------------------------------------------------------------------------
PlayState::PlayState(SpaceInvader *si) :
    HUD(si),
    starsDraw(si),
    scene(si)
    // ,postEffectRender(w,h)
{
    this->spaceInvader = si;
    BackGroundColor1 = vec4(0.0, 0.5, 0.5, 1.0);
    BackGroundColor2 = vec4(0.0, 0.0, 0.0, 1.0);
    //  starsDraw.setSpeed(4);
}
//------------------------------------------------------------------------------
void PlayState::resizeWindow(int w, int h) {
    //  postEffectRender.windowResize(w,h);
      /*
        tem que atualizar pelo menos o processador de efeitos pos processados
      */
    scene.resizeWindow(w,h);
}
//------------------------------------------------------------------------------
void PlayState::simulationTick() {
    scene.simulationTick();
}
//------------------------------------------------------------------------------
void PlayState::printState() {
    printf("  PLAY STATE\n");
    scene.printState();
}
//------------------------------------------------------------------------------
void PlayState::render(float deltatime, const vec2 &screensize) {

    //  postEffectRender.beginRender();
    glDisable(GL_DEPTH_TEST);

    spaceInvader->render->pushProjetionAndConfigureParallel();
    vec2 dim = spaceInvader->render->dimension;

    vec3 vpos[] = { //pos
            vec3(0,0, 0.0f),
            vec3(dim.x,0, 0.0f),
            vec3(dim.x,dim.y, 0.0f),

            vec3(0,0, 0.0f),
            vec3(dim.x,dim.y, 0.0f),
            vec3(0,dim.y, 0.0f)
        };
    vec4 vcolor[] = { //color
            BackGroundColor1,
            BackGroundColor1,
            BackGroundColor2,

            BackGroundColor1,
            BackGroundColor2,
            BackGroundColor2
        };

    spaceInvader->render->drawColor(GL_TRIANGLES, vpos, vcolor, 6);

    spaceInvader->render->popProjection();

    spaceInvader->render->model.top *= translate(0, 0, -2);
    starsDraw.draw(deltatime);

    glEnable(GL_DEPTH_TEST);


    scene.render(deltatime);


    //  postEffectRender.saveRender();
    //  postEffectRender.makeBlur(0.01);

    //  glDisable(GL_TEXTURE_2D);
    //  postEffectRender.negativeDraw(1.0);

    //  postEffectRender.endRender();

    //  glClear(GL_COLOR_BUFFER_BIT);
    //  postEffectRender.drawFullScreenRenderedTexture2();
    //  postEffectRender.drawFullScreenRenderedTexture();



    HUD.setNumberOfLifes(scene.spaceShipList.size());
    HUD.setScore(scene.score);
    HUD.setLevel(level);
    HUD.render(deltatime, screensize);

    glDisable(GL_DEPTH_TEST);


    spaceInvader->render->pushProjetionAndConfigureParallel();

    if (scene.getglobalLerpOn()) {

        spaceInvader->render->print(L"wait", screensize*0.5, vec4(1.0f));

        /*
      //glColor3f(1,1,1);
      font->staticPrint(PM_VERTICAL_CENTER|PM_HORIZONTAL_CENTER,
                       screensize*0.5,
                        "wait",strlen("wait"));
      */
      //    font->print("wait",0.1,vec3(-2,0,0),vec3(0,0,0));
    }
    else if (scene.getSpaceShipObject() != NULL) {
        if (!scene.getSpaceShipObject()->active) {
            //glColor3f(1,1,1);

            spaceInvader->render->print(L"you have a spaceship. Press \"Enter\"", screensize*0.5, vec4(1.0f));

            /*
          font->staticPrint(PM_VERTICAL_CENTER|PM_HORIZONTAL_CENTER,
                         screensize*0.5,
                          "you have a spaceship. Press \"Enter\"",strlen("you have a spaceship. Press \"Enter\""));
          */
          //      font->print("you have a spaceship. Press \"Enter\"",0.1,vec3(-7,0,0),vec3(0,0,0));
        }
        else {

            vec2 dim = spaceInvader->render->dimension;

            float barrPosition = scene.getSpaceShipLerp() * dim.x;

            {
            vec3 vpos[] = { //post
                vec3(0,0,0),
                vec3(dim.x,0,0),
                vec3(dim.x,dim.y * 0.0525,0),

                vec3(0,0,0),
                vec3(dim.x,dim.y * 0.0525,0),
                vec3(0,dim.y * 0.0525,0)
            };
            vec4 vcolor[] = { //color
                vec4(0.5f,0.5f,0.5f,0.5f),
                vec4(0.5f,0.5f,0.5f,0.5f),
                vec4(0.5f,0.5f,0.5f,0.5f),

                vec4(0.5f,0.5f,0.5f,0.5f),
                vec4(0.5f,0.5f,0.5f,0.5f),
                vec4(0.5f,0.5f,0.5f,0.5f)
            };
            spaceInvader->render->drawColor(GL_TRIANGLES, vpos, vcolor, 6);
            }


            {
            vec3 vpos[] = { //post
                vec3(dim.x*0.5f, 0, 0),
                vec3(dim.x*0.5f,dim.y * 0.0525, 0),

                vec3(barrPosition,0, 0),
                vec3(barrPosition,dim.y * 0.0525,0)
            };
            vec4 vcolor[] = { //color
                vec4(0.5f,0.5f,0.0f,1.0f),
                vec4(0.5f,0.5f,0.0f,1.0f),
                vec4(1.0f,0.0f,0.0f,1.0f),
                vec4(1.0f,0.0f,0.0f,1.0f)
            };
            spaceInvader->render->drawColor(GL_LINES, vpos, vcolor, 4);
            }

            /*

            glColor4f(0.5,0.5,0.5,0.5);
            glBegin(GL_QUADS);
                glVertex2f(-1,-1);
                glVertex2f( 1,-1);
                glVertex2f( 1,-0.85);
                glVertex2f(-1,-0.85);
            glEnd();

            glLineWidth(4.0);
            glBegin(GL_LINES);
              glColor3f(0.5,0.5,0);
              glVertex2f( 0, -1);
              glVertex2f( 0, -0.85);
              glColor3f(1,0,0);
              glVertex2f( lerp(-1,1,barrPosition), -1);
              glVertex2f( lerp(-1,1,barrPosition), -0.85);
            glEnd();

            */


        }
    }
    else {
        //glColor3f(1,1,1);

        spaceInvader->render->print(L"gameOver", screensize*0.5, vec4(1.0f));

        /*
      font->staticPrint(PM_VERTICAL_CENTER|PM_HORIZONTAL_CENTER,
                       screensize*0.5,
                        "gameOver",strlen("gameOver"));
                        */
                        //    font->print("gameOver",0.1,vec3(-1.2,0,0),vec3(0,0,0));
    }

    spaceInvader->render->popProjection();

    glEnable(GL_DEPTH_TEST);
}
//------------------------------------------------------------------------------
/*
  reinicia os componentes deste playstate atual
*/
void PlayState::resetPlayState() {
    scene.clear();
    level = 1;
}
bool PlayState::resetPlayStateToNextLevel() {
    int score = scene.score;
    scene.clear();
    scene.score = score;
    level++;
    return level >= 11;
}
//------------------------------------------------------------------------------
/*
  cria o playstate de acordo com um n�vel
     1 - espa�o
     2 - espa�o perto de algum planeta
     3 - ...
     4 - terra
     5 - morte de todos os terraquio
*/
void PlayState::createPlaystate() {
    scene.createEnemy(0.01 + 0.005*(float)level,//double enemy_Velocity,
        0.1 + 0.0025*(float)level,//double down_Velocity,
        3,//int Nl,
        7,//int Nc,
        0.2,//double vspc,
        0.2);//double hspc
    scene.createSpaceShip(3);//int numberOfLifes
    scene.fatorNumeroTiro = (level * 3) / 2 + 15;
}
//------------------------------------------------------------------------------
void PlayState::shoot() {
    scene.shootShip();
}
//------------------------------------------------------------------------------
void PlayState::moveSpaceShip(float velocity) {
    scene.incVelShip(velocity);
}
//------------------------------------------------------------------------------
#endif
