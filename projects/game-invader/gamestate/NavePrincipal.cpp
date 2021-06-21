
#include "NavePrincipal.h"
#include<stdio.h>

//#include<gl/glut.h>

const char* spaceShipToStr(unsigned char state) {
    switch (state) {
        //moving state
    case idle_SpaceShip: return "idle";
    case startingMovingToLeft_SpaceShip: return "startingMovingToLeft";
    case movingToLeft_SpaceShip: return "movingToLeft";
    case movingToRightWithLeftPressed_SpaceShip: return "movingToRightWithLeftPressed";
    case startingMovingToRight_SpaceShip: return "startingMovingToRight";
    case movingToRight_SpaceShip: return "movingToRight";
    case movingToLeftWithRightPressed_SpaceShip: return "movingToLeftWithRightPressed";
        //shooting state
    case noShooting_SpaceShip: return "noShooting";
    case chargingShoot_SpaceShip: return "chargingShoot";
    case shooting_SpaceShip: return "shooting";
        //life state
    case alive_SpaceShip: return "alive";
    case invencible_SpaceShip: return "invencible";
    case xplosion_SpaceShip: return "xplosion";
    case dead_SpaceShip: return "dead";
    }
    return "unknown state";
}


#define timeToStartMovingLeft 10
#define timeToStartMovingRight 10

#define timeToChargeShoot 10
#define shootingInterval 10

#define timeToStayInInvencibleMode 100
#define timeToXplodeSpaceShip 100
/*
 parser de movimento da nave principal
*/
void NavePrincipal::movingParser(MovingParserInput input) {
    switch (movingState) {
        //  IDLE
    case idle_SpaceShip:
        switch (input) {
        case MP_RIGHT_PRESS:
            timeInCurrentMovingState = 0;
            movingState = startingMovingToRight_SpaceShip;
            return;
        case MP_LEFT_PRESS:
            timeInCurrentMovingState = 0;
            movingState = startingMovingToLeft_SpaceShip;
            return;
        default: break;
        }
        return;
        //  STARTING MOVING TO LEFT
    case startingMovingToLeft_SpaceShip:
        switch (input) {
        case MP_RIGHT_PRESS:
            movingState = startingMovingToRight_SpaceShip;
            return;
        case MP_LEFT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = idle_SpaceShip;
            return;
        case MP_TICKTIME:
            if (timeInCurrentMovingState > timeToStartMovingLeft) {
                timeInCurrentMovingState = 0;
                movingState = movingToLeft_SpaceShip;
            }
            return;
        default: break;
        }
        return;
        // MOVING TO LEFT
    case movingToLeft_SpaceShip:
        switch (input) {
        case MP_RIGHT_PRESS:
            timeInCurrentMovingState = 0;
            movingState = movingToRightWithLeftPressed_SpaceShip;
            return;
        case MP_LEFT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = idle_SpaceShip;
            return;
        case MP_TICKTIME:
            posicao = posicao - EixoDeslocamento;
            return;
        default: break;
        }
        return;
        // MOVING TO RIGHT WITH LEFT PRESSED
    case movingToRightWithLeftPressed_SpaceShip:
        switch (input) {
        case MP_RIGHT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = movingToLeft_SpaceShip;
            return;
        case MP_LEFT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = movingToRight_SpaceShip;
            return;
        case MP_TICKTIME:
            posicao = posicao + EixoDeslocamento;
            return;
        default: break;
        }
        return;
        // STARTING MOVING TO RIGHT
    case startingMovingToRight_SpaceShip:
        switch (input) {
        case MP_LEFT_PRESS:
            movingState = startingMovingToLeft_SpaceShip;
            return;
        case MP_RIGHT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = idle_SpaceShip;
            return;
        case MP_TICKTIME:
            if (timeInCurrentMovingState > timeToStartMovingRight) {
                timeInCurrentMovingState = 0;
                movingState = movingToRight_SpaceShip;
            }
            return;
        default: break;
        }
        return;
        // MOVING TO RIGHT
    case movingToRight_SpaceShip:
        switch (input) {
        case MP_LEFT_PRESS:
            timeInCurrentMovingState = 0;
            movingState = movingToLeftWithRightPressed_SpaceShip;
            return;
        case MP_RIGHT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = idle_SpaceShip;
            return;
        case MP_TICKTIME:
            posicao = posicao + EixoDeslocamento;
            return;
        default: break;
        }
        return;
        // MOVING TO LEFT WITH RIGHT PRESSED
    case movingToLeftWithRightPressed_SpaceShip:
        switch (input) {
        case MP_RIGHT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = movingToLeft_SpaceShip;
            return;
        case MP_LEFT_RELEASE:
            timeInCurrentMovingState = 0;
            movingState = movingToRight_SpaceShip;
            return;
        case MP_TICKTIME:
            posicao = posicao - EixoDeslocamento;
            return;
        default: break;
        }
        return;
    }
}
/*
  shooting Parser da Nave Principal
*/
void NavePrincipal::shootingParser(ShootingParserInput input) {
    switch (shootingState) {
        //shooting state
    case noShooting_SpaceShip:
        switch (input) {
        case SP_SHOOT_PRESS:
            timeInCurrentShootingState = 0;
            shootingState = chargingShoot_SpaceShip;
            return;
        default: break;
        }
        return;
    case chargingShoot_SpaceShip:
        switch (input) {
        case SP_SHOOT_RELEASE:
            timeInCurrentShootingState = 0;
            shootingState = noShooting_SpaceShip;
            return;
        case SP_TICKTIME:
            if (timeInCurrentShootingState > timeToChargeShoot) {
                timeInCurrentShootingState = 0;
                shootingState = shooting_SpaceShip;
            }
            return;
        default: break;
        }
        return;
    case shooting_SpaceShip:
        switch (input) {
        case SP_SHOOT_RELEASE:
            timeInCurrentShootingState = 0;
            shootingState = noShooting_SpaceShip;
            return;
        case SP_TICKTIME:
            if (timeInCurrentShootingState > shootingInterval) {
                timeInCurrentShootingState = 0;
                //SHOOT
            }
            return;
        default: break;
        }
        return;
    }
}
/*
 life parser da Nave Principal
*/
void NavePrincipal::lifeParser(LifeParserInput input) {
    switch (lifeState) {
        //life state
    case alive_SpaceShip:
        switch (input) {
        case LP_GET_ITEMINVENCIBLE:
            timeInCurrentLifeState = 0;
            lifeState = invencible_SpaceShip;
            return;
        case LP_RECEIVE_SHOOT:
            timeInCurrentLifeState = 0;
            lifeState = xplosion_SpaceShip;
            /*
              alterando o estado das outras m�quinas de estado
              para fazer com que elas n�o fa�am + nada
            */
            timeInCurrentMovingState = 0;
            timeInCurrentShootingState = 0;
            shootingState = dead_SpaceShip;
            movingState = dead_SpaceShip;
            return;
        default: break;
        }
        return;
    case invencible_SpaceShip:
        switch (input) {
        case LP_GET_ITEMINVENCIBLE:
            timeInCurrentLifeState = 0;
            return;
        case LP_TICKTIME:
            if (timeInCurrentLifeState > timeToStayInInvencibleMode) {
                timeInCurrentLifeState = 0;
                lifeState = alive_SpaceShip;
            }
            return;
        default: break;
        }
        return;
    case xplosion_SpaceShip:
        switch (input) {
        case LP_TICKTIME:
            if (timeInCurrentLifeState > timeToXplodeSpaceShip) {
                timeInCurrentLifeState = 0;
                lifeState = dead_SpaceShip;
            }
            return;
        default: break;
        }
        return;
    case dead_SpaceShip:
        return;
    }
}
/*
  construtor Nave Principal
*/
NavePrincipal::NavePrincipal(SpaceInvader *si) {

    spaceInvader = si;

    movingState = idle_SpaceShip;
    lifeState = alive_SpaceShip;
    shootingState = noShooting_SpaceShip;
    timeInCurrentMovingState = 0;
    timeInCurrentShootingState = 0;
    timeInCurrentLifeState = 0;
    posicao = vec3(0);
    EixoDeslocamento = vec3(0.1, 0, 0);
}
/*
  setInput
*/
void NavePrincipal::setInput(MovingParserInput movingInput) {
    if (movingInput != MP_TICKTIME)
        movingParser(movingInput);
}
/*
  setShootInput
*/
void NavePrincipal::setShootInput(ShootingParserInput shootInput) {
    if (shootInput != SP_TICKTIME)
        shootingParser(shootInput);
}
/*
  lifeInput
*/
void NavePrincipal::setLifeInput(LifeParserInput lifeInput) {
    if (lifeInput != LP_TICKTIME)
        lifeParser(lifeInput);
}
/*
  simulation Tick
*/
void NavePrincipal::simulationTick() {
    movingParser(MP_TICKTIME);
    shootingParser(SP_TICKTIME);
    lifeParser(LP_TICKTIME);
    timeInCurrentMovingState++;
    timeInCurrentShootingState++;
    timeInCurrentLifeState++;
}
/*
  printState
*/
void NavePrincipal::printState() {
    printf("  SPACESHIP OBJECT\n");
    printf("              lifeState = %s\n", spaceShipToStr(lifeState));
    printf("           time in life = %u\n", timeInCurrentLifeState);
    printf("            movingState = %s\n", spaceShipToStr(movingState));
    printf("         time in moving = %u\n", timeInCurrentMovingState);
    printf("          shootingState = %s\n", spaceShipToStr(shootingState));
    printf("       time in shooting = %u\n", timeInCurrentShootingState);
    printf("                posicao = (%.2f, %.2f, %.2f)\n", posicao.x, posicao.y, posicao.z);
}
/*
 render
*/
void NavePrincipal::render() {

    spaceInvader->render->model.top *= translate(posicao.x, posicao.y, posicao.z);

    if (lifeState == dead_SpaceShip) {
        //    glutWireCube(1);
        return;
    }
    switch (movingState) {
    case idle_SpaceShip:

        spaceInvader->render->model.top *=
            rotate(timeInCurrentMovingState, 1, 0, 0) *
            rotate(timeInCurrentMovingState, 0, 1, 0);
        break;
    case startingMovingToLeft_SpaceShip:
        spaceInvader->render->model.top *=
            rotate((float)timeInCurrentMovingState*0.3, 0, 0, 1);
        break;
    case movingToLeft_SpaceShip:
    case movingToLeftWithRightPressed_SpaceShip:
        spaceInvader->render->model.top *=
            rotate((float)timeToStartMovingLeft*0.3, 0, 0, 1);
        break;
    case startingMovingToRight_SpaceShip:
        spaceInvader->render->model.top *=
            rotate(-(float)timeInCurrentMovingState*0.3, 0, 0, 1);
        break;
    case movingToRight_SpaceShip:
    case movingToRightWithLeftPressed_SpaceShip:
        spaceInvader->render->model.top *=
            rotate(-(float)timeToStartMovingRight*0.3, 0, 0, 1);
        break;
    }
    //  glutWireSphere(0.5, 10, 10);

}
