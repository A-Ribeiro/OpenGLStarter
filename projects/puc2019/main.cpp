
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4305)
#endif

#include <glew/glew.h>
#include <aribeiro/aribeiro.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <wchar.h>

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "App.h"
#include "util/AppBase.h"

#ifdef _MSC_VER
#    ifdef NDEBUG
#        pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#    else
#        pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#    endif
#endif

AppBase *app = NULL;

//
// function prototypes
//
void freeOpenGLResources();
void processSingleEvent(sf::RenderWindow &window, sf::Event &event);

int main(int argc, char* argv[]) {
    
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
    
    sf::ContextSettings contextSettings;
    contextSettings.depthBits = 24;
    contextSettings.sRgbCapable = false;
    
    // OpenGL 2.0 context
    contextSettings.majorVersion = 2;
    contextSettings.majorVersion = 1;
    
    //
    // get fullscreen modes and look the near desktop mode resolution
    //
    sf::VideoMode desktopMode = sf::VideoMode(1920, 1080);//sf::VideoMode::getDesktopMode();
    sf::VideoMode nearDesktopMode;
    const std::vector<sf::VideoMode> &modes = sf::VideoMode::getFullscreenModes();
    nearDesktopMode = modes[0];
    int distance = (desktopMode.width - modes[0].width)*(desktopMode.width - modes[0].width) +
    (desktopMode.height - modes[0].height)*(desktopMode.height - modes[0].height);
    
    for(int i=1;i<modes.size();i++) {
        int dstToTest = (desktopMode.width - modes[i].width)*(desktopMode.width - modes[i].width) +
        (desktopMode.height - modes[i].height)*(desktopMode.height - modes[i].height);
        if ( dstToTest <= distance ){
            distance = dstToTest;
            nearDesktopMode = modes[i];
        }
    }
    
    printf(" Best Fullscreen Mode Found: %ix%i\n", nearDesktopMode.width, nearDesktopMode.height);
    
    
#if NDEBUG
    // Fullscreen window
    sf::RenderWindow window(
                            nearDesktopMode,
                            //sf::VideoMode::getDesktopMode(),
                            "PUC-2019",
                            sf::Style::Fullscreen,
                            contextSettings);
#else
    // 640x480 window
    sf::RenderWindow window(
                            sf::VideoMode(800, 600),
                            "PUC-2019",
                            sf::Style::Default,
                            contextSettings);
#endif
    
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);
    
    window.setMouseCursorVisible(true);
    
    //
    // Check hardware capabilities
    //
    glewInit();
    if (!(//GLEW_ARB_multitexture   &&
          GLEW_ARB_shading_language_100 &&
          GLEW_ARB_shader_objects       &&
          GLEW_ARB_vertex_shader        &&
          GLEW_ARB_fragment_shader      &&
          GLEW_ARB_texture_non_power_of_two //&&
          //GLEW_ARB_framebuffer_object &&
          //GLEW_ARB_draw_buffers &&
          //GLEW_ARB_framebuffer_no_attachments && -- n funciona no macos
          //GLEW_ARB_depth_texture
          )) {
        perror("Hardware does not support the required opengl features.\n");
        exit(-1);
    }
    
    //
    // Data structures
    //
    app = new App(window.getSize().x, window.getSize().y);
    
    //app->WindowSize = iSize(window.getSize().x, window.getSize().y);
    //app->OnSetSize.call(window.getSize().x, window.getSize().y);
    //SpaceInvaderGlobal->setupViewport(window.getSize().x, window.getSize().y);
    
    //
    // Main Loop
    //
    while (window.isOpen()) {
        //
        // Display Game
        //
        app->draw();
        
        //
        // Swap Buffers
        //
        window.display();
        
        //
        //avoid linux 100% CPU usage
        //
        PlatformSleep::sleepMillis(1);
        
        //
        // Process any keyboard, mouse, joystick, window and system events...
        // If the command closes the window, avoid use OpenGL resources without a window...
        //
        sf::Event event;
        while (window.pollEvent(event)) {
            processSingleEvent(window, event);
            if (!window.isOpen())
                break;
        }
        
        if (app != NULL && app->canExitApplication) {
            freeOpenGLResources();
            window.close();
            break;
        }
    }
    
    
    
    return 0;
}


//
// function implementation
//
void freeOpenGLResources() {
    GLFont::releaseSharedResources();
    //RenderSystem::getSingleton()->releaseGLResources();
    setNullAndDelete(app);
}

void processSingleEvent(sf::RenderWindow &window, sf::Event &event) {
    
    // Close window : exit
    if (event.type == sf::Event::Closed) {
        freeOpenGLResources();
        window.close();
        return;
    }
    
    // Escape key : exit
    if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
        freeOpenGLResources();
        window.close();
        return;
    }
    
    if (event.type == sf::Event::KeyPressed) {
        printf("key pressed: %d \n", event.key.code);
        app->OnKeyDown(event.key.code);
    }
    
    if (event.type == sf::Event::KeyReleased) {
        printf("key released: %d \n", event.key.code);
        app->OnKeyUp(event.key.code);
    }
    
    // Adjust the viewport when the window is resized
    if (event.type == sf::Event::Resized) {
        app->WindowSize = iSize(event.size.width, event.size.height);
    }
    
    if (event.type == sf::Event::JoystickButtonPressed) {
        printf("joy: %d btn: %d\n", event.joystickButton.joystickId, event.joystickButton.button);
    }
    
    if (event.type == sf::Event::MouseButtonPressed) {
        printf(" press: %d %d \n", event.mouseButton.x, event.mouseButton.y);
        app->OnMouseDown(event.mouseButton.button, vec2((int)event.mouseButton.x, (int)window.getSize().y - 1 - (int)event.mouseButton.y));
    }
    
    if (event.type == sf::Event::MouseButtonReleased) {
        printf(" release: %d %d \n", event.mouseButton.x, event.mouseButton.y);
        app->OnMouseUp(event.mouseButton.button, vec2((int)event.mouseButton.x, (int)window.getSize().y - 1 - (int)event.mouseButton.y));
    }
    
    if (event.type == sf::Event::MouseMoved) {
        //int y = (int)window.getSize().y -1 - event.mouseMove.y;
        printf(" move: %d %d \n", event.mouseMove.x, event.mouseMove.y);
        //app->mouseMove(event.mouseMove.x, event.mouseMove.y);
        //app->OnMouseMove.call(vec2(event.mouseMove.x, window.getSize().y - 1 - event.mouseMove.y));
        
        app->MousePos = vec2((int)event.mouseMove.x, (int)window.getSize().y - 1 - (int)event.mouseMove.y);
    }
    
    if (event.type == sf::Event::MouseWheelScrolled) {
        printf(" wheel: %f \n", event.mouseWheelScroll.delta);
        if (event.mouseWheelScroll.delta > 0.9)
            app->OnMouseWheelUp();
        else if (event.mouseWheelScroll.delta < -0.9)
            app->OnMouseWheelDown();
    }
    
}
