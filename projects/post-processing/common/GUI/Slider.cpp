#include "Slider.h"


Slider::Slider(
    ActionProcessor *actionProc,
    const vec2 &min,
    const vec2 &max,
    const float distanceToConsider,
    GLTexture *b) : InterfaceGuiGroup(actionProc) {
    this->bolinha = b;
    this->distanceToConsider = distanceToConsider;
    sqrDistanceToConsider = distanceToConsider * distanceToConsider;
    this->min = min;
    this->max = max;
    min2max = max - min;
    length_min2max = length(min2max);
    length_inv = 1.0f / length_min2max;
    length_sqr = length_min2max * length_min2max;

    sqrLengthToTest = length_min2max + distanceToConsider;
    sqrLengthToTest = sqrLengthToTest * sqrLengthToTest;

    unitVecMin2Max = min2max * length_inv;
    item_lerp = 0;
    perpVecMin2Max = vec2(unitVecMin2Max.y, -unitVecMin2Max.x)*distanceToConsider;
    distance2ConsidVec = unitVecMin2Max * distanceToConsider;
    mouseDown_bool = false;
}

bool Slider::colide(const vec2 &ptn) {
    vec2 perpendicular, paralelo;
    vecDecomp(ptn - min, unitVecMin2Max,
        &perpendicular, &paralelo);
    if (sqrLength(perpendicular) <= sqrDistanceToConsider) {
        lenghtSelected = sqrLength(paralelo);
        if (dot(paralelo, unitVecMin2Max) < 0)
            lenghtSelected = -lenghtSelected;
        if (lenghtSelected >= (-sqrDistanceToConsider) &&
            lenghtSelected <= sqrLengthToTest) {//(length_sqr + sqrDistanceToConsider)){
            lenghtSelected = clamp(lenghtSelected, 0, length_sqr);
            if (lenghtSelected != 0.0f)
                lenghtSelected = sqrt(lenghtSelected);
            return true;
        }
    }
    return false;
}
bool Slider::mouseDown(const int ID, const int btn, const vec2 &pos) {
    if (colide(pos)) {
        float newLerp = clamp(lenghtSelected / length_min2max, 0, 1);
        mouseDown_bool = true;
        if (newLerp != item_lerp) {
            Event e;
            e.type = LERP_CHANGED;
            e.lerpChanged.oldLerp = item_lerp;
            e.lerpChanged.newLerp = newLerp;
            if (performAction(ID, e)) {
                item_lerp = e.lerpChanged.newLerp;
            }
        }
        return true;//consumption
    }
    return false;
}
bool Slider::mouseUp(const int ID, const int btn, const vec2 &pos) {
    if (mouseDown_bool)
        mouseDown_bool = false;
    return false;
}
void Slider::render(const int ID, const unsigned int time_ms) {
    /*
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(-1,-1,0);
        glScalef(screenRes_inv.x,screenRes_inv.y,1);//*/
        /*
            glColor3f(0,0,0.5);
            glBegin(GL_QUADS);
              glVertex2fv((min+perpVecMin2Max-distance2ConsidVec).array); glVertex2fv((max+perpVecMin2Max+distance2ConsidVec).array);
              glVertex2fv((max-perpVecMin2Max+distance2ConsidVec).array); glVertex2fv((min-perpVecMin2Max-distance2ConsidVec).array);
            glEnd();
            glColor3f(1,1,1);
        //*/

    RenderSystem *render = RenderSystem::getSingleton();

    //
    // draw line
    //

    vec3 vpos[] = {
        vec3(min, 0.0f),
        vec3(max, 0.0f)
    };

    const vec4 vcolor[] = {
        vec4(1),
        vec4(1)
    };

    render->drawColor(GL_LINES, vpos, vcolor, 2);

    //
    // draw bolinha
    //

    if (bolinha != NULL) {

        vec2 pos = min + min2max * item_lerp - vec2(bolinha->width, bolinha->height) * 0.5f;

        vec3 vpos2[] = {
        vec3(pos.x                         , pos.y                         , 0.0f),
        vec3(pos.x + (float)bolinha->width , pos.y                         , 0.0f),
        vec3(pos.x + (float)bolinha->width , pos.y + (float)bolinha->height, 0.0f),

        vec3(pos.x                         , pos.y                         , 0.0f),
        vec3(pos.x + (float)bolinha->width , pos.y + (float)bolinha->height, 0.0f),
        vec3(pos.x                         , pos.y + (float)bolinha->height, 0.0f)
        };

        const vec2 vuv[] = {
            vec2(0, 1),
            vec2(1, 1),
            vec2(1, 0),

            vec2(0, 1),
            vec2(1, 0),
            vec2(0, 0)
        };

        render->drawTexture(bolinha, vec4(1.0f), GL_TRIANGLES, vpos2, vuv, 6);

    }


    /*
    glBegin(GL_LINES);
    glVertex2fv(min.array);
    glVertex2fv(max.array);
    glEnd();
    if (bolinha != NULL)
        bolinha->centerDraw(min + min2max * item_lerp);
        */


    //    cursor->centerDraw(mousePos);
        //glPopMatrix();
}
void Slider::setMousePos(const int ID, const vec2 &pos) {
    if (mouseDown_bool) {
        vec2 perpendicular, paralelo;
        vecDecomp(pos - min, unitVecMin2Max,
            &perpendicular, &paralelo);
        lenghtSelected = length(paralelo);
        if (dot(paralelo, unitVecMin2Max) < 0)
            lenghtSelected = -lenghtSelected;
        float newLerp = clamp(lenghtSelected / length_min2max, 0, 1);
        if (newLerp != item_lerp) {
            Event e;
            e.type = LERP_CHANGED;
            e.lerpChanged.oldLerp = item_lerp;
            e.lerpChanged.newLerp = newLerp;
            if (performAction(ID, e)) {
                item_lerp = e.lerpChanged.newLerp;
            }
        }

    }
}
void Slider::setLerp(const float l) {
    item_lerp = clamp(l, 0, 1);
}
float Slider::getLerp()const {
    return item_lerp;
}


