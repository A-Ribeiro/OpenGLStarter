#include "GUIGroup.h"


/*
#define EXCEPT(message)                     \
{                                           \
  fprintf(stderr, "Error in file: [%s line %d] ->> %s\n",  \
          __FILE__, __LINE__, message);     \
  exit(-1); \
}
*/

#define EXCEPT(message)\
    ARIBEIRO_ABORT(true,"%s\n",message);

#define DEBUG_PRINT(msg)


void GUIGroup::clearAll() {
    for (unsigned int i = 0; i < UIs.size(); i++)
        delete UIs[i];
    UIs.resize(0);
}
GUIGroup::GUIGroup() {
    w = 0;
    h = 0;
}

GUIGroup::~GUIGroup() {
    clearAll();
}
int GUIGroup::addSlider(const vec2 &min,
    const vec2 &max,
    const float distanceToConsider,
    GLTexture *bolinha) {
    DEBUG_PRINT("[GUIGroup] ADDING SLIDER");
    UIs.push_back(new Slider(this, min, max, distanceToConsider, bolinha));
    return (int)UIs.size() - 1;
}
int GUIGroup::addLabel(GLFont *f,
    const char*text,
    const vec2 &pos,
    const bool centerV, const bool centerH,
    float size) {
    DEBUG_PRINT("[GUIGroup] ADDING LABEL");
    UIs.push_back(new Label(this, f, text, pos, centerV, centerH, size));
    return (int)UIs.size() - 1;
}
int GUIGroup::addLabelBtn(GLFont *f,
    const char*text,
    const vec2 &pos, const vec2 &dim,
    const bool centerV, const bool centerH,
    float size) {
    DEBUG_PRINT("[GUIGroup] ADDING LABEL--BUTTON");
    UIs.push_back(new LabelBtn(this, f, text, pos, dim, centerV, centerH, size));
    return (int)UIs.size() - 1;
}
InterfaceGuiGroup* GUIGroup::getInterface(const int i)const {
    if (i<0 || i>(int)UIs.size())
        EXCEPT("Invalid index access!!!!");
    return UIs[i];
}
void GUIGroup::setMousePos(const vec2 &pos) {
    mousepos = pos;
    for (unsigned int i = 0; i < UIs.size(); i++)
        UIs[i]->setMousePos(i, pos);
}
void GUIGroup::resizeWindow(const int w, const int h) {
    this->w = w;
    this->h = h;
    centerScreen = vec2((float)w, (float)h)*0.5f;
    screenRes_inv = 1.0f / centerScreen;
    for (unsigned int i = 0; i < UIs.size(); i++)
        UIs[i]->resizeWindow(i, w, h);
}
void GUIGroup::render(const unsigned int time_ms, const bool showCursor) {

    RenderSystem *render = RenderSystem::getSingleton();

    render->projection.push();
    render->model.push();

    render->projection.top = mat4_IdentityMatrix;
    render->model.top = translate(-1, -1, 0) * scale(screenRes_inv.x, screenRes_inv.y, 1);

    //glPushAttrib(GL_ENABLE_BIT);
    GLboolean depthTest;
    glGetBooleanv (GL_DEPTH_TEST, &depthTest);

    if (depthTest)
        glDisable(GL_DEPTH_TEST);

    for (unsigned int i = 0; i < UIs.size(); i++)
        UIs[i]->render(i, time_ms);

    //glPopAttrib();
    if (depthTest)
        glEnable(GL_DEPTH_TEST);

    render->projection.pop();
    render->model.pop();


    /*
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-1, -1, 0);
    glScalef(screenRes_inv.x, screenRes_inv.y, 1);
    for (unsigned int i = 0; i < UIs.size(); i++)
        UIs[i]->render(i, time_ms);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    */
}
bool GUIGroup::keyDown(const int k) {
    for (int i = (int)UIs.size() - 1; i >= 0; i--) {
        if (UIs[i]->keyDown(i, k)) {
            return true;
        }
    }
    return false;
}
bool GUIGroup::keyUp(const int k) {
    for (int i = (int)UIs.size() - 1; i >= 0; i--) {
        if (UIs[i]->keyUp(i, k)) {
            return true;
        }
    }
    return false;
}
bool GUIGroup::mouseDown(const int btn, const vec2 &pos) {
    for (int i = (int)UIs.size() - 1; i >= 0; i--) {
        if (UIs[i]->mouseDown(i, btn, pos)) {
            return true;
        }
    }
    return false;
}
bool GUIGroup::mouseUp(const int btn, const vec2 &pos) {
    for (int i = (int)UIs.size() - 1; i >= 0; i--) {
        if (UIs[i]->mouseUp(i, btn, pos)) {
            return true;
        }
    }
    return false;
}

