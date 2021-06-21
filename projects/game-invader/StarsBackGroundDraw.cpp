#include "StarsBackGroundDraw.h"
#include "SpaceInvader.h"

//#include "textureHelper.h"
//#include <math.h>
//#include "math/all.h"
//#include "menuResource/allImgs.hpp"
//#include <SPD.h>

StarsBackGroundDraw::StarsBackGroundDraw(SpaceInvader * si){
    spaceInvader = si;

    // setup texture repeat
    spaceInvader->resources->StarsImg->active(0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    spaceInvader->resources->StarsImg->deactive(0);


//  dynStars = stars;
  speed = 1.0;
}
StarsBackGroundDraw::~StarsBackGroundDraw(){
  numberRef--;
  if (numberRef==0){
  }
}
//------------------------------------------------------------------------------
void StarsBackGroundDraw::draw(float deltaTime){
  static float deslocamento = 0;

  deslocamento += deltaTime * 0.001f;

  deslocamento = fmod(deslocamento, 1.0f);

  float toModify = deslocamento * speed * 5;

  GLShaderTextureColor *shader = spaceInvader->render->shader;
  shader->enable();
  shader->setColor(vec4(1.0f));

{


    vec3 vpos[] = { //post
                vec3(-5,-5, 0),
                vec3( 5,-5, 0),
                vec3( 5, 5, 0),

                vec3(-5,-5, 0),
                vec3( 5, 5, 0),
                vec3(-5, 5, 0)
          };

    vec2 vuv[] = { //uv
            vec2(0, 5) + toModify,
            vec2(5, 5) + toModify,
            vec2(5, 0) + toModify,

            vec2(0, 5) + toModify,
            vec2(5, 0) + toModify,
            vec2(0, 0) + toModify
        };


  spaceInvader->render->drawTexture(
      spaceInvader->resources->StarsImg,
      GL_TRIANGLES, vpos, vuv, 6 );

}

  //intensidade = absv( ( fmod(deslocamento * 10.0f,1.0f) ) * 2.0f - 1.0f );//-1 a 1
  //intensidade = intensidade*0.5+0.5;

  intensidade += deltaTime;
  float intensity = absv( fmod(intensidade, 2.0f) - 1.0f);
  intensidade = fmod(intensidade, 2.0f);

  intensity = intensity * 0.5 + 0.5;

  shader->setColor(vec4(vec3(intensity),1.0f));

  {

  vec3 vpos[] = {
        vec3(-5,-5, 0),
        vec3(5,-5, 0),
        vec3(5, 5, 0),

        vec3(-5,-5, 0),
        vec3(5, 5, 0),
        vec3(-5, 5, 0)
  };

  vec2 vuv[] = {
        vec2(0, 2) + toModify,
        vec2(2, 2) + toModify,
        vec2(2, 0) + toModify,

        vec2(0, 2) + toModify,
        vec2(2, 0) + toModify,
        vec2(0, 0) + toModify
  };

  spaceInvader->render->drawTexture(
      spaceInvader->resources->StarsImg,
      GL_TRIANGLES, vpos, vuv, 6 );

  }



}
//------------------------------------------------------------------------------
void StarsBackGroundDraw::customDraw(const vec2 &deslocamento){

    GLShaderTextureColor *shader = spaceInvader->render->shader;
    shader->enable();
    shader->setColor(vec4(1.0f));

    {
    vec3 vpos[] = { //post
              vec3(-5,-5, 0),
              vec3(5,-5, 0),
              vec3(5, 5, 0),

              vec3(-5,-5, 0),
              vec3(5, 5, 0),
              vec3(-5, 5, 0)
        };
    vec2 vuv[] = { //uv
            vec2(0, 5) + deslocamento,
            vec2(5, 5) + deslocamento,
            vec2(5, 0) + deslocamento,

            vec2(0, 5) + deslocamento,
            vec2(5, 0) + deslocamento,
            vec2(0, 0) + deslocamento
        };

    spaceInvader->render->drawTexture(
        spaceInvader->resources->StarsImg,
        GL_TRIANGLES, vpos, vuv, 6
    );

    }

    {

     vec3 vpos[] ={ //post
              vec3(-5,-5, 0),
              vec3(5,-5, 0),
              vec3(5, 5, 0),

              vec3(-5,-5, 0),
              vec3(5, 5, 0),
              vec3(-5, 5, 0)
        };

    vec2 vuv[] = { //uv
        vec2(0, 2) + deslocamento,
        vec2(2, 2) + deslocamento,
        vec2(2, 0) + deslocamento,

        vec2(0, 2) + deslocamento,
        vec2(2, 0) + deslocamento,
        vec2(0, 0) + deslocamento
    };

    spaceInvader->render->drawTexture(
        spaceInvader->resources->StarsImg,
        GL_TRIANGLES, vpos, vuv ,6
    );

    }
}

float StarsBackGroundDraw::getLastStarsIntensity(){
  return intensidade;
}

void StarsBackGroundDraw::setSpeed(float spd){
  speed = spd;
}
float StarsBackGroundDraw::getSpeed(){
  return speed;
}
