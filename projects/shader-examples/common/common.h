#ifndef common_h
#define common_h

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4305)
#endif

#include <aribeiro/aribeiro.h>

using namespace aRibeiro;

#include "GUI/all.h"
#include "SysHelper/all.h"
#include "SysHelper/TinyXML/all.h"

#include "Simulation.h"

/*

#include <SFML/Audio.hpp>

class AlMultiSource {

    sf::SoundBuffer *audioBuffer;
    sf::Sound *audioSource[10];
    int count;

public:
    AlMultiSource(const char*audio) {

        audioBuffer = new sf::SoundBuffer();
        audioBuffer->loadFromFile(audio);

        for (int i = 0; i < 10; i++)
            audioSource[i] = new sf::Sound(*audioBuffer);

        count = 0;

    }

    ~AlMultiSource() {

        for (int i = 0; i < 10; i++)
            setNullAndDelete(audioSource[i]);
        setNullAndDelete(audioBuffer);
    }

    void play() {
        audioSource[count]->play();
        count = (count + 1) % 4;
    }

    bool isPlaying() {

        for (int i = 0; i < 10; i++)
            if (audioSource[i]->getStatus() == sf::Sound::Playing)
                return true;

        return false;

    }

    void setGain(float v) {
        for (int i = 0; i < 10; i++)
            audioSource[i]->setVolume(v * 100.0f);
    }

};

class AlMultiBuffer {

    sf::Music* music;
    float volume;

public:
    AlMultiBuffer(const char*audio) {
        music = new sf::Music();
        music->openFromFile(audio);
        volume = 1.0f;
    }

    ~AlMultiBuffer() {
        setNullAndDelete(music);
    }

    void play() {
        if (!isPlaying())
            music->play();
    }


    bool isPlaying() {
        return music->getStatus() == sf::SoundStream::Playing;
    }


    void stop() {
        music->stop();
    }

    void setGain(float v) {
        volume = v;
        music->setVolume(volume * 100.0f);
    }

    void changeStreamFile(const char* audio) {
        setNullAndDelete(music);
        music = new sf::Music();
        music->openFromFile(audio);
        music->setVolume(volume * 100.0f);
        music->play();

    }

    void checkBuffersRoundUp() {

    }

};

*/

#endif
