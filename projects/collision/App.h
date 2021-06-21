#ifndef App__H
#define App__H

#include <mini-gl-engine/mini-gl-engine.h>
using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;
using namespace openglWrapper;

#include "util/ReferenceCounter.h"

class App : public AppBase {
    GLRenderState *renderState;
    GLShaderColor *shaderColor;
    PlatformTime time;
    //
    // Graph Variables
    //
    Transform *root;
    Transform* camera1;
    Transform* cube;
    
    ComponentCameraPerspective *cameraPerspective;
    ReferenceCounter<Component*>comps;
    
    // Scene Graph Ops
    void render_pipeline();
    
    bool traverse_render(Transform *element, void* userData);
    bool traverse_frustum(Transform *element, void *userData);
    bool traverse_delete(Transform *element, void* userData);
    
public:
    App();
    virtual ~App();
    virtual void draw();

    void onGainFocus();
    void onWindowSizeChange(Property<sf::Vector2i> *prop);

};

#endif
