
#include <glew/glew.h>
#include <aribeiro/aribeiro.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "EasingEqDrawer.h"

using namespace aRibeiro;

//
// Make windows release build to run in a window without a console window
//
#ifdef _MSC_VER
#    ifdef NDEBUG
#        pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#    else
#        pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#    endif
#endif


GLShaderColor *shaderColor = NULL;
GLFont *font = NULL;
mat4 mvp;

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
    contextSettings.majorVersion = 0;

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
        "Template",
        sf::Style::Fullscreen,
        contextSettings);
#else
    // 640x480 window
    sf::RenderWindow window(
        sf::VideoMode(640, 480, 32),
        "Template",
        sf::Style::Default,
        contextSettings);
#endif
 // /opt/vc/lib/libbrcmEGL.so /opt/vc/lib/libbrcmGLESv2.so
//OpenGL ES-CM 1.1
 /*
/usr/bin/c++   -g
CMakeFiles/aribeiro-test-easing-eq.dir/main.cpp.o  -o ../../bin/aribeiro-test-easing-eq
-L/opt/vc/lib -Wl,-rpath,/opt/vc/lib
../../bin/libaribeirod.a
../../bin/libsfml-window-s-d.a
../../bin/libsfml-graphics-s-d.a
../../bin/libglewd.a
../../bin/libconvertutfd.a
../../bin/libpng16d.a
../../bin/libzd.a
-lbcm_host
-lopenmaxil
-lvcos
-lvchiq_arm
-lpthread
../../bin/libsfml-window-s-d.a
/lib/arm-linux-gnueabihf/libudev.so
../../bin/libsfml-system-s-d.a
-lrt
/opt/vc/lib/libbrcmEGL.so
/opt/vc/lib/libbrcmGLESv2.so
/usr/lib/arm-linux-gnueabihf/libfreetype.so
/usr/lib/arm-linux-gnueabihf/libjpeg.so
/usr/lib/arm-linux-gnueabihf/libm.so
-lpthread

 */

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    // clear current error... the window system seems to create an OpenGL Context with an error in GL commands...
    //glGetError();


    const char* str;
    OPENGL_CMD ( str = (const char*)glGetString(GL_VERSION) );
    printf("str: %s\n",str);
    // OpenGL ES-CM 1.1

    //
    // Check hardware capabilities
    //
    #ifndef ARIBEIRO_RPI
    glewInit();
    if (!(GLEW_ARB_multitexture   &&
        GLEW_ARB_shading_language_100 &&
        GLEW_ARB_shader_objects       &&
        GLEW_ARB_vertex_shader        &&
        GLEW_ARB_fragment_shader)) {
        perror("Hardware does not support the required opengl features.\n");
        exit(-1);
    }
    #endif

    //
    // OpenGL Initial state
    //
    OPENGL_CMD( glDisable(GL_DEPTH_TEST) );
    OPENGL_CMD( glEnable(GL_CULL_FACE) );
    OPENGL_CMD( glFrontFace(GL_CCW) );

    OPENGL_CMD( glEnable(GL_BLEND) );
    OPENGL_CMD( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

    glLineWidth(2.0f);
    #ifndef ARIBEIRO_RPI
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glPointSize(4.0f);
    #endif

    OPENGL_CMD( glClearColor(1.0f, 1.0f, 1.0f, 0.0f) );

    OPENGL_CMD( glViewport(0, 0, window.getSize().x, window.getSize().y) );

    //
    // Data structures
    //
    PlatformTime time;
    time.timeScale = 0.25f;

    mvp = mat4_IdentityMatrix * scale(vec3((float)window.getSize().y / (float)window.getSize().x, 1.0f, 1.0f));
    shaderColor = new GLShaderColor();

    //load font
    {
        Font* font_raw = Font::LoadFromBasof("resources/goudy_bookletter_1911.basof");
        font = font_raw->createGLFont();
        setNullAndDelete(font_raw);
    }

    EasingEqDrawer eqDrawer;

    //
    // Main Loop
    //
    time.update();
    while (window.isOpen()) {
        //
        // Update app logic
        //
        time.update();
        //printf("fps: %f\n", 1.0f / time.unscaledDeltaTime);

        //
        // OpenGL Draw Commands
        //
        glClear(GL_COLOR_BUFFER_BIT);

        shaderColor->enable();

        eqDrawer.drawAllFunctions(mvp, shaderColor, font, &time);

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
    }



    return 0;
}


//
// function implementation
//
void freeOpenGLResources() {

    GLFont::releaseSharedResources();

    setNullAndDelete(shaderColor);
    setNullAndDelete(font);
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

    // Adjust the viewport when the window is resized
    if (event.type == sf::Event::Resized) {
        glViewport(0, 0, event.size.width, event.size.height);
        mvp = mat4_IdentityMatrix * scale(vec3((float)event.size.height / (float)event.size.width, 1.0f, 1.0f));
    }

    if (event.type == sf::Event::JoystickButtonPressed) {
        printf("joy: %d btn: %d\n", event.joystickButton.joystickId, event.joystickButton.button);
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        printf(" press: %d %d \n", event.mouseButton.x, event.mouseButton.y);
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        printf(" release: %d %d \n", event.mouseButton.x, event.mouseButton.y);
    }

    if (event.type == sf::Event::MouseMoved) {
        printf(" move: %d %d \n", event.mouseMove.x, event.mouseMove.y);
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        printf(" wheel: %f \n", event.mouseWheelScroll.delta);
    }
}

