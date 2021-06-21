#ifndef InterfaceGuiGroup_h
#define InterfaceGuiGroup_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "GUI.h"

enum EventTypes {
    ET_NON_SPECIFIED = 0,
    MOUSE_PRESS,
    MOUSE_RELEASE,
    LERP_CHANGED,
    KEY_PRESS,
    KEY_RELEASE,
    SET_FOCUS
};

struct Event {
    EventTypes type;
    union {
        struct { float x, y; int btn; } mousePress, mouseRelease;
        struct { float oldLerp, newLerp; } lerpChanged;
        struct { int key; float x, y; } keyPress, keyRelease;
    };
};

class ActionProcessor {
public:
    //retorna true se puder dar continuidade ao evento
    virtual bool performAction(const int ID, Event &e);
    virtual ~ActionProcessor();
};


class InterfaceGuiGroup {
private:
    ActionProcessor *actionPerformer;
protected:
    bool performAction(const int ID, Event &e)const;
public:
    InterfaceGuiGroup(ActionProcessor *actionPerformer);
    virtual void setMousePos(const int ID, const vec2 &pos);
    virtual void resizeWindow(const int ID, const int w, const int h);
    virtual void render(const int ID, const unsigned int time_ms);
    //bool-> pra fazer se o evento foi consumido
    virtual bool keyDown(const int ID, const int k);
    virtual bool keyUp(const int ID, const int k);
    virtual bool mouseDown(const int ID, const int btn, const vec2 &pos);
    virtual bool mouseUp(const int ID, const int btn, const vec2 &pos);
    virtual ~InterfaceGuiGroup();
    void setFocus(const int ID);

    SSE2_CLASS_NEW_OPERATOR
};


#endif
