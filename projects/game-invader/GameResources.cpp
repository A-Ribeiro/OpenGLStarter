#include "GameResources.h"

GameResources::GameResources() {

    ScoreImg = GLTexture::loadFromFile("resources/menu/ScoreImg.png");
    ExitGameImg = GLTexture::loadFromFile("resources/menu/ExitGameImg.png");
    NewGameImg = GLTexture::loadFromFile("resources/menu/NewGameImg.png");
    LoadGameImg = GLTexture::loadFromFile("resources/menu/LoadGameImg.png");
    tiroImg = GLTexture::loadFromFile("resources/menu/tiroImg.png");
    TituloImg = GLTexture::loadFromFile("resources/menu/TituloImg.png");
    OpenALImg = GLTexture::loadFromFile("resources/menu/OpenALImg.png");
    spaceshipImg = GLTexture::loadFromFile("resources/menu/spaceshipImg.png");
    enemyImg = GLTexture::loadFromFile("resources/menu/enemyImg.png");
    StarsImg = GLTexture::loadFromFile("resources/menu/StarsImg.png");

    for (int i = 0; i < 17; i++) {
        char txt[64];
        sprintf(txt, "resources/menu/xplod/xplod%.2d.png", i);
        xplod[i] = GLTexture::loadFromFile(txt);
    }

    //load font
    {
        Font* font_raw = Font::LoadFromBasof("resources/font/SmartieCAPS.basof");
        font = font_raw->createGLFont();
        setNullAndDelete(font_raw);
    }

    //load audios
    {
        shootBuffer = new sf::SoundBuffer();
        shootBuffer->loadFromFile("resources/audio/laser.ogg");
        shootXplode = new sf::SoundBuffer();
        shootXplode->loadFromFile("resources/audio/explosion.ogg");
        shootMenuchange = new sf::SoundBuffer();
        shootMenuchange->loadFromFile("resources/audio/menu1.ogg");

        for (int i = 0; i < 4; i++) {
            shoot[i] = new sf::Sound(*shootBuffer);
            xplode[i] = new sf::Sound(*shootXplode);
            menuchange[i] = new sf::Sound(*shootMenuchange);
        }

        music[0] = new sf::Music();
        music[0]->openFromFile("resources/audio/menu.ogg");
        music[0]->setLoop(true);
        music[1] = new sf::Music();
        music[1]->openFromFile("resources/audio/drums.ogg");
        music[1]->setLoop(true);
        music[2] = new sf::Music();
        music[2]->openFromFile("resources/audio/game.ogg");
        music[2]->setLoop(true);

        shootCount = 0;
        xplodCount = 0;
        menuCount = 0;
    }

}

GameResources::~GameResources() {

    setNullAndDelete(ScoreImg);
    setNullAndDelete(ExitGameImg);
    setNullAndDelete(NewGameImg);
    setNullAndDelete(LoadGameImg);
    setNullAndDelete(tiroImg);
    setNullAndDelete(TituloImg);
    setNullAndDelete(OpenALImg);
    setNullAndDelete(spaceshipImg);
    setNullAndDelete(enemyImg);
    setNullAndDelete(StarsImg);
    for (int i = 0; i < 17; i++)
        setNullAndDelete(xplod[i]);

    setNullAndDelete(font);

    for (int i = 0; i < 4; i++) {
        setNullAndDelete(shoot[i]);
        setNullAndDelete(xplode[i]);
        setNullAndDelete(menuchange[i]);
    }
    setNullAndDelete(shootBuffer);
    setNullAndDelete(shootXplode);
    setNullAndDelete(shootMenuchange);

    setNullAndDelete(music[0]);
    setNullAndDelete(music[1]);
    setNullAndDelete(music[2]);

}


//audio ops
void GameResources::sndPlayShoot() {
    shoot[shootCount]->play();
    shootCount = (shootCount + 1) % 4;
}
void GameResources::sndPlayXplode() {
    xplode[shootCount]->play();
    xplodCount = (xplodCount + 1) % 4;
}
void GameResources::sndPlayMenuChange() {
    menuchange[shootCount]->play();
    menuCount = (menuCount + 1) % 4;
}

void GameResources::setMusic(int i) {
    if (i == 0) {
        if (music[0]->getStatus() != sf::SoundStream::Playing)
            music[0]->play();
    }
    else
        if (music[0]->getStatus() != sf::SoundStream::Stopped)
            music[0]->stop();

    if (i == 1) {
        if (music[1]->getStatus() != sf::SoundStream::Playing)
            music[1]->play();
    }
    else
        if (music[1]->getStatus() != sf::SoundStream::Stopped)
            music[1]->stop();

    if (i == 2) {
        if (music[2]->getStatus() != sf::SoundStream::Playing)
            music[2]->play();
    }
    else
        if (music[2]->getStatus() != sf::SoundStream::Stopped)
            music[2]->stop();

}
