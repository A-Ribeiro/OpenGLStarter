
#include "scene.h"


SpaceInvader *invaderref = NULL;
GLTexture *textureref = NULL;
bool canGenerateShoot = false;

/*
GLuint xplodeTex[17],
        tiro,
        spaceship,
        enemy;
        */


void Scene::renderCongratulations(const vec2& screenCenter, float deltatime){

    invaderref->render->pushProjetionAndConfigureParallel();


    const wchar_t * str =
        L"You defeated all enemies! \n"
        L"Congratulations!!! \n"
        L"Press \"Enter\" to back to menu";

    invaderref->render->print(str, screenCenter, vec4(1.0f));

    invaderref->render->popProjection();

    glEnable(GL_DEPTH_TEST);

    invaderref->render->model.top *= translate(0, 0, -10);

  //glColor3f(1,1,1);
  static float deslocamento = 0;
  deslocamento += deltatime;// (float)time / 10.0;


  //glEnable(GL_TEXTURE_2D);
  //activeTexture(enemy);

  invaderref->render->model.top *=
      rotate(deslocamento, 1, 0, 0) *
      rotate(deslocamento, 0, 1, 0) *
      rotate(deslocamento * 2, 0, 0, 1);

  invaderref->render->shader->enable();
  invaderref->render->shader->setColor(vec4(1.0f));

  for (int i=-5;i<=5;i++){
    if (absv(i)<2) continue;

    invaderref->render->model.push();

    //glPushMatrix();

    invaderref->render->model.top *=
        translate(i * 2, sin(deslocamento / 10.0), 0) *
        rotate(-deslocamento * 2, 0, 0, 1) *
        rotate(-deslocamento, 0, 1, 0) *
        rotate(-deslocamento, 1, 0, 0);

    invaderref->render->drawTexture(invaderref->resources->enemyImg, AABB(vec2(0.0f), vec2(1.0f)));
    //drawTexturedQuad();

    //glPopMatrix();

    invaderref->render->model.pop();

    invaderref->render->model.push();

    invaderref->render->model.top *=
        translate(sin(deslocamento / 10.0), i * 2, 0) *
        rotate(-deslocamento * 2, 0, 0, 1) *
        rotate(-deslocamento, 0, 1, 0) *
        rotate(-deslocamento, 1, 0, 0);

    invaderref->render->drawTexture(invaderref->resources->enemyImg, AABB(vec2(0.0f), vec2(1.0f)));

    //drawTexturedQuad();

    invaderref->render->model.pop();
  }


  //PopAttrib();
}
