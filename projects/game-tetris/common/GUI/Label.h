#ifndef Label_h
#define Label_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "InterfaceGuiGroup.h"
//#include "../GLHelper/PixelFont.h"


class Label : public InterfaceGuiGroup{
private:
    GLFont *font;
    char* txt;
    AABB aabb;
    vec2 p, originalPos;
    bool centerx, centery;
    void configureLabel(const char*text);
    void ajustPosition();
protected:
    Label(ActionProcessor *actionPerformer, GLFont *f);
    AABB getBounds()const;
    float size;
public:
    Label(ActionProcessor *actionPerformer,
        GLFont *f,
        const char*text,
        const vec2 &pos,
        const bool Vcentered, const bool Hcentered, float size);
    virtual ~Label();
    virtual void render(const int ID, const unsigned int time_ms);
    virtual void setText(const char*text);
    virtual void setPosition(const vec2 &p);
    virtual void setAlign(const bool Vcentered, const bool Hcentered);

    SSE2_CLASS_NEW_OPERATOR
};


#endif
