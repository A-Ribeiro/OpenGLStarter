
#ifndef HUDCONTROLER_hpp
#define HUDCONTROLER_hpp


//#include "../../Font3d.h"
//#include "../../../../common/common.h"

#include "../../SpaceInvader.h"


class HUDcontroler {
private:
    int numberOfLifes;
    int score;
    int level;
    //PixelFont *font;
    //int w, h;

    SpaceInvader * spaceInvader;
public:
    HUDcontroler(SpaceInvader * si);
    void setNumberOfLifes(int value);
    void setScore(int value);
    void setLevel(int value);
    void innerDraw(const vec2 &screensize);
    void render(float deltaTime, const vec2 &screensize);
};


#endif
