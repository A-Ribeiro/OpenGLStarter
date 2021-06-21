#ifndef App__H
#define App__H

#include <mini-gl-engine/mini-gl-engine.h>

#include "SceneSplash.h"
#include "SceneGUI.h"

#include "SceneMary.h"
#include "SceneJesus.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;
using namespace openglWrapper;

enum LoadActionsEnum{
    LoadActions_None,
    LoadActions_Mary,
    LoadActions_Jesus
};

class App : public AppBase {
    RenderPipeline renderPipeline;
    PlatformTime time;
    ResourceHelper resourceHelper;

    Fade *fade;
    
    //fade aux variables
    float timer;
    int state;

    LoadActionsEnum currentScene;
    
public:

    SceneGUI *sceneGUI;
    SceneSplash *sceneSplash;
    GLEngine::SceneBase *activeScene;

    App();
    virtual ~App();
    virtual void draw();

    // occures after new operator... to guarantee app access to all events on engine...
    void load();

    void onGainFocus();
    void onWindowSizeChange(Property<sf::Vector2i> *prop);
    void onMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos);

    void changeScene(LoadActionsEnum scene);

};

#endif
