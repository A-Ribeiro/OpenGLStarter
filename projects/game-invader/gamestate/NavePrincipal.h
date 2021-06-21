
#ifndef NavePrincipal_hpp
#define NavePrincipal_hpp

#include "../SpaceInvader.h"

//#include "../../../common/common.h"
//#include "../math/all.h"


/*
 State da nave principal


 Esta entidade no jogo possui tr�s aut�matos,
      * um para realizar o controle de movimento
      * um para realizar o controle de tiros
      * um para realizar o controle de vida


--------------------------------------------------------------------------------
   Estados da Nave
--------------------------------------------------------------------------------

Representa��o em ASCII do diagrama de estados:


  <noem> - denota uma token
  ------
  |nome| - denota um estado
  ------

Representa��o da Seta de direcionamento:
  ( '*' - inicio; '>' - fim )
            *-------->


--------------------------------------------------------------------------------



  *************************
  * Controle de movimento *
  *************************
    Estado inicial = IDLE

                                                  <LEFT_RELEASE>
                       ---------------------------------------------------------
                       |                                    |                  |
                       v                                    *                  *
<RIGHT_RELEASE>     -------- <LEFT_PRESS>    ----------------------      --------------
        |-------->  | IDLE | *------------>  | StartingMovingLeft | *--> | MovingLeft |
        |           --------                 ----------------------      --------------
        |              *                            *   ^                     ^    *
        |              | <RIGHT_PRESS>              |   |      <RIGHT_RELEASE>|    |
        |              |                            |   |                     |    |
        |              v              <RIGHT_PRESS> |   |                     |    |
        |   ----------------------- <----------------   |                     |    |
        |-* | StartingMovingRight |                     |                     |    |
        |   ----------------------- *--------------------                     |    |
        |             |               <LEFT_PRESS>                            |    |
        |             v                                                       |    |
        |     ---------------                                                 |    |
        |---* | MovingRight | *-|                                             |    |
              ---------------   | <LEFT_PRESS>   --------------------------------  |
                  ^             |--------------> | MovingToLeftWithRightPressed |  |
                  |                              --------------------------------  |
    <LEFT_RELEASE>|                                                                |
                  |   --------------------------------   <RIGHT_PRESS>             |
                  --* | MovingToRightWithLeftPressed | <----------------------------
                      --------------------------------



  *********************
  * Controle de tiros *
  *********************
    Estado inicial = noShooting

    --------------   <SHOOT_PRESS>     -----------------
    | noShooting | *-----------------> | chargingShoot |
    --------------                     -----------------
         ^                                   *
         |                                   |
         |                                   v
         | <SHOOT_RELEASE>              ------------
         -----------------------------* | shooting |
                                        ------------


  ********************
  * Controle de Vida *
  ********************
    Estado inicial = alive

     ---------   <RECEIVE_SHOOT>   ------------       --------
     | alive | *-----------------> | xplosion | *---> | dead |
     ---------                     ------------       --------
       ^  *
       |  | <GET_ITEMINVENCIBLE>   --------------
       |  -----------------------> | invencible |
       |                           --------------
       |                                 *
       -----------------------------------

*/
enum SpaceShipState{
  //moving state
    idle_SpaceShip = 0,
    startingMovingToLeft_SpaceShip,
    movingToLeft_SpaceShip,
    movingToRightWithLeftPressed_SpaceShip,
    startingMovingToRight_SpaceShip,
    movingToRight_SpaceShip,
    movingToLeftWithRightPressed_SpaceShip,
  //shooting state
    noShooting_SpaceShip,
    chargingShoot_SpaceShip,
    shooting_SpaceShip,
  //life state
    alive_SpaceShip,
    invencible_SpaceShip,
    xplosion_SpaceShip,
    dead_SpaceShip
};
const char* spaceShipToStr(unsigned char state);

enum MovingParserInput{
  MP_RIGHT_PRESS=0,
  MP_RIGHT_RELEASE,
  MP_LEFT_PRESS,
  MP_LEFT_RELEASE,
  MP_TICKTIME
};
enum ShootingParserInput{
  SP_SHOOT_PRESS=0,
  SP_SHOOT_RELEASE,
  SP_TICKTIME
};
enum LifeParserInput{
  LP_RECEIVE_SHOOT=0,
  LP_GET_ITEMINVENCIBLE,
  LP_TICKTIME
};

class NavePrincipal{
  private:
    //contador de ticks em que o estado atual est�
    unsigned int timeInCurrentMovingState;
    unsigned int timeInCurrentShootingState;
    unsigned int timeInCurrentLifeState;
    //estados na nave
    unsigned char movingState;
    unsigned char lifeState;
    unsigned char shootingState;
    vec3 posicao;
    vec3 EixoDeslocamento;

    SpaceInvader *spaceInvader;
    //--------------------------------------------------------------------------
    /*
      parser desta entidade no jogo
    */
    void movingParser(MovingParserInput input);
    void shootingParser(ShootingParserInput input);
    void lifeParser(LifeParserInput input);
  public:
    NavePrincipal(SpaceInvader *si);
    void setInput(MovingParserInput movingInput);
    void setShootInput(ShootingParserInput shootInput);
    void setLifeInput(LifeParserInput lifeInput);
    void simulationTick();
    void printState();
    void render();
};

#endif
