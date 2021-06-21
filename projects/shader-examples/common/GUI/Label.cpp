#include "Label.h"

#include "InterfaceGuiGroup.h"

#include <string.h>

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

//-----------------------------------------------------------------------------------------------
void Label::configureLabel(const char*text) {
    if (txt != NULL) {
        delete[] txt;
        txt = NULL;
    }
    txt = new char[strlen(text) + 1];
    strcpy(txt, text);

    aabb = font->computeBoundsJustBox((const char*)txt);

    aabb.min_box *= size;
    aabb.max_box *= size;

}
void Label::ajustPosition() {
    p = originalPos;

    /*
    if (centerx) {
        p = p - vec2(aabb.max_box.x*0.5, 0);
    }

    if (centery) {
        float delta = aabb.min_box.y - aabb.max_box.y;
        p = p + vec2(0, -font->getLineHeight() + delta * 0.5f);
    }
    */

    //AABB aabb = font->computeBoundsJustBox(txt);

    /*
    vec2 bounds = getBounds();

    if (centerx) {
        p = p - vec2(1, 0)*bounds*0.5;
    }
    if (centery) {
        p = p - vec2(0, 1)*bounds*0.5;
    }
    */
}

Label::Label(ActionProcessor *actionPerformer, GLFont *f) :InterfaceGuiGroup(actionPerformer) {
    DEBUG_PRINT("[LABEL] Calling hidden constructor");
    font = f;
    txt = NULL;
    txt = new char[1];
    txt[0] = '\0';
    originalPos = vec2(0);
    centery = false;
    centerx = false;
}
AABB Label::getBounds()const {
    return aabb;
    /*
    vec3 bounds = aabb.max_box - aabb.min_box;
    return vec2(bounds.x, bounds.y);
    */

    //return font->getBounds(txt);
}
Label::Label(ActionProcessor *actionPerformer, GLFont *f,
    const char*text,
    const vec2 &pos,
    const bool Vcentered, const bool Hcentered, float size) :InterfaceGuiGroup(actionPerformer) {
    this->size = size;
    font = f;
    txt = NULL;
    originalPos = pos;
    centery = Vcentered;
    centerx = Hcentered;
    configureLabel(text);
    ajustPosition();
}
Label::~Label() {
    DEBUG_PRINT("[LABEL] destroying");
    if (txt != NULL)
        delete[] txt;
    txt = NULL;
    font = NULL;
}
void Label::render(const int ID, const unsigned int time_ms) {

    RenderSystem *render = RenderSystem::getSingleton();

    float xAlign = 0.0f;
    if (centerx)
        xAlign = 0.5f;

    float yAlign = 0.0f;
    if (centery)
        yAlign = 0.5f;

    /*
    vec2 min = vec2(aabb.min_box.x, aabb.min_box.y);
    vec2 max = vec2(aabb.max_box.x, aabb.max_box.y);

    vec2 center = (min + max) * 0.5f;

    AABB aux = AABB(min + p - center, max + p - center);

    render->drawAABB_2D(aux, vec4(0.5f));

    aux = AABB(vec2(-10,-10) + p, vec2(10,10) + p);
    render->drawAABB_2D(aux, vec4(0.9f));
    */
    

    render->print(font, txt, p, vec4(1), xAlign, yAlign, size);

    //font->staticPrint(p, txt);
}
void Label::setText(const char*text) {
    configureLabel(text);
    ajustPosition();
}
void Label::setPosition(const vec2 &p) {
    originalPos = p;
    ajustPosition();
}
void Label::setAlign(const bool Vcentered, const bool Hcentered) {
    centery = Vcentered;
    centerx = Hcentered;
    ajustPosition();
}
//-----------------------------------------------------------------------------------------------
