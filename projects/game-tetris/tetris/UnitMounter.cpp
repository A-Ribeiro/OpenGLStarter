#include "UnitMounter.h"

#include <stdio.h>
#include <stdlib.h>

UnitMounter2D::UnitMounter2D() {
    posx = 0;
    posy = 0;
}
UnitMounter2D::~UnitMounter2D() {}
void UnitMounter2D::setUnit(const int index, const vec2 &pos) {
    positions[index] = pos;
}
bool UnitMounter2D::colide(const vec2& ptn) {
    for (int i = 0; i < 4; i++) {
        if (AABB::pointInsideAABB(ptn, AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5))))
            return true;
    }
    return false;
}
void UnitMounter2D::rotate() {};
bool UnitMounter2D::colide(const AABB &aabb) {
    for (int i = 0; i < 4; i++) {
        if (AABB::aabbOverlapsAABB(aabb, AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5))))
            return true;
    }
    return false;
}
bool UnitMounter2D::someBlockDontColide(const AABB &aabb) {
    for (int i = 0; i < 4; i++) {
        if (!AABB::aabbOverlapsAABB(aabb, AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5)))) {
            return true;
        }
    }
    return false;
}
void UnitMounter2D::turnOffDontCollided(const AABB &aabb) {
    vec2 p = positions[0];
    for (int i = 0; i < 4; i++) {
        if (AABB::aabbOverlapsAABB(aabb, AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5)))) {
            p = positions[i];
        }
    }
    blocks = 4;
    for (int i = 0; i < 4; i++) {
        if (!AABB::aabbOverlapsAABB(aabb, AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5)))) {
            positions[i] = p;
            blocks--;
        }
    }
}
void UnitMounter2D::backToNormal() {
    rotate(); rotate(); rotate(); rotate();
}
void UnitMounter2D::consoleDraw() {
    for (int i = -4; i <= 4; i++) {
        for (int j = -4; j <= 4; j++) {
            if (colide(vec2(j, -i))) {
                printf("#");
            }
            else {
                printf("o");
            }
        }
        printf("\n");
    }
}
void UnitMounter2D::glDraw() {

    RenderSystem *render = RenderSystem::getSingleton();

    AABB aabb;
    for (int i = 0; i < 4; i++) {
        aabb = AABB(positions[i] + vec2(posx, posy) - vec2(0.5), positions[i] + vec2(posx, posy) + vec2(0.5));// .draw();

        render->drawAABB_Lines(aabb, vec4(1.0f));

    }
}
vec3 UnitMounter2D::getWorldPos() {
    return vec3(posx, posy, 0);
}
int UnitMounter2D::getBlocks() {
    return blocks;
}

/*
    #
    #
    #
    #
*/
Unit1::Unit1() {
    state = 0;
    setUnit(0, vec2(0, 0));
    setUnit(1, vec2(0, 1));
    setUnit(2, vec2(0, 2));
    setUnit(3, vec2(0, 3));
}
void Unit1::rotate() {
    switch (state) {
    case 0:
        setUnit(0, vec2(0, 0));
        setUnit(1, vec2(1, 0));
        setUnit(2, vec2(2, 0));
        setUnit(3, vec2(3, 0));
        state = 1;
        break;
    case 1:
        setUnit(0, vec2(0, 0));
        setUnit(1, vec2(0, 1));
        setUnit(2, vec2(0, 2));
        setUnit(3, vec2(0, 3));
        state = 0;
        break;
    }
}

/*
    #
   ###
*/
Unit2::Unit2() {
    state = 0;
    setUnit(0, vec2(-1, 0));// #
    setUnit(1, vec2(0, 0));//###
    setUnit(2, vec2(1, 0));//
    setUnit(3, vec2(0, 1));
}
void Unit2::rotate() {
    switch (state) {
    case 0:
        setUnit(0, vec2(0, 1));// #
        setUnit(1, vec2(0, 0));// ##
        setUnit(2, vec2(0, -1));// #
        setUnit(3, vec2(1, 0));
        state = 1;
        break;
    case 1:
        setUnit(0, vec2(-1, 0));//
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));// #
        setUnit(3, vec2(0, -1));
        state = 2;
        break;
    case 2:
        setUnit(0, vec2(0, 1));// #
        setUnit(1, vec2(0, 0));//##
        setUnit(2, vec2(0, -1));// #
        setUnit(3, vec2(-1, 0));
        state = 3;
        break;
    case 3:
        setUnit(0, vec2(-1, 0));// #
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));//
        setUnit(3, vec2(0, 1));
        state = 0;
        break;
    }
}


/*
    ##
   ##
*/
Unit3::Unit3() {
    state = 0;
    setUnit(0, vec2(-1, 0));// ##
    setUnit(1, vec2(0, 0));//##
    setUnit(2, vec2(0, 1));//
    setUnit(3, vec2(1, 1));
}
void Unit3::rotate() {
    switch (state) {
    case 0:
        setUnit(0, vec2(0, 1));// #
        setUnit(1, vec2(0, 0));// ##
        setUnit(2, vec2(1, 0));//  #
        setUnit(3, vec2(1, -1));
        state = 1;
        break;
    case 1:
        setUnit(0, vec2(-1, 0));// ##
        setUnit(1, vec2(0, 0));//##
        setUnit(2, vec2(0, 1));//
        setUnit(3, vec2(1, 1));
        state = 0;
        break;
    }
}

/*
      #
    ###
*/
Unit4::Unit4() {
    state = 0;
    setUnit(0, vec2(-1, 0));//  #
    setUnit(1, vec2(0, 0));//###
    setUnit(2, vec2(1, 0));//
    setUnit(3, vec2(1, 1));
}
void Unit4::rotate() {
    switch (state) {
    case 0:
        setUnit(0, vec2(0, 1));// #
        setUnit(1, vec2(0, 0));// #
        setUnit(2, vec2(0, -1));// ##
        setUnit(3, vec2(1, -1));
        state = 1;
        break;
    case 1:
        setUnit(0, vec2(-1, 0));//
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));//#
        setUnit(3, vec2(-1, -1));
        state = 2;
        break;
    case 2:
        setUnit(0, vec2(0, 1));//##
        setUnit(1, vec2(0, 0));// #
        setUnit(2, vec2(0, -1));// #
        setUnit(3, vec2(-1, 1));
        state = 3;
        break;
    case 3:
        setUnit(0, vec2(-1, 0));//  #
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));//
        setUnit(3, vec2(1, 1));
        state = 0;
        break;
    }
}


/*
    #
    ###
*/
Unit5::Unit5() {
    state = 0;
    setUnit(0, vec2(-1, 0));//#
    setUnit(1, vec2(0, 0));//###
    setUnit(2, vec2(1, 0));//
    setUnit(3, vec2(-1, 1));
}
void Unit5::rotate() {
    switch (state) {
    case 0:
        setUnit(0, vec2(0, 1));// ##
        setUnit(1, vec2(0, 0));// #
        setUnit(2, vec2(0, -1));// #
        setUnit(3, vec2(1, 1));
        state = 1;
        break;
    case 1:
        setUnit(0, vec2(-1, 0));//
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));//  #
        setUnit(3, vec2(1, -1));
        state = 2;
        break;
    case 2:
        setUnit(0, vec2(0, 1));// #
        setUnit(1, vec2(0, 0));// #
        setUnit(2, vec2(0, -1));//##
        setUnit(3, vec2(-1, -1));
        state = 3;
        break;
    case 3:
        setUnit(0, vec2(-1, 0));//#
        setUnit(1, vec2(0, 0));//###
        setUnit(2, vec2(1, 0));//
        setUnit(3, vec2(-1, 1));
        state = 0;
        break;
    }
}

/*
    ##
    ##
*/
Unit6::Unit6() {
    state = 0;
    setUnit(0, vec2(0, 0));
    setUnit(1, vec2(0, 1));
    setUnit(2, vec2(1, 0));
    setUnit(3, vec2(1, 1));
}
void Unit6::rotate() {
    setUnit(0, vec2(0, 0));
    setUnit(1, vec2(0, 1));
    setUnit(2, vec2(1, 0));
    setUnit(3, vec2(1, 1));
}

