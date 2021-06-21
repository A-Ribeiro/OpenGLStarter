#include <mini-gl-engine/mini-gl-engine.h>
#include "App.h"

static AppBase * CreateAppInstance() {
    return new App();
}

int main(int argc, char* argv[]) {
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    
    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    
    engine->initialSetup("Milky Way Studio", "Collision", &CreateAppInstance);

    engine->setResolution( sf::VideoMode(640,480), false, 0);
    
    engine->mainLoop();
    
    return 0;
}
