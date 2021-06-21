#include "GUI.h"

/*
   interface main definition
*/
Interface::Interface() { actRet = -1; }
void Interface::setMousePos(const vec2 &pos) {}
void Interface::resizeWindow(const int w, const int h) {}
void Interface::render(const unsigned int time_ms, const bool showCursor) {}
bool Interface::keyDown(const int k) { return true; }
bool Interface::keyUp(const int k) { return true; }
bool Interface::mouseDown(const int btn, const vec2 &pos) { return true; }
bool Interface::mouseUp(const int btn, const vec2 &pos) { return true; }

//retorna uma ação com um número maior ou igual a 0
int Interface::actionReturn()const { return actRet; }
void Interface::setActionReturn(const int num) { actRet = num; }
Interface::~Interface() {}

/*
   AABBItem main definition
*/
AABBitem::AABBitem(const char*file, const int ID) {
    this->ID = ID;
    position = vec2(0);

    img = GLTexture::loadFromFile(file);
    img_dim = vec2(img->width, img->height);
    img_center = img_dim * 0.5f;
}
AABBitem::~AABBitem() {
    setNullAndDelete(img);
}

void AABBitem::setPosition(const vec2 &p, const bool centerBased) {
    position = p;
    if (centerBased) {
        position = position - img_center;
    }
}
float AABBitem::getWidth()const {
    return (float)img_dim.x;
}
float AABBitem::getHeight()const {
    return (float)img_dim.y;
}
int AABBitem::getID()const {
    return ID;
}
bool AABBitem::colide(const vec2 &point)const {
    AABB aabb = AABB(position, position + img_dim);
    return AABB::pointInsideAABB(point, aabb);
}
void AABBitem::render()const {
    RenderSystem *render = RenderSystem::getSingleton();

    vec3 vpos[] = {
        vec3(position.x             , position.y            , 0.0f),
        vec3(position.x + img_dim.x , position.y            , 0.0f),
        vec3(position.x + img_dim.x , position.y + img_dim.y, 0.0f),

        vec3(position.x             , position.y            , 0.0f),
        vec3(position.x + img_dim.x , position.y + img_dim.y, 0.0f),
        vec3(position.x             , position.y + img_dim.y, 0.0f)
    };

    const vec2 vuv[] = {
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 1),
        vec2(1, 0),
        vec2(0, 0)
    };

    render->drawTexture(img, vec4(1.0f), GL_TRIANGLES, vpos, vuv, 6);

    //img.draw(position);
}

/*
   AABBItemList main definition
*/
AABBMenuList::AABBMenuList(const char*files[], const char*filesSel[], const int count, const float space) {
    this->space = space;
    items.resize(count);
    itemsSel.resize(count);
    for (int i = 0; i < count; i++) {
        items[i] = new AABBitem(files[i], i);
        itemsSel[i] = new AABBitem(filesSel[i], i);
    }
    selected = -1;
}
AABBMenuList::~AABBMenuList() {
    for (unsigned int i = 0; i < items.size(); i++) {
        delete items[i];
        delete itemsSel[i];
    }
    items.resize(0);
    itemsSel.resize(0);
}
void AABBMenuList::render()const {
    for (unsigned int i = 0; i < items.size(); i++) {
        if ((int)i == selected)
            itemsSel[i]->render();
        else
            items[i]->render();
    }
}
void AABBMenuList::setPosition(const vec2 &posp, const bool centerBased) {
    vec2 pos = posp;
    for (unsigned int i = 0; i < items.size(); i++) {
        items[i]->setPosition(pos, centerBased);
        itemsSel[i]->setPosition(pos, centerBased);
        pos = pos - vec2(0, items[i]->getHeight() + space);
    }
}
int AABBMenuList::colision(const vec2 &p)const {
    for (unsigned int i = 0; i < items.size(); i++) {
        if (items[i]->colide(p))
            return (int)i;
    }
    return -1;
}
void AABBMenuList::setSelected(const int s) {
    if (s != selected)
        selected = s;
}
int AABBMenuList::getSelected()const {
    return selected;
}
int AABBMenuList::count()const {
    return (int)items.size();
}


