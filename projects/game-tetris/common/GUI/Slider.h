#ifndef Slider_h
#define Slider_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "InterfaceGuiGroup.h"

class Slider : public InterfaceGuiGroup{
    vec2 min, max,
        min2max,
        unitVecMin2Max,
        perpVecMin2Max,
        distance2ConsidVec;
    float length_min2max, length_inv, length_sqr;

    float distanceToConsider,
        sqrDistanceToConsider,
        sqrLengthToTest;

    float item_lerp;

    float lenghtSelected;

    GLTexture *bolinha;//,*cursor;

    bool mouseDown_bool;

    //ActionProcessor *actionProcessor;

    bool colide(const vec2 &ptn);
public:
    Slider(
        ActionProcessor *actionProc,
        const vec2 &min,
        const vec2 &max,
        const float distanceToConsider,
        GLTexture *b);
    bool mouseDown(const int ID, const int btn, const vec2 &pos);
    bool mouseUp(const int ID, const int btn, const vec2 &pos);
    void render(const int ID, const unsigned int time_ms);
    void setMousePos(const int ID, const vec2 &pos);
    void setLerp(const float l);
    float getLerp()const;

    SSE2_CLASS_NEW_OPERATOR
};


#endif
