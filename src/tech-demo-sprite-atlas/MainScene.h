﻿#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

#include <InteractiveToolkit-Extension/image/PNG.h>
#include <InteractiveToolkit/Platform/Platform.h>

#include "components/ComponentSprite.h"

class App;

//
// Auto Generated: Exported Bitmaps inside the Font
//
#define  Font_xbox_a u8"\U00010000"
#define  Font_xbox_b u8"\U00010001"
#define  Font_xbox_x u8"\U00010002"
#define  Font_xbox_y u8"\U00010003"
#define  Font_ps_circle_color u8"\U00010004"
#define  Font_ps_cross_color u8"\U00010005"
#define  Font_ps_square_color u8"\U00010006"
#define  Font_ps_triangle_color u8"\U00010007"
#define  Font_ps_circle_white u8"\U00010008"
#define  Font_ps_cross_white u8"\U00010009"
#define  Font_ps_square_white u8"\U0001000a"
#define  Font_ps_triangle_white u8"\U0001000b"
#define  Font_L_stick u8"\U0001000c"
#define  Font_R_stick u8"\U0001000d"
#define  Font_Key_arrows u8"\U0001000e"
#define  Font_Key_z u8"\U0001000f"
#define  Font_Key_x u8"\U00010010"
#define  Font_Key_c u8"\U00010011"

class MainScene : public AppKit::GLEngine::SceneBase
{
protected:
    // to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    // to load the scene graph
    virtual void loadGraph();
    // to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    // clear all loaded scene
    virtual void unloadAll();

public:

    App *app;

    std::shared_ptr<AppKit::GLEngine::SpriteShader> spriteShader;

    // std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> componentSprite;
    std::shared_ptr<AppKit::GLEngine::Transform> spriteNode;
    std::shared_ptr<AppKit::GLEngine::Transform> logoNode;

    std::shared_ptr<AppKit::GLEngine::Components::ComponentSprite> bgComponentSprite;
    std::shared_ptr<AppKit::GLEngine::Transform> bgNode;
    

    float randomNext;
    ITKCommon::Random32 random32;
    MathCore::MathRandomExt<ITKCommon::Random32> mathRandom;

    Platform::ObjectQueue<std::shared_ptr<AppKit::GLEngine::Transform>> transformPool;

    MainScene(
        App *app,
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow
    );
    ~MainScene();

    virtual void draw();

    void resize(const MathCore::vec2i &size);

    void update(Platform::Time *elapsed);
};
