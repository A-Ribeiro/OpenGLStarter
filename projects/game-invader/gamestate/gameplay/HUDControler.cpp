
#include "HUDControler.h"

//#include "NavePrincipal.hpp"
#include <stdio.h>
#include "../../StarsBackGroundDraw.h"
//#include <GL/gl.h>
//#include "../../GLee/glee.h"
//#include "../../math/all.h"
//#include "../../textureHelper.h"
//#include "../../OglStateHelper.h"

#define initialNumberOfLife 5


//------------------------------------------------------------------------------
HUDcontroler::HUDcontroler(SpaceInvader * si) {
    this->spaceInvader = si;
    //this->w = si->render->dimension.x;// w;
    //this->h = h;
    numberOfLifes = initialNumberOfLife;
    score = 0;
}
//------------------------------------------------------------------------------
void HUDcontroler::setNumberOfLifes(int value) {
    numberOfLifes = value;
}
//------------------------------------------------------------------------------
void HUDcontroler::setScore(int value) {
    score = value;
}
void HUDcontroler::setLevel(int value) {
    level = value;
}
//------------------------------------------------------------------------------
void HUDcontroler::innerDraw(const vec2 &screensize) {
    //static char aux[80];
  //printf("%f\n",screensize.y);

    //spaceInvader->render->model.top *= scale(1, 1.0f / screensize.y, 0);

    {


    vec3 vpos[] = { //pos
            vec3(0,            screensize.y - 0.15*400.0, 0.0f),
            vec3(screensize.x, screensize.y - 0.15*400.0, 0.0f),
            vec3(screensize.x, screensize.y, 0.0f),

            vec3(0,            screensize.y - 0.15*400.0, 0.0f),
            vec3(screensize.x, screensize.y, 0.0f),
            vec3(0,            screensize.y, 0.0f)
        };
    vec4 vcolor[] = { //color
            vec4(0.5  ,0.5  ,1 ,0.5),
            vec4(0.5  ,0.5  ,1 ,0.5),
            vec4(1  ,1  ,1 ,0.5),

            vec4(0.5  ,0.5  ,1 ,0.5),
            vec4(1  ,1  ,1 ,0.5),
            vec4(1  ,1  ,1 ,0.5)
        };
    spaceInvader->render->drawColor(GL_TRIANGLES, vpos, vcolor, 6 );

    }

    static wchar_t str[512];
    swprintf(str,512,
        L"Lifes: %.2u", numberOfLifes);

    spaceInvader->render->print(str,
        vec2(10.0f, screensize.y - 0.0f),
        vec4(vec3(1.0f), 1.0f),
        0.0f, 1.0f);

    swprintf(str,512,
        L"score: %.4u     level: %.2u", score, level);

    spaceInvader->render->print(str,
        vec2(screensize.x - 400.0f, screensize.y - 0.0f),
        vec4(vec3(1.0f), 1.0f),
        0.0f, 1.0f);

    /*
    spaceInvader->render->model.top *= translate(vec3(vec2(10.0f, screensize.y - 50.0f), 0.0f));

    spaceInvader->render->drawAABB(
        AABB(vec2(-10.0f, -10.0f), vec2(10.0f, 10.0f)),
        vec4(0.0f, 1.0f, 1.0f, 1.0f)
    );
    */

    /*
  glColor3f(0,0,0);


  font->printf(50, PM_TOP_BEGIN_FROM_UPPER ,
                     vec2(10,screensize.y-5),
                     "Lifes: %.2u",numberOfLifes );

  font->printf(50, PM_TOP_BEGIN_FROM_UPPER | PM_RIGHT,
                     vec2(screensize.x-50,screensize.y-5),
                     "score: %.4u     level: %.2u",score, level);

                     */
                     /*
                       glTranslatef(-1,0.9,0);

                       sprintf(aux,"Lifes: %.2u",numberOfLifes);
                       font->print(aux,0.1,vec3(1,0,0),vec3(0,0,0));
                       glTranslatef(0.75,0,0);
                       sprintf(aux,"score: %.4u",score);
                       font->print(aux,0.1,vec3(0,0,0),vec3(0,0,0));
                       glTranslatef(0.75,0,0);
                       sprintf(aux,"level: %.2u",level);
                       font->print(aux,0.1,vec3(0,0,0),vec3(0,0,0));
                     //*/
}
//------------------------------------------------------------------------------
void HUDcontroler::render(float deltaTime,const vec2 &screensize) {
    /*
     esta chamada apenas configura o OpenGL para que ele tenha a tela normalizada

    */
    glDisable(GL_DEPTH_TEST);
    spaceInvader->render->pushProjetionAndConfigureParallel();

    innerDraw(screensize);

    spaceInvader->render->popProjection();

    glEnable(GL_DEPTH_TEST);
}
//------------------------------------------------------------------------------


