#include <mini-gl-engine/mini-gl-engine.h>
#include "App.h"

static AppBase * CreateAppInstance() {
    App *app = new App();
    app->load();
    return app;
}

int main(int argc, char* argv[]) {
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));

    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    engine->initialSetup("Milky Way Studio", "Tech Demo: Soft Particle", &CreateAppInstance);

#ifdef ARIBEIRO_RPI
    engine->setResolution( sf::VideoMode(1280,720), true, true, 0);
#else
    engine->setResolution( sf::VideoMode(852,480), false, true, 1);
#endif


    engine->mainLoop();

    return 0;
}
