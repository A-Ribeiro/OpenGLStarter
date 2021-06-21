#include "LabelBtn.h"

void LabelBtn::ajustPosBtn() {

    AABB aabb = getBounds();

    vec2 bounds = (vec2(aabb.max_box.x, aabb.max_box.y) - vec2(aabb.min_box.x, aabb.min_box.y));

    quadDim = maximum(bounds + 5.0f, originalquadDim);
    p = originalPosB;

    //vec2 center = (vec2(aabb.max_box.x, aabb.max_box.y) + vec2(aabb.min_box.x, aabb.min_box.y)) * 0.5f;

    vec2 center = p;

    if (centx) {
        p = p - vec2(1, 0)*quadDim*0.5;
    }
    else
    {
        center.x += quadDim.x * 0.5;
    }

    if (centy) {
        p = p - vec2(0, 1)*quadDim*0.5;
    }
    else {
        center.y += quadDim.y * 0.5;
    }

    Label::setPosition(center);



}
LabelBtn::LabelBtn(ActionProcessor *actionPerformer, GLFont *f,
    const char*text,
    const vec2 &pos, const vec2 &dim,
    const bool Vcentered, const bool Hcentered, float size) : Label(actionPerformer, f) {
    this->size = size;
    originalPosB = pos;
    originalquadDim = dim;
    centx = Hcentered;
    centy = Vcentered;
    //label config
    Label::setAlign(true, true);
    Label::setText(text);
    ajustPosBtn();
}
void LabelBtn::render(const int ID, const unsigned int time_ms) {


    RenderSystem *render = RenderSystem::getSingleton();

    vec3 vpos[] = {
        vec3(p                         , 0.0f),
        vec3((p + quadDim * vec2(1, 0)), 0.0f),
        vec3((p + quadDim * vec2(1, 1)), 0.0f),

        vec3(p                         , 0.0f),
        vec3((p + quadDim * vec2(1, 1)), 0.0f),
        vec3((p + quadDim * vec2(0, 1)), 0.0f)
    };

    const vec4 vColor_selected[] = {
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f),
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f),
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f),
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f),
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f),
        vec4(vec3(0.3f, 0.35f, 0.35f),1.0f)
    };

    const vec4 vColor_not_selected[] = {
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f),
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f),
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f),
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f),
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f),
        vec4(vec3(0.2f, 0.25f, 0.25f),1.0f)
    };

    if (selected)
        render->drawColor(GL_TRIANGLES, vpos, vColor_selected, 6);
    else
        render->drawColor(GL_TRIANGLES, vpos, vColor_not_selected, 6);


    /*

    if (selected) {
        glColor3f(0.3f, 0.35f, 0.35f);
    }
    else {
        glColor3f(0.2f, 0.25f, 0.25f);
    }
    glBegin(GL_QUADS);
    glVertex2fv(p.array);                   glVertex2fv((p + quadDim * vec2(1, 0)).array);
    glVertex2fv((p + quadDim * vec2(1, 1)).array); glVertex2fv((p + quadDim * vec2(0, 1)).array);
    glEnd();
    glColor3f(1, 1, 1);

    */
    Label::render(ID, time_ms);
}
void LabelBtn::setText(const char*text) {
    Label::setText(text);
    ajustPosBtn();
}
void LabelBtn::setPosition(const vec2 &p) {
    originalPosB = p;
    ajustPosBtn();
}
void LabelBtn::setAlign(const bool Vcentered, const bool Hcentered) {
    centx = Hcentered;
    centy = Vcentered;
    ajustPosBtn();
}
void LabelBtn::setMousePos(const int ID, const vec2 &pos) {
    if (AABB::pointInsideAABB(pos, AABB(p, (p + quadDim)))) {
        if (!selected)
            selected = true;
    }
    else {
        if (selected)
            selected = false;
    }
}
bool LabelBtn::mouseDown(const int ID, const int btn, const vec2 &pos) {
    if (AABB::pointInsideAABB(pos, AABB(p, (p + quadDim)))) {
        Event e;
        e.type = MOUSE_PRESS;
        e.mousePress.x = pos.x;
        e.mousePress.y = pos.y;
        e.mousePress.btn = btn;
        performAction(ID, e);
        return true;
    }
    return false;
}

vec2 LabelBtn::getQuadDimension() {
    return quadDim;
}

//-----------------------------------------------------------------------------------------------
