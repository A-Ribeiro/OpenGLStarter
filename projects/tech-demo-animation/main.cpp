#include <mini-gl-engine/mini-gl-engine.h>
#include "App.h"

static AppBase * CreateAppInstance() {
    App *app = new App();
    app->load();
    return app;
}

int main(int argc, char* argv[]) {
    
    /*
    FILE* fp;
    AllocConsole();
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    // */

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));

    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    engine->initialSetup("Milky Way Studio", "Tech Demo: Animation", &CreateAppInstance);

#ifdef ARIBEIRO_RPI
    engine->setResolution( sf::VideoMode(1280,720), true, true, 0);
#else
#if defined(NDEBUG)
    //fullscreen
    engine->setResolution(engine->getResolutionList()[0], true, true, 1);
#else
    engine->setResolution( sf::VideoMode(852,480), false, true, 1);
#endif
#endif


    engine->mainLoop();

    return 0;
}
