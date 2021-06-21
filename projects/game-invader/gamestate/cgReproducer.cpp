
#include "cgReproducer.h"
#include <stdio.h>


CGReproducer::CGReproducer(SpaceInvader *si) :starsDraw(si) {
    this->spaceInvader = si;
}
CGReproducer::~CGReproducer() {
}
void CGReproducer::resizeWindow(int w, int h) {
}
void CGReproducer::loadAnim() {
}

void CGReproducer::render(float elapsedTime, const vec2 &screenCenter) {
    /*
  glLoadIdentity();
  glPushAttrib(GL_ENABLE_BIT);
  glDepthMask(GL_FALSE);
  glEnable(GL_TEXTURE_2D);
  */

    //glDepthMask(GL_FALSE);

    spaceInvader->render->model.top = translate(0, 0, -1);

    static float nt = 0;
    nt += elapsedTime * 0.1f;
    while (nt > 2 * PI)
        nt -= 2 * PI;

    //  printf("\r nt: %f\t",nt);


    spaceInvader->render->drawTexture(
        spaceInvader->resources->spaceshipImg,
        AABB(vec2(0, 0 + nt * 0.22) + vec2(0.5, -0.7), vec2(0.1, 0.1 + nt * 0.22) + vec2(0.5, -0.7))
    );

    starsDraw.customDraw(vec2(sin(nt), cos(nt)));

    //spaceInvader->render->model.top = translate(0, 0, -5);

    spaceInvader->render->pushProjetionAndConfigureParallel();

    spaceInvader->render->model.top = mat4::IdentityMatrix;

    glDisable(GL_DEPTH_TEST);

    static wchar_t str[512];
    swprintf(str,512,
        L"You Score = %.4d\n"
        L"Next Level = %.2d\n"
        L"\n\nTo start play\n"
        L"press \"ENTER\"",
        score, nextLevel);

    spaceInvader->render->print(str, screenCenter, vec4(0.0f,1.0f,1.0f,1.0f));

    /*
    spaceInvader->render->model.top *= translate(vec3(screenCenter, 0.0f));
    spaceInvader->render->drawAABB(
        AABB(vec2(-10.0f, -10.0f), vec2(10.0f, 10.0f)),
        vec4(0.0f, 1.0f, 1.0f, 1.0f)
    );
    */



    /*
    AABB aabb = spaceInvader->resources->font->computeBounds(str);

    GLShaderFont *shaderfont = spaceInvader->resources->font->getShader();
    shaderfont->enable();
    shaderfont->setMatrix(spaceInvader->render->getMVP()*translate(vec3(screenCenter, 0.0f) - (aabb.min_box + aabb.max_box) * 0.5f));
    spaceInvader->resources->font->print(vec4(1.0f), str);
    */

    /*
  font->printf(200, PM_VERTICAL_CENTER|PM_HORIZONTAL_CENTER,
                     screenCenter,
                     "You Score = %.4d\n"
                     "Next Level = %.2d\n"
                     "\n\nTo start play\n"
                     "press \"ENTER\"",
                     score, nextLevel);
  */

    glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    spaceInvader->render->popProjection();

}

bool CGReproducer::cgReproducerIsTerminated() {
    return true;
}

void CGReproducer::printState() {
    printf("  CGReproducer\n");
    printf("       elapsed Time = \n");
    printf("       total time = \n");
    printf("       video Ref = \n");
}

void CGReproducer::setScoreLeve(int score, int level) {
    this->score = score;
    this->nextLevel = level;
}
