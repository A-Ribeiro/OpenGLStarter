#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

#include <InteractiveToolkit-Extension/image/PNG.h>
#include <InteractiveToolkit/Platform/Platform.h>

class App;

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

    Platform::ThreadPool mThreadPool;

public:

    App *app;

    AppKit::OpenGL::GLFont2Builder fontBuilder;

    std::shared_ptr<AppKit::GLEngine::Transform> scaleNode;

    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> font_line1;
    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> font_line2;
    
    AppKit::OpenGL::GLDynamicFBO fbo;
    AppKit::OpenGL::GLTexture colorTexture;
    
    struct Task {
        std::string out_filename;
        AppKit::OpenGL::TextureBuffer texBuffer;
    };
    Platform::ObjectQueue<Task> queue;

    // AppKit::OpenGL::TextureBuffer texBuffer;

    // std::string text;
    // float text_size;
    // float text_margin;

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

    void setText(
        const std::string &text, 
        float _size, 
        float window_width,
        std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> toMesh);
    
    void centerAllMesh();

    void setTextWithWidth(float width);

    enum class RecordingState: uint8_t {
        NONE,
        CLEAR_SCREEN,
        RECORDING
    };

    RecordingState recording;

    const float fade_time_sec = 4;
    const float complete_time_sec = 7;
    const float total_time_sec = 15;
    const float fps_render_image = 30;
    const float fps_interval_sec = 1.0f / fps_render_image;
    
    float time_sec;

    int video_image_count;

    float max_text_width;


    void update(Platform::Time *elapsed);
};
