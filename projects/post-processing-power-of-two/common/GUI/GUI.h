#ifndef GUI_h
#define GUI_h

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "../../RenderSystem.h"

//#include "../GLHelper/ListHelper.h"
//#include "../GLHelper/DrawableImage.h"
//#include "../Simulation/AABB.h"


class Interface {
private:
    int actRet;
public:
    Interface();
    virtual void setMousePos(const vec2 &pos);
    virtual void resizeWindow(const int w, const int h);
    virtual void render(const unsigned int time_ms, const bool showCursor = true);
    virtual bool keyDown(const int k);
    virtual bool keyUp(const int k);
    virtual bool mouseDown(const int btn, const vec2 &pos);
    virtual bool mouseUp(const int btn, const vec2 &pos);

    //retorna uma ação com um número maior ou igual a 0
    int actionReturn()const;
    void setActionReturn(const int num);
    virtual ~Interface();

    SSE2_CLASS_NEW_OPERATOR
};

class AABBitem {
    GLTexture *img;
    vec2 position;
    int ID;
    vec2 img_center;
    vec2 img_dim;
public:
    AABBitem(const char*file, const int ID);
    virtual ~AABBitem();
    void setPosition(const vec2 &p, const bool centerBased = false);
    float getWidth()const;
    float getHeight()const;
    int getID()const;
    bool colide(const vec2 &point)const;
    void render()const;

    SSE2_CLASS_NEW_OPERATOR
};
#include <vector>
class AABBMenuList {
    std::vector<AABBitem*> items, itemsSel;
    float space;
    int selected;
public:
    AABBMenuList(const char*files[], const char*filesSel[], const int count, const float space);
    ~AABBMenuList();
    void render()const;
    void setPosition(const vec2 &pos, const bool centerBased);
    int colision(const vec2 &p)const;
    void setSelected(const int s);
    int getSelected()const;
    int count()const;

    SSE2_CLASS_NEW_OPERATOR
};


#endif
