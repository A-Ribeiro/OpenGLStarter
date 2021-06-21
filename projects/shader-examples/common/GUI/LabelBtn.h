#ifndef LabelBtn_h
#define LabelBtn_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "Label.h"

class LabelBtn : public Label {
    vec2 quadDim,
        originalquadDim,
        originalPosB,
        p;
    bool centx, centy, selected;
private:
    void ajustPosBtn();
public:
    LabelBtn(ActionProcessor *actionPerformer, GLFont *f,
        const char*text,
        const vec2 &pos, const vec2 &dim,
        const bool Vcentered, const bool Hcentered, float size);
    void render(const int ID, const unsigned int time_ms);
    void setText(const char*text);
    void setPosition(const vec2 &p);
    void setAlign(const bool Vcentered, const bool Hcentered);
    void setMousePos(const int ID, const vec2 &pos);
    bool mouseDown(const int ID, const int btn, const vec2 &pos);

    vec2 getQuadDimension();

    SSE2_CLASS_NEW_OPERATOR
};

#endif
