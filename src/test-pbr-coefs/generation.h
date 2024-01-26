#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
using namespace MathCore;

#include <appkit-gl-engine/util/Generation.h>
using namespace AppKit::GLEngine;

#include <locale.h>

static void sunFlowerValues() {
    //setlocale(LC_ALL, ".OCP");
    //setlocale(LC_ALL, "pt_BR");
    std::vector<MathCore::vec3f> points = PatternPoints::sunFlower(32, 2, false);
    PatternPoints::optimize2DAccess(&points, 8, false);
    for(const auto &v: points){
        printf("(%f, %f)\n", v.x, v.y);
    }
}