
#include "gameState.h"
#include <stdio.h>

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
//------------------------------------------------------------------------------
const char* gamestateToStr(GAMESTATE_STATES_ENUM state) {
    switch (state) {
    case GS_MENU:return "menu";
    case GS_CG_BEFORE_PLAY:return "cg_before_play";
    case GS_PLAY:return "play";
    case GS_PAUSE:return "pause";
    case GS_STAGE_COMPLETE:return "stage complete";
    case GS_STAGE_INCOMPLETE:return "stage incomplete";
    case GS_CONGRATULATIONS: return "congratulations";
    }
    return "unknown state";
}
//------------------------------------------------------------------------------
GameState::GameState(SpaceInvader *si) :
    menu(si),
    cgReproducer(si),
    playState(si),
    pauseState(si)
{
    globalGamestate = GS_MENU;
    this->spaceInvader = si;
}
//------------------------------------------------------------------------------
void GameState::resizeWindow(int w, int h) {
    /*
      deve atualizar todos os componentes internos
    */
    playState.resizeWindow(w, h);
    pauseState.resizeWindow(w, h);
    cgReproducer.resizeWindow(w, h);
}
//------------------------------------------------------------------------------
  /*
  tem efeito no estado:
        PLAY
  */
  //void GameState::setNaveMovingInput(MovingParserInput movingInput){
  //  if (globalGamestate != GS_PLAY) return;
  //  playState.gamePlay.getNavePrincipal()->setInput(movingInput);
  //  navePrincipal.setInput(movingInput);
  //}
  //------------------------------------------------------------------------------
    /*
    tem efeito no estado:
          PLAY
    */
    //void GameState::setNaveShootingInput(ShootingParserInput shootInput){
    //  if (globalGamestate != GS_PLAY) return;
    //  playState.gamePlay.getNavePrincipal()->setShootInput(shootInput);
    //  navePrincipal.setShootInput(shootInput);
    //}
    //------------------------------------------------------------------------------
      /*
      tem efeito no estado:
            PLAY
      */
      //void GameState::setNaveLifeInput(LifeParserInput lifeInput){
      //  if (globalGamestate != GS_PLAY) return;
      //  navePrincipal.setLifeInput(lifeInput);
      //}
      //------------------------------------------------------------------------------
        /*
           retorna true se for para sair do jogo
          tem efeito somente no estado:
            MENU
          */
bool GameState::setMenuInput(MENU_TOKENS menuInput) {
    if (globalGamestate != GS_MENU) return false;
    if (menu.inputMenu(menuInput)) {
        //OK
        switch (menu.getMenuState()) {
        case MS_NEW_GAME:
            //reset playState code
            spaceInvader->resources->setMusic(1);
            globalGamestate = GS_CG_BEFORE_PLAY;
            cgReproducer.setScoreLeve(0, 1);
            playState.resetPlayState();
            return false;
        case MS_LOAD_GAME:
            //load playState code
    //        globalGamestate = GS_CG_BEFORE_PLAY;
            return false;
        case MS_EXIT_GAME: return true;
        default: break;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
  /*
  tem efeito no estado:
        CG_BEFORE_PLAY
  */
bool GameState::cgReproducerIsTerminatedCheck() {
    if (globalGamestate != GS_CG_BEFORE_PLAY) return false;
    if (cgReproducer.cgReproducerIsTerminated()) {
        spaceInvader->resources->setMusic(2);
        globalGamestate = GS_PLAY;
        playState.createPlaystate();
        return true;
    }
    return false;
}
//------------------------------------------------------------------------------
  /*
  tem efeito no estado:
        PLAY
        PAUSE
  */
void GameState::postPauseToken() {
    //  if (globalGamestate != GS_PLAY && globalGamestate != GS_PAUSE) return;
    parseGS(GS_pause);
}
//------------------------------------------------------------------------------
  /*
  tem efeito no estado:
        PAUSE
        STAGE_COMPLETE
        STAGE_INCOMPLETE
  */
void GameState::postToMenuToken() {
    //  if (globalGamestate != GS_PAUSE &&
    //      globalGamestate != GS_STAGE_COMPLETE &&
    //      globalGamestate != GS_STAGE_INCOMPLETE ) return;
    parseGS(GS_to_menu);
}
//------------------------------------------------------------------------------
/*
  m�todo que � chamado sempre que � para se fazer
  o calculo de um movimento no jogo
*/
void GameState::simulationTick() {
    switch (globalGamestate) {
    case GS_MENU:
        return;
    case GS_CG_BEFORE_PLAY:
        return;
    case GS_PLAY:
        playState.simulationTick();
        if (playState.scene.isLevelComplete()) {
            spaceInvader->resources->setMusic(1);
            parseGS(GS_stage_clear);
        }
        //      navePrincipal.simulationTick();
        return;
    case GS_PAUSE:
        return;
    case GS_STAGE_COMPLETE:
        return;
    case GS_STAGE_INCOMPLETE:
        return;
    default: break;
    }
}
//------------------------------------------------------------------------------
void GameState::printState() {
    printf("GAME STATE\n");
    printf("     globalGamestate = %s\n", gamestateToStr(globalGamestate));
    printf("\n");
    switch (globalGamestate) {
    case GS_MENU:
        menu.printState();
        return;
    case GS_CG_BEFORE_PLAY:
        cgReproducer.printState();
        return;
    case GS_PLAY:
        playState.printState();
        //navePrincipal.printState();
        return;
    case GS_PAUSE:
        return;
    case GS_STAGE_COMPLETE:
        return;
    case GS_STAGE_INCOMPLETE:
        return;
    default: break;
    }
}
//------------------------------------------------------------------------------
void GameState::render(float deltaTime, const vec2& screenCenter) {
    switch (globalGamestate) {
    case GS_MENU:
        menu.render(deltaTime);
        return;
    case GS_CG_BEFORE_PLAY:
        cgReproducer.render(deltaTime, screenCenter);
        //      menu.render(time);
        return;
    case GS_PLAY:
        playState.render(deltaTime, screenCenter*2.0f);
        //      navePrincipal.render();
        return;
    case GS_PAUSE:
        pauseState.render(deltaTime, screenCenter);
        return;
    case GS_STAGE_COMPLETE:
        return;
    case GS_STAGE_INCOMPLETE:
        return;
    case GS_CONGRATULATIONS:
        playState.scene.renderCongratulations(screenCenter, deltaTime);
        return;
    }
}
//------------------------------------------------------------------------------
void GameState::parseGS(GAMESTATE_TOKENS_ENUM token) {
    switch (globalGamestate) {
        //menu
    case GS_MENU:
        switch (token) {
        case GS_init_cg:
            spaceInvader->resources->setMusic(1);
            playState.scene.clear();
            cgReproducer.setScoreLeve(0, 1);
            globalGamestate = GS_CG_BEFORE_PLAY;
            return;
        default: break;
        }
        return;
        //cg_before_play
    case GS_CG_BEFORE_PLAY:
        switch (token) {
        case GS_init_play:
            spaceInvader->resources->setMusic(2);
            globalGamestate = GS_PLAY;
            return;
        default: break;
        }
        return;
        //play
    case GS_PLAY:
        switch (token) {
        case GS_pause:
            globalGamestate = GS_PAUSE;
            return;
        case GS_stage_clear:
            if (playState.resetPlayStateToNextLevel()) {
                //zero o jogo
                globalGamestate = GS_CONGRATULATIONS;
                return;
            }
            cgReproducer.setScoreLeve(playState.scene.score, playState.level);
            globalGamestate = GS_CG_BEFORE_PLAY;//GS_STAGE_COMPLETE;
            return;
        case GS_bad_stage:
            globalGamestate = GS_STAGE_INCOMPLETE;
            return;
        default: break;
        }
        return;
        //pause
    case GS_PAUSE:
        switch (token) {
        case GS_init_cg:
            spaceInvader->resources->setMusic(1);
            globalGamestate = GS_CG_BEFORE_PLAY;
            return;
        case GS_pause:
            globalGamestate = GS_PLAY;
            return;
        case GS_to_menu:
            spaceInvader->resources->setMusic(0);
            globalGamestate = GS_MENU;
            return;
        default: break;
        }
        return;
        //stage complete
    case GS_STAGE_COMPLETE:
        switch (token) {
        case GS_init_cg:
            spaceInvader->resources->setMusic(1);
            globalGamestate = GS_CG_BEFORE_PLAY;
            return;
        case GS_to_menu:
            spaceInvader->resources->setMusic(0);
            globalGamestate = GS_MENU;
            return;
        default: break;
        }
        return;
        //stage incomplete
    case GS_STAGE_INCOMPLETE:
        switch (token) {
        case GS_init_cg:
            spaceInvader->resources->setMusic(1);
            globalGamestate = GS_CG_BEFORE_PLAY;
            return;
        case GS_to_menu:
            spaceInvader->resources->setMusic(0);
            globalGamestate = GS_MENU;
            return;
        default: break;
        }
        return;
    default: break;
    }
}
//------------------------------------------------------------------------------
void GameState::shoot() {
    if (globalGamestate != GS_PLAY) return;
    playState.shoot();
}
//------------------------------------------------------------------------------
void GameState::moveSpaceShip(float velocity) {
    if (globalGamestate != GS_PLAY) return;
    playState.moveSpaceShip(velocity);
}
//------------------------------------------------------------------------------
void GameState::enterPress() {
    if (globalGamestate == GS_PLAY) {
        if (!playState.scene.activateSpaceShip()) {
            spaceInvader->resources->setMusic(0);
            globalGamestate = GS_MENU;
        }
    }
    else if (globalGamestate == GS_CONGRATULATIONS) {
        spaceInvader->resources->setMusic(0);
        globalGamestate = GS_MENU;
    }
}
//------------------------------------------------------------------------------
