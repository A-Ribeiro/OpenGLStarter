
#include "pauseState.h"
#include <stdio.h>

PauseState::PauseState(SpaceInvader *si) :starsDraw(si) {

    spaceInvader = si;

    //this->font = font;
  //  fontWidth = font->getFontWidth("PAUSE");
}
void PauseState::resizeWindow(int w, int h) {
}
void PauseState::printState() {
    printf("  PAUSE STATE\n");
}
void PauseState::render(float elapsedTime, const vec2 &screencenter) {

    //glDepthMask(GL_FALSE);

    spaceInvader->render->model.top = translate(0, 0, -6);


    static float nt = 0;
    nt += elapsedTime * 0.1f;
    while (nt > 2 * PI)
        nt -= 2 * PI;


    //  printf("\r nt: %f\t",nt);
    starsDraw.customDraw(vec2(sin(nt), cos(nt)));

    spaceInvader->render->pushProjetionAndConfigureParallel();

    spaceInvader->render->model.top = mat4::IdentityMatrix;

    glDisable(GL_DEPTH_TEST);

    const wchar_t *str =
        L"PAUSE\n"
        L"\"ESC\" to continue game\n"
        L"\"m\" back to menu";


    AABB aabb = spaceInvader->resources->font->computeBounds(str);


    GLShaderFont *shaderfont = spaceInvader->resources->font->getShader();
    shaderfont->enable();
    shaderfont->setMatrix(spaceInvader->render->getMVP()*translate(vec3(screencenter, 0.0f) - (aabb.min_box + aabb.max_box) * 0.5f));
    spaceInvader->resources->font->print(vec4(1.0f), str);

    spaceInvader->render->popProjection();


    //glDepthMask(GL_TRUE);

}
