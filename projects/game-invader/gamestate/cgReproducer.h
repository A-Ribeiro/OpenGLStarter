
#ifndef cgReproducer_hpp
#define cgReproducer_hpp

#include <aribeiro/aribeiro.h>

//#include "../Font3d.h"

//#include "../../../common/common.h"

//#include "../StarsBackGroundDraw.h"

//#include "../textureHelper.h"
#include <math.h>
//#include "../math/all.h"

#include "../SpaceInvader.h"
#include "../StarsBackGroundDraw.h"


class CGReproducer{
  private:
    StarsBackGroundDraw starsDraw;
    //PixelFont *font;

      SpaceInvader *spaceInvader;

    int score, nextLevel;
  public:
    CGReproducer(SpaceInvader *si);
    ~CGReproducer();
    void resizeWindow(int w,int h);
    void loadAnim();
    /*

      se retornar true ent�o
      j� pode come�ar o jogo

    */
    void render(float elapsedTime, const vec2 &screenCenter);

    bool cgReproducerIsTerminated();
    void printState();
    void setScoreLeve(int score, int level);
};

#endif
