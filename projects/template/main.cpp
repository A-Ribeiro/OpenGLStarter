
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <glew/glew.h>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <wchar.h>

using namespace aRibeiro;
using namespace openglWrapper;

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


mat4 mvp;
GLShaderTextureColor *shader = NULL;
GLFont *font = NULL;
GLTexture *openglLogo = NULL;

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

#if NDEBUG
    // Fullscreen window
    sf::RenderWindow window(
        sf::VideoMode::getDesktopMode(),
        "Template",
        sf::Style::Fullscreen,
        contextSettings);
#else
    // 640x480 window
    sf::RenderWindow window(
        sf::VideoMode(640, 480),
        "Template",
        sf::Style::Default,
        contextSettings);
#endif

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(0);

    //
    // Check hardware capabilities
    //
    glewInit();
    if (!(GLEW_ARB_multitexture   &&
        GLEW_ARB_shading_language_100 &&
        GLEW_ARB_shader_objects       &&
        GLEW_ARB_vertex_shader        &&
        GLEW_ARB_fragment_shader)) {
        perror("Hardware does not support the required opengl features.\n");
        exit(-1);
    }

    printf("Extensions: %s\n", glGetString(GL_EXTENSIONS));

    //
    // OpenGL Initial state
    //
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glViewport(0, 0, window.getSize().x, window.getSize().y);

    //
    // Data structures
    //
    PlatformTime time;
    //mat4 mvp = mat4::IdentityMatrix * scale(vec3(0.5f));
    mvp = mat4::IdentityMatrix * scale(vec3((float)window.getSize().y / (float)window.getSize().x, 1.0f, 1.0f));

    GLSquare square;
    float color = 0.0f;

    shader = new GLShaderTextureColor();
    //load font
    {
        Font* font_raw = Font::LoadFromBasof("resources/goudy_bookletter_1911.basof");
        font = font_raw->createGLFont();
        setNullAndDelete(font_raw);
    }
    openglLogo = GLTexture::loadFromFile("resources/opengl_logo.png");

    //
    // Main Loop
    //
    time.update();
    while (window.isOpen()) {
        //
        // Update app logic
        //
        time.update();
        color = (color + time.deltaTime);
        color = fmod(color, 2.0f);

        //
        // OpenGL Draw Commands
        //
        glClear(GL_COLOR_BUFFER_BIT);

        shader->enable();

        shader->setColor(vec4(vec3(absv(color - 1.0f) * 0.4f + 0.6f), 1.0f));
        shader->setMatrix(mvp * scale(1.0f, 0.5f, 1.0f) * translate(0.0f, 1.0f, 0.0f) * scale(0.5f, 0.5f, 0.5f));

        openglLogo->active(0);
        shader->setTexture(0);

        square.draw(shader);

        openglLogo->deactive(0);

        wchar_t text[1024];// = L"Template";

        swprintf(text, 1024, L"%.0f fps", 1.0f / time.unscaledDeltaTime);

        collision::AABB aabb = font->computeBounds(text);
        vec3 center = -aabb.max_box * 0.5f;

        float fontSize = (1.0f / font->getLineHeight()) * 2.0f;
        float fontSize_10_percent = fontSize * 0.10f;

        font->getShader()->enable();
        font->getShader()->setMatrix(mvp * scale(vec3(fontSize_10_percent)) * translate(center));

        font->print(vec3(0, 0.0f, 0.0f), text);

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
    setNullAndDelete(shader);
    setNullAndDelete(font);
    setNullAndDelete(openglLogo);

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
        mvp = mat4::IdentityMatrix * scale(vec3((float)event.size.height / (float)event.size.width, 1.0f, 1.0f));
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

