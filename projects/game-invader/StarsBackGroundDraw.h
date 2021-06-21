#ifndef StarsBackGroundDraw_hpp
#define StarsBackGroundDraw_hpp

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <wchar.h>

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>

#include "SpaceInvader.h"


class StarsBackGroundDraw{
  private:
    SpaceInvader * spaceInvader;
    int numberRef;
    
    //GLuint stars;
    //void createStars();
    //void destroyStars();


    //GLuint dynStars;
    float intensidade;
    float speed;
  public:
    StarsBackGroundDraw(SpaceInvader * spaceInvader);
    ~StarsBackGroundDraw();
    void draw(float deltaTime);
    void customDraw(const vec2 &deslocamento);
    float getLastStarsIntensity();
    void setSpeed(float spd);
    float getSpeed();
};

#endif
