#ifndef GUIGroup_h
#define GUIGroup_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "InterfaceGuiGroup.h"
//#include "../GLHelper/PixelFont.h"
#include "GUI_ext.h"


#include <vector>
class GUIGroup : public Interface, public ActionProcessor {
    std::vector<InterfaceGuiGroup*> UIs;
protected:
    vec2 centerScreen, screenRes_inv, mousepos;
    int w, h;
    void clearAll();
    GUIGroup();
    virtual ~GUIGroup();
    int addSlider(const vec2 &min,
        const vec2 &max,
        const float distanceToConsider,
        GLTexture *bolinha);
    int addLabel(GLFont *f,
        const char*text,
        const vec2 &pos,
        const bool centerV, const bool centerH,
        float size);
    int addLabelBtn(GLFont *f,
        const char*text,
        const vec2 &pos, const vec2 &dim,
        const bool centerV, const bool centerH,
        float size);
    InterfaceGuiGroup* getInterface(const int i)const;
public:
    virtual void setMousePos(const vec2 &pos);
    virtual void resizeWindow(const int w, const int h);
    virtual void render(const unsigned int time_ms, const bool showCursor = true);
    virtual bool keyDown(const int k);
    virtual bool keyUp(const int k);
    virtual bool mouseDown(const int btn, const vec2 &pos);
    virtual bool mouseUp(const int btn, const vec2 &pos);

    SSE2_CLASS_NEW_OPERATOR
};


#endif
