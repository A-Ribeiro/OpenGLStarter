#ifndef App__H
#define App__H

#include <mini-gl-engine/mini-gl-engine.h>

//#include "SceneJesusCross.h"
#include "SceneSplash.h"
//#include "SceneGUI.h"
#include "ScenePalace.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;
using namespace openglWrapper;

class App : public AppBase {
    RenderPipeline renderPipeline;
    PlatformTime time;
    ResourceHelper resourceHelper;

    Fade *fade;

    float timer;
    int state;
public:

    //SceneJesusCross *sceneJesusCross;
    //SceneGUI *sceneGUI;
    SceneSplash *sceneSplash;
    ScenePalace *scenePalace;

    bool mousePressed;

    App();
    virtual ~App();
    virtual void draw();

    // occures after new operator... to guarantee app access to all events on engine...
    void load();

    void onGainFocus();
    void onWindowSizeChange(Property<sf::Vector2i> *prop);

    void onMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos);
    void onMouseUp(sf::Mouse::Button button, const aRibeiro::vec2 &pos);
};

#endif
