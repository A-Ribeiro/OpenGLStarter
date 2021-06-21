
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

class _SSE2_ALIGN_PRE App {
public:
    PlatformTime time;
    sf::RenderWindow *window;
    float w, h;

    mat4 projection;

    GLTexture *openglLogo;
    GLSquare square;
    GLShaderTextureColor *shader;

    GLFont2Builder *fontBuilder_80;
    GLFont2Builder *fontBuilder_40;
    GLFont2VBO fontVBO_80;
    GLFont2VBO fontVBO_40;
    GLShaderFont2 shaderFont2;

    StringUtil stringUtil;



    App(sf::RenderWindow *_window) {
        window = _window;
        openglLogo = NULL;
        fontBuilder_80 = NULL;
        fontBuilder_40 = NULL;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        shader = new GLShaderTextureColor();
        openglLogo = GLTexture::loadFromFile("resources/opengl_logo.png");

        fontBuilder_80 = new GLFont2Builder();
        fontBuilder_80->load("resources/Roboto-Regular-80.basof2");

        fontBuilder_40 = new GLFont2Builder();
        fontBuilder_40->load("resources/Roboto-Regular-40.basof2");

        fontBuilder_80->glFont2.texture.generateMipMap();
        fontBuilder_80->glFont2.texture.setAnisioLevel(16.0f);

        fontBuilder_40->glFont2.texture.generateMipMap();
        //fontBuilder_40->glFont2.texture.setAnisioLevel(16.0f);

        resize();
    }

    ~App() {
        setNullAndDelete(fontBuilder_40);
        setNullAndDelete(fontBuilder_80);
        setNullAndDelete(openglLogo);
        setNullAndDelete(shader);
    }

    void resize() {
        w = window->getSize().x;
        h = window->getSize().y;
        projection = projection_ortho_rh_negative_one(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, -1000.0f, 1000.0f);
        glViewport(0, 0, window->getSize().x, window->getSize().y);
    }

    void draw() {
        time.update();

        stringUtil.printf("Elapsed:\n%f", time.deltaTime);

        glClear(GL_COLOR_BUFFER_BIT);

        //draw OpenGL Logo
        shader->enable();
        shader->setColor(vec4(1,1,1,1));
        shader->setMatrix(projection * scale(0.5f*(float)openglLogo->width, 0.5f*(float)openglLogo->height, 1.0f) );
        openglLogo->active(0);
        shader->setTexture(0);
        square.draw(shader);
        openglLogo->deactive(0);

        //draw GLFont2
        fontBuilder_80->strokeColor = vec4(0.25, 0.25, 0.25, 1);
        fontBuilder_80->horizontalAlign = GLFont2HorizontalAlign_center;
        fontBuilder_80->verticalAlign = GLFont2VerticalAlign_middle;
        fontBuilder_80->build(stringUtil.char_ptr());

        fontVBO_80.uploadData(fontBuilder_80);

        fontBuilder_40->strokeColor = vec4(0.25, 0.25, 0.25, 1);
        fontBuilder_40->horizontalAlign = GLFont2HorizontalAlign_center;
        fontBuilder_40->verticalAlign = GLFont2VerticalAlign_middle;
        fontBuilder_40->build(stringUtil.char_ptr());

        fontVBO_40.uploadData(fontBuilder_40);


        shaderFont2.enable();
        shaderFont2.setTexture(0);

        shaderFont2.setMatrix(projection * scale(vec3(40.0f / fontBuilder_80->glFont2.size)));
        fontBuilder_80->glFont2.texture.active(0);
        fontVBO_80.draw(GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);

        shaderFont2.setMatrix(projection * translate(0, -100.0f, 0) );
        fontBuilder_40->glFont2.texture.active(0);
        fontVBO_40.draw(GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);

        shaderFont2.setMatrix(projection * translate(0, -200.0f, 0));
        GLFont2DirectDraw::draw(fontBuilder_40, GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);

        GLTexture::deactive(0);


    }


    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;


App *app = NULL;

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

#if NDEBUG && false
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

    app = new App(&window);

    //
    // Main Loop
    //

    app->time.update();
    while (window.isOpen()) {

        app->draw();

        // Swap Buffers
        window.display();

        //avoid linux 100% CPU usage
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

    // Adjust the viewport when the window is resized
    if (event.type == sf::Event::Resized) {
        app->resize();
        //glViewport(0, 0, event.size.width, event.size.height);
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

