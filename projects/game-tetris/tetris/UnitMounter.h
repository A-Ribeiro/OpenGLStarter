#ifndef unitMounter_h
#define unitMounter_h

#include "../common/common.h"

class UnitMounter2D {
private:
    int blocks;
public:
    int posx, posy;
    vec2 positions[4];//quatro unidades por peça
protected:
    int state;
    UnitMounter2D();
    virtual ~UnitMounter2D();
    void setUnit(const int index, const vec2 &pos);
    bool colide(const vec2& ptn);
public:
    virtual void rotate();
    bool colide(const AABB &aabb);
    bool someBlockDontColide(const AABB &aabb);
    void consoleDraw();
    void glDraw();
    void backToNormal();
    int getBlocks();
    void turnOffDontCollided(const AABB &aabb);
    vec3 getWorldPos();
};

/*
    #
    #
    #
    #
*/
class Unit1 :public UnitMounter2D {
public:
    Unit1();
    void rotate();
};

/*
    #
   ###
*/
class Unit2 :public UnitMounter2D {
public:
    Unit2();
    void rotate();
};


/*
    ##
   ##
*/
class Unit3 :public UnitMounter2D {
public:
    Unit3();
    void rotate();
};

/*
      #
    ###
*/
class Unit4 :public UnitMounter2D {
public:
    Unit4();
    void rotate();
};


/*
    #
    ###
*/
class Unit5 :public UnitMounter2D {
public:
    Unit5();
    void rotate();
};

/*
    ##
    ##
*/
class Unit6 :public UnitMounter2D {
public:
    Unit6();
    void rotate();
};


#endif
