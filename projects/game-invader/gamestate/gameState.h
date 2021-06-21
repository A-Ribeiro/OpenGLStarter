
#ifndef gameState_hpp
#define gameState_hpp

#include "../SpaceInvader.h"

//#include "./gamePlay/NavePrincipal.hpp"

#include "playState.h"
#include "pauseState.h"

#include "Menu.h"
#include "cgReproducer.h"



/*
  o gameState de cada fase possui
       um cen�rio,
       um conjunto de alienigenas(inimigos),
       um conjunto de objetos (stuff) que podem ser lan�ados pelos inimigos ou pela nave principal
       a nave principal

                  ** ao passar a token init_cg
                     pode ser pelo loadGame ou NewGame
       ------  <init_cg>         -------------------
       |Menu| *----------------> |  CG Antes Jogo  |
       ------                    -------------------
         ^                             *   ^
         |                  <init_play>|   |<init_cg>
         |                             v   |
         |   -------     <pause>   ------  |
         |-* |Pause| <-----------> |Jogo|  |
         |   -------               ------  |
         |    *    <stage_clear>    * *    |
         |    |                     | |    |
         |    -----------------------------|
         |                          | |    |
<to_menu>|                |---------- |    |
         |                v           |    |
         |       -----------------    |    |
         |-----* | fase completa | *-------|
         |       -----------------    |    |
         |                            |    |
         |                 <bad_stage>|    |
         |                            v    |
         |           -------------------   |
         |---------* | fase incompleta | *--
                     -------------------
*/


enum GAMESTATE_TOKENS_ENUM{
  GS_init_cg=0,
  GS_init_play,
  GS_pause,
  GS_stage_clear,
  GS_bad_stage,
  GS_to_menu
};
enum GAMESTATE_STATES_ENUM{
  GS_MENU=0,
  GS_CG_BEFORE_PLAY,
  GS_PLAY,
  GS_PAUSE,
  GS_STAGE_COMPLETE,
  GS_STAGE_INCOMPLETE,
  GS_CONGRATULATIONS
};

const char* gamestateToStr(GAMESTATE_STATES_ENUM state);

class GameState {

    SpaceInvader *spaceInvader;

    //PixelFont *font;

    Menu menu;
    CGReproducer cgReproducer;

    PlayState playState;
    PauseState pauseState;
//    NavePrincipal navePrincipal;

    GAMESTATE_STATES_ENUM globalGamestate;

    void parseGS(GAMESTATE_TOKENS_ENUM token);
  public:
    GameState(SpaceInvader *si);

    void resizeWindow(int w,int h);
    /*
      os comandos abaixo somente tem efeito no estado:

          PLAY

    */
//    void setNaveMovingInput(MovingParserInput movingInput);
//    void setNaveShootingInput(ShootingParserInput shootInput);
//    void setNaveLifeInput(LifeParserInput lifeInput);

    /*
       retorna true se for para sair do jogo
      tem efeito somente no estado:
        MENU
      */
    bool setMenuInput(MENU_TOKENS menuInput);

    /*
    faz a verifica��o se o cg j� acabou... e se sim, ent�o
       passa para o pr�ximo estado
      tem efeito nos estados:
          CG_BEFORE_PLAY
      */
    bool cgReproducerIsTerminatedCheck();

    /*
      comando que somente tem efeito dentro dos estados:
        PLAY
        PAUSE
    */
    void postPauseToken();
    /*
      comando que somente tem efeito dentro dos estados:
        PAUSE
        STAGE_COMPLETE
        STAGE_INCOMPLETE
    */
    void postToMenuToken();

    /*
      m�todo que � chamado sempre que � para se fazer
      o calculo de um movimento no jogo
    */
    void simulationTick();
    void printState();
    void render(float deltaTime, const vec2& screenCenter);
    /*
      tem efeito somente em:
        PLAY
    */
    void shoot();
    void moveSpaceShip(float velocity);
    void enterPress();

    SSE2_CLASS_NEW_OPERATOR
};

#endif
