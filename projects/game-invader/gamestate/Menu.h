
#ifndef menu_hpp
#define menu_hpp

#include "../SpaceInvader.h"

//#include <GL/gl.h>
//#include "../GLee/glee.h"
//#include "../math/all.h"

//#include "../../../common/common.h"

#include "../StarsBackGroundDraw.h"

/*

 Estados Menu - primeira vers�o

 ------------
 | New Game |
 ------------
      *
      |
      v
 -------------
 | Load Game |
 -------------
      *
      |
      v
  ----------   <score_enter>    --------------
  | SCORES |  *---------------> | SHOW SCORE |
  ----------                    --------------
      *
      |
      v
 -------------
 | Exit Game |
 -------------
*/

enum MENU_TOKENS{
  MT_NEXT_OPTION,
  MT_PREV_OPTION,
  MT_OK,
  MT_ESCAPE
};
enum MENU_STATES{
  MS_NEW_GAME=0,
  MS_LOAD_GAME,
  MS_SCORES,
  MS_EXIT_GAME,
  MS_TOTAL_STATES
};

const char* menuStateToStr(MENU_STATES state);

const vec2 ItemMenuPos[]={vec2(0.0,0),
                          vec2(16.0*1.2,0),
                          vec2(24.0*1.2,0),
                          vec2(8.0*1.2,0)};

class Menu{
  private:
    /*texturas das mensagens dos estados do Menu*/
    GLTexture *textureStates[MS_TOTAL_STATES];
//    GLuint stars;
    StarsBackGroundDraw starsDraw;
    //GLuint Titulo, OpenALSymbol;

    MENU_STATES menuState;


    vec2 drawPos, initDraw, finishDraw;
//    unsigned int initialTime, finalTime, currentTime;

    float currentTimef;
    void parser(MENU_TOKENS token);

    SpaceInvader *spaceInvader;

  public:
    Menu(SpaceInvader *si);
    ~Menu();
    //retorna true se a token de entrada foi um OK
    bool inputMenu(MENU_TOKENS token);
    MENU_STATES getMenuState();
    void printState();
    void render(float deltaTime);
};

#endif
