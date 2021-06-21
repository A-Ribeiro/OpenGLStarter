
#ifndef playState_hpp


//#include "../../../common/common.h"

#include "./gameplay/objects/scene.h"

//#include "../Font3d.h"
//#include "NavePrincipal.hpp"
//#include <stdio.h>
#include "../StarsBackGroundDraw.h"
//#include <GL/gl.h>
//#include "../GLee/glee.h"
//#include "../math/all.h"

//#include "../TextureHelper.h"

//#include "../OglStateHelper.h"

#include "./gameplay/HUDControler.h"

//#include "../PostRenderEffect.hpp"

#include "../SpaceInvader.h"

/*
  input for playState

*/
enum playStateInputTokens {
    //for spaceship movement
   //  MP_RIGHT_PRESS
   //  MP_RIGHT_RELEASE
   //  MP_LEFT_PRESS
   //  MP_LEFT_RELEASE
    //for shooting from the spaceship
   //  SP_SHOOT_PRESS
   //  SP_SHOOT_RELEASE
    //for item in spaceship
   //  LP_RECEIVE_SHOOT
   //  LP_GET_ITEMINVENCIBLE
};

class PlayState {
private:

    //  PostEffectRender postEffectRender;


    HUDcontroler HUD;
    StarsBackGroundDraw starsDraw;
    vec4 BackGroundColor1, BackGroundColor2;

    SpaceInvader *spaceInvader;

    //PixelFont *font;
public:
    int level;
    Scene scene;

    PlayState(SpaceInvader *si);
    void resizeWindow(int w, int h);
    void simulationTick();
    void printState();
    void render(float deltatime, const vec2 &screensize);
    /*
      reinicia os componentes deste playstate atual
    */
    void resetPlayState();
    /*
      reinicia os componentes deste playstate atual
    */
    bool resetPlayStateToNextLevel();
    /*
      cria o playstate de acordo com um n�vel
         1 - espa�o
         2 - espa�o perto de algum planeta
         3 - ...
         4 - terra
         5 - morte de todos os terraquio
    */
    void createPlaystate();

    void shoot();
    void moveSpaceShip(float velocity);
};

#endif
