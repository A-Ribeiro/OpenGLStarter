#ifndef tetris_h
#define tetris_h

#include "../common/common.h"
#include "UnitMounter.h"
#include "TableMounter.h"

enum BtnDefs {
    Rotate = 0,
    Down,
    Left,
    Right,
    AllBtnDefs
};

struct HorizontalFlashLight {
    float height;
    unsigned int lifetime, totalLifetime;
};

struct movableCube {
    vec3 force;
    vec3 position;
    unsigned int lifetime;
};

#include <queue>

class Tetris :public GenericSimulation {
    mat4 oglM;

    TableMounter2D table;
    UnitMounter2D *currentUnit;
    Unit1 u1, su1;  Unit2 u2, su2;  Unit3 u3, su3;
    Unit4 u4, su4;  Unit5 u5, su5;  Unit6 u6, su6;
    UnitMounter2D *units[6], *unitsToShow[6];
    bool keyState[AllBtnDefs];
    bool endGame;
    int nextUnit;
    unsigned int lvlInterval;
    AlMultiSource *whoosh, *plink, *boom;
    int score;
    mat4 world;

    unsigned int lastRender;
    int countdownToShowX2, countdownToShowX3, countdownToShowX4;
    GLTexture *x2, *x3, *x4;

    std::queue<movableCube> movableCubes;
    std::queue<HorizontalFlashLight> flashs;
    int timeComformant;

    void addExplosionEffect(const std::vector<vec3> &positions, const std::vector<vec3> &linhas);
    void drawMovableCubes(const unsigned int time_interval);
public:
    int selectRandomUnit();
    Tetris(AlMultiSource *whoosh, AlMultiSource *plink, AlMultiSource *boom);
    virtual ~Tetris();
    void processSimulation(const unsigned int counter, const unsigned int time_ms);
    void player1PushBtn(const BtnDefs btn);
    void player1ReleaseBtn(const BtnDefs btn);
    void render(const unsigned int time_ms, const vec2&screenCenter);
    bool isGameEnd();
    bool isPause();
    void pause();
    int getScore();
    vec3 getViewPos();
};


#endif
