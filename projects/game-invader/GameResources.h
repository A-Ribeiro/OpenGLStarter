#ifndef GameResource__H
#define GameResource__H

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <wchar.h>

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>



class GameResources {
public:

    // texture
    GLTexture *ScoreImg;
    GLTexture *ExitGameImg;
    GLTexture *NewGameImg;
    GLTexture *LoadGameImg;
    GLTexture *tiroImg;
    GLTexture *TituloImg;
    GLTexture *OpenALImg;
    GLTexture *spaceshipImg;
    GLTexture *enemyImg;
    GLTexture *StarsImg;

    GLTexture *xplod[17];

    //font
    GLFont *font;

    //audio buffers
    sf::SoundBuffer *shootBuffer;
    sf::SoundBuffer *shootXplode;
    sf::SoundBuffer *shootMenuchange;
    //audio sources
    sf::Sound *shoot[4]; int shootCount;
    sf::Sound *xplode[4]; int xplodCount;
    sf::Sound *menuchange[4]; int menuCount;
    sf::Music* music[3];



    GameResources();

    ~GameResources();


    //audio ops
    void sndPlayShoot();
    void sndPlayXplode();
    void sndPlayMenuChange();

    void setMusic(int i);

};


#endif
