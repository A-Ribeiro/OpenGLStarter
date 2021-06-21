#include "SpaceInvader.h"
#include "gamestate/gameState.h"

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

SpaceInvader::SpaceInvader(int w, int h) {

    resources = new GameResources();
    render = new RenderSystem(resources);

    render->setupViewport(w, h);

    gameState = new GameState(this);

    gameState->resizeWindow(w, h);

    time.update();
    simulationtick_interval = 0;

    canExitApplication = false;

    resources->setMusic(0);
}

SpaceInvader::~SpaceInvader() {
    setNullAndDelete(gameState);
    setNullAndDelete(render);
    setNullAndDelete(resources);
}

void SpaceInvader::setupViewport(int w, int h) {
    render->setupViewport(w, h);
    gameState->resizeWindow(w, h);
}

void SpaceInvader::draw() {
    time.update();
    processInput();

    const float interval = 0.030f;

    simulationtick_interval += time.deltaTime;
    while (simulationtick_interval > interval) {
        simulationtick_interval -= interval;
        //gameState->simulationTick();
    }

    if (time.deltaTime > 0.050f)
        time.deltaTime = 0.050f;

    gameState->simulationTick();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render->model.top = mat4::IdentityMatrix;

    gameState->render(time.deltaTime, render->dimension*0.5);

    //
    // print FPS
    //
    render->pushProjetionAndConfigureParallel();

    static wchar_t str[20];
    swprintf(str,20, L" \nFPS: %2.2f", 1.0f / time.unscaledDeltaTime);

    glDisable(GL_DEPTH_TEST);

    render->print(str,
        vec2(10.0f, render->dimension.y - 10.0f),
        vec4(1.0f),
        0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    /*

    render->drawAABB(
        AABB(vec2(-10.0f), vec2(10.0f)),
        vec4(0.0f, 1.0f, 0.0f, 1.0f)
    );

    render->model.top *= translate(vec3(vec2(10.0f, render->dimension.y - 10.0f),0.0f));

    render->drawAABB (
        AABB(vec2(-10.0f,-10.0f), vec2(10.0f, 10.0f)),
        vec4(0.0f, 1.0f, 1.0f, 1.0f)
    );

    render->model.top *= translate(vec3(vec2(0, -resources->font->getLineHeight()), 0.0f));

    render->drawAABB(
        AABB(vec2(-10.0f, -10.0f), vec2(10.0f, 10.0f)),
        vec4(0.0f, 1.0f, 1.0f, 1.0f)
    );

    */


    /*
    GLShaderFont * shader = resources->font->getShader();
    shader->enable();
    shader->setMatrix(render->getMVP());
    resources->font->print(vec3(1.0, 1.0, 1.0), str);
    */



    render->popProjection();

}


const int
//XBOX_LB = 4,
//XBOX_RB = 5,
XBOX_START = 7,
XBOX_A = 0;

void SpaceInvader::processInput() {

    bool composedLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool composedRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool composedShoot = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bool composedStart = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);

    if (sf::Joystick::isConnected(0)) {
        float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
        float povx = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);

        composedLeft = composedLeft || (x < -50.0f) || (povx < -50.0f);
        composedRight = composedRight || (x > 50.0f) || (povx > 50.0f);
        composedShoot = composedShoot || sf::Joystick::isButtonPressed(0, XBOX_A);
        composedStart = composedStart || sf::Joystick::isButtonPressed(0, XBOX_START);
    }

    btnLeft.setState(composedLeft);
    btnRight.setState(composedRight);
    btnShoot.setState(composedShoot);
    btnStart.setState(composedStart);

    if (btnLeft.down)
        gameState->setMenuInput(MT_PREV_OPTION);
    
    if (btnRight.down)
        gameState->setMenuInput(MT_NEXT_OPTION);
    
    if (btnShoot.down)
        gameState->shoot();

    if (btnStart.down) {
        //esta verificação deve vir antes para evitar que
        //  um enter faça efeito nos dois elementos em estados diferentes
        //        cgReproducer e menu
        if (!gameState->cgReproducerIsTerminatedCheck()) {
            //se não consumiu o evento
            if (gameState->setMenuInput(MT_OK)) {
                canExitApplication = true;
                return;
            }
        }
        gameState->enterPress();
    }

    float speed = 0;
    if (btnLeft.pressed)
        speed = -0.05f;
    if (btnRight.pressed)
        speed = +0.05f;
    gameState->moveSpaceShip(speed);


    /*


    bool keyL = false;
    bool keyLrelease = false;

    bool keyR = false;
    bool keyRrelease = false;

    bool menuOK = false;
    bool menuOKrelease = false;

    bool pause = false;
    bool pauserelease = false;




    //keyboard processing
    {
        bool L = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        static bool oldL = false;
        keyL = keyL || (!oldL && L);
        keyLrelease = keyLrelease || (oldL && !L);
        oldL = L;

        bool R = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        static bool oldR = false;
        keyR = keyR || (!oldR && R);
        keyRrelease = keyRrelease || (oldR && !R);
        oldR = R;


        bool ret = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        static bool oldret = false;
        menuOK = menuOK || (!oldret && ret);
        menuOKrelease = menuOKrelease || (oldret && !ret);
        oldret = ret;

        bool pkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);
        static bool oldpkey = false;
        pause = pause || (!oldpkey && pkey);
        pauserelease = pauserelease || (oldpkey && !pkey);
        oldpkey = pkey;



    }

    if (sf::Joystick::isConnected(0)) {


        //keyEnter = keyEnter || sf::Joystick::isButtonPressed(0, XBOX_START) || sf::Joystick::isButtonPressed(0, XBOX_A);

        //keyL = keyL || sf::Joystick::isButtonPressed(0, XBOX_LB);
        //keyR = keyR || sf::Joystick::isButtonPressed(0, XBOX_RB);

        float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);

        bool xL = (x < -50.0f);
        static bool oldXL = false;
        keyL = keyL || (!oldXL && xL);
        keyLrelease = keyLrelease || (oldXL && !xL);
        oldXL = xL;

        bool xR = (x > 50.0f);
        static bool oldXR= false;
        keyR = keyR || (!oldXR && xR);
        keyRrelease = keyRrelease || (oldXR && !xR);
        oldXR = xR;


        float povx = sf::Joystick::getAxisPosition(0, sf::Joystick::PovX);


        bool pov_xL = (povx < -50.0f);
        static bool pov_oldXL = false;
        keyL = keyL || (!pov_oldXL && pov_xL);
        keyLrelease = keyLrelease || (pov_oldXL && !pov_xL);
        pov_oldXL = pov_xL;

        bool pov_xR = (povx > 50.0f);
        static bool pov_oldXR = false;
        keyR = keyR || (!pov_oldXR && pov_xR);
        keyRrelease = keyRrelease || (pov_oldXR && !pov_xR);
        pov_oldXR = pov_xR;

        //keyL = keyL || (x < -50.0f) || (povx < -50.0f);
        //keyR = keyR || (x > 50.0f) || (povx > 50.0f);


        bool btna = sf::Joystick::isButtonPressed(0, XBOX_A);
        static bool oldbtna = false;
        menuOK = menuOK || (!oldbtna && btna);
        menuOKrelease = menuOKrelease || (oldbtna && !btna);
        oldbtna = btna;


        bool btnstart = sf::Joystick::isButtonPressed(0, XBOX_START);
        static bool oldbtnstart = false;
        pause = pause || (!oldbtnstart && btnstart);
        pauserelease = pauserelease || (oldbtnstart && !btnstart);
        oldbtnstart = btnstart;

    }

    //
    // Second pass processing of the input
    //

    static float keepMoveLeft = false;
    static float keepMoveRight = false;

    if (keyL) {
        gameState->setMenuInput(MT_PREV_OPTION);
        keepMoveLeft = true;
    }

    if (keyLrelease) {
        keepMoveLeft = false;
    }

    if (keyR) {
        gameState->setMenuInput(MT_NEXT_OPTION);
        keepMoveRight = true;
    }

    if (keyRrelease) {
        keepMoveRight = false;
    }

    if (menuOK) {

        //gameState->enterPress();

        gameState->shoot();
    }

    if (menuOKrelease) {

    }

    if (pause) {

        //esta verificação deve vir antes para evitar que
          //  um enter faça efeito nos dois elementos em estados diferentes
          //        cgReproducer e menu
        if (!gameState->cgReproducerIsTerminatedCheck()) {
            //se não consumiu o evento
            if (gameState->setMenuInput(MT_OK)) {
                canExitApplication = true;
                return;
            }
        }

        gameState->enterPress();
    }

    if (pauserelease) {
    }

    //
    // third level of processing
    //

    float speed = 0;

    if (keepMoveLeft) {
        speed = -0.05f;
    }

    if (keepMoveRight) {
        speed = +0.05f;
    }

    gameState->moveSpaceShip(speed);


    */
}

