
#ifndef pauseState_hpp

#include "../StarsBackGroundDraw.h"
#include "../SpaceInvader.h"

//#include "../Font3d.h"


class PauseState{
  private:
    StarsBackGroundDraw starsDraw;
    //PixelFont *font;
    //float fontWidth;

    SpaceInvader *spaceInvader;

  public:
    PauseState(SpaceInvader *si);
    void resizeWindow(int w,int h);
    void printState();
    void render(float deltaTime, const vec2 &screencenter);
};

#endif
