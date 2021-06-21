#include "Engine.h"
#include <mini-gl-engine/GLRenderState.h>

#include <glew/glew.h> // extensions here

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

namespace GLEngine {

    Engine *Engine::Instance() {
        static Engine result;
        return &result;
    }

    Engine::Engine() {
        OnCreateInstance = NULL;
        app = NULL;
        window = NULL;
        setVideoMode.should_set_resolution = false;
    }

    void Engine::initialSetup(const std::string &_companyName, const std::string &_gameName, const CreateAppInstanceMethodPtr& _OnCreateInstance){
        OnCreateInstance = _OnCreateInstance;
        app = NULL;
        window = NULL;

        companyName = _companyName;
        gameName = _gameName;
    }

    const std::vector<sf::VideoMode> &Engine::getResolutionList() const {
        return sf::VideoMode::getFullscreenModes();
    }

    void Engine::setResolution(const sf::VideoMode &videoMode, bool fullscreen, bool sRGB, int aa_level ) {

        if (app != NULL) {

            setVideoMode.videoMode = videoMode;
            setVideoMode.fullscreen = fullscreen;
            setVideoMode.aa_level = aa_level;
            setVideoMode.sRGB = sRGB;
            setVideoMode.should_set_resolution = true;

            return;
        }


        //clear();

        sf::ContextSettings contextSettings;
        contextSettings.depthBits = 24;
        contextSettings.sRgbCapable = sRGB;
        contextSettings.antialiasingLevel = aa_level;

        // OpenGL 2.0 context
        //contextSettings.majorVersion = 2;
        //contextSettings.majorVersion = 1;
        contextSettings.majorVersion = 3;
        contextSettings.majorVersion = 2;

        window = new sf::RenderWindow(videoMode,gameName.c_str(), (fullscreen)? sf::Style::Fullscreen : sf::Style::Default, contextSettings);

        contextSettings = window->getSettings();

        sRGBCapable = contextSettings.sRgbCapable;

        window->setVerticalSyncEnabled(true);
        window->setFramerateLimit(0);
        window->setMouseCursorVisible(true);

#ifdef _WIN32
        SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
        //SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

        //
        // Check hardware capabilities
        //
        glewInit();

        GLRenderState::Instance()->initialize(window->getSize().x,window->getSize().y);


        std::string vendor = aRibeiro::StringUtil::toLower( std::string((const char*)glGetString(GL_VENDOR)) );
        //std::string renderer = std::string((const char*)glGetString(GL_RENDERER));
printf("Card: %s\n",vendor.c_str());
        isNVidiaCard = aRibeiro::StringUtil::contains( vendor, "nvidia" );
        isAMDCard = aRibeiro::StringUtil::contains( vendor, "amd" ) || aRibeiro::StringUtil::contains( vendor, "radeon" );
        isIntelCard = aRibeiro::StringUtil::contains( vendor, "intel" );

        app = OnCreateInstance();

    }

    void Engine::clear() {
        GLRenderState::Instance()->releaseResources();
        openglWrapper::GLFont::releaseSharedResources();

        //cannot setnull and delete app... some components need app access when deleting
        //aRibeiro::setNullAndDelete(app);
        delete app;
        app = NULL;

        if (window != NULL)
            window->close();
        aRibeiro::setNullAndDelete(window);
    }

    void Engine::mainLoop() {
        aRibeiro::PressReleaseDetector focusChange;

        while (window != NULL && window->isOpen()) {

            focusChange.setState(window->hasFocus());

            if (focusChange.down) {
                app->OnGainFocus();
            } else if (focusChange.up) {
                app->OnLostFocus();
            }

            if (focusChange.pressed){
                app->draw();
                window->display();//swap buffers

                //force terminate all GL commands after swap buffers...
                // reduces a bit of stuttering
                if (wglGetCurrentContext() != NULL)
                    glFinish();

            }

            //On windows OS, this sleep millist causes stuttering on rendering...
#if !defined( OS_TARGET_win )
            //avoid linux 100% CPU usage
            //aRibeiro::PlatformSleep::sleepMillis(1);
#endif
            
            sf::Event event;
            while (window != NULL && window->pollEvent(event)) {
                if (focusChange.pressed || event.type == sf::Event::Closed || event.type == sf::Event::Resized)
                    onSingleWindowEvent(event);
            }

            if (app != NULL && app->canExitApplication)
                clear();

            if (setVideoMode.should_set_resolution) {
                setVideoMode.should_set_resolution = false;
                clear();
                setResolution(setVideoMode.videoMode, setVideoMode.fullscreen, setVideoMode.sRGB, setVideoMode.aa_level);
            }
        }
    }

    void Engine::onSingleWindowEvent(sf::Event &event) {
        switch(event.type) {
            case sf::Event::Closed:// Close window : exit
                clear();
                break;
            case sf::Event::KeyPressed:
                app->OnKeyDown(event.key.code);
                break;
            case sf::Event::KeyReleased:
                app->OnKeyUp(event.key.code);
                break;
            case sf::Event::Resized:
                app->WindowSize = sf::Vector2i(event.size.width, event.size.height);
                break;
            //case sf::Event::JoystickButtonPressed:
            case sf::Event::MouseButtonPressed:
                app->OnMouseDown(event.mouseButton.button, aRibeiro::vec2((int)event.mouseButton.x, (int)window->getSize().y - 1 - (int)event.mouseButton.y));
                break;
            case sf::Event::MouseButtonReleased:
                app->OnMouseUp(event.mouseButton.button, aRibeiro::vec2((int)event.mouseButton.x, (int)window->getSize().y - 1 - (int)event.mouseButton.y));
                break;
            case sf::Event::MouseMoved:
            {
                aRibeiro::vec2 _2dCoord = aRibeiro::vec2((int)event.mouseMove.x, (int)window->getSize().y - 1 - (int)event.mouseMove.y);
                app->MousePos = _2dCoord;
                _2dCoord -= app->MousePosCenter;
                app->MousePosRelatedToCenter = _2dCoord;

                if (app->screenCenterWindowSpace.x > app->screenCenterWindowSpace.y)
                    _2dCoord /= (float)app->screenCenterWindowSpace.y;
                else
                    _2dCoord /= (float)app->screenCenterWindowSpace.x;

                app->MousePosRelatedToCenterNormalized = _2dCoord;

                //printf("%f %f\n",app->MousePosCenter.x,app->MousePosCenter.y);
                //printf("%f %f\n",app->MousePosRelatedToCenter.value.x,app->MousePosRelatedToCenter.value.y);
                //printf("%f %f\n",app->MousePosRelatedToCenterNormalized.value.x,app->MousePosRelatedToCenterNormalized.value.y);
            }
                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.delta > 0.9)
                    app->OnMouseWheelUp();
                else if (event.mouseWheelScroll.delta < -0.9)
                    app->OnMouseWheelDown();
                break;
            default:
                break;
        }
    }

}
