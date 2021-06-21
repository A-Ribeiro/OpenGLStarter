
#ifndef gamePlay_hpp
#define gamePlay_hpp

//#include "../../math/all.h"
//#include "../../GLee/GLee.h"
//#include "../../common/common.h"

#include <stdio.h>

#include "./objects/scene.h"

class GamePlay {
private:
    //--------------------------------------------------------------------------
    Scene scene;
public:
    //--------------------------------------------------------------------------
    /*
      processamento da l�gica do jogo
    */
    void simulationTick() {
        scene.simulationTick();
    }//*/
    //--------------------------------------------------------------------------
    GamePlay() {

    }
    //--------------------------------------------------------------------------
    void render() {
        scene.render();
        /*
         desenho dos limites do cen�rio
        */
        //      glColor3ub(0xff,0xff,0xff);

        //      glColor3ub(0,0xff,0);

    }
    //--------------------------------------------------------------------------
    void printState() {
        printf("    GAMEPLAY STATE\n");

    }
    //--------------------------------------------------------------------------
};

#endif
