#ifndef SpaceInvader__H
#define SpaceInvader__H

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4305)

#endif

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include <wchar.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>

#include "RenderSystem.h"
#include "GameResources.h"

class GameState;

class SpaceInvader {
public:
    bool canExitApplication;
    RenderSystem *render;
    GameResources *resources;
    GameState *gameState;

    vec2 mousePos;
    PlatformTime time;
    float simulationtick_interval;

    PressReleaseDetector btnLeft, btnRight, btnShoot, btnStart;

    SpaceInvader(int w, int h);

    virtual ~SpaceInvader();

    void setupViewport(int w, int h);
    void processInput();
    void draw();

    SSE2_CLASS_NEW_OPERATOR
};



#endif
