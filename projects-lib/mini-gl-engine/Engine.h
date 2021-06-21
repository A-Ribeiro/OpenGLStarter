#ifndef gl_engine_h__
#define gl_engine_h__

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#pragma warning(disable:4305)
#endif

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <mini-gl-engine/AppBase.h>

#include <mini-gl-engine/ReferenceCounter.h>

#ifdef _MSC_VER
#    ifdef NDEBUG
#        pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#    else
#        pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#    endif
#endif

namespace GLEngine {

    DefineMethodPointer( CreateAppInstanceMethodPtr,AppBase * ) ReturnMethodCall ();

    struct SetVideoMode {
        sf::VideoMode videoMode;
        bool fullscreen;
        int aa_level;
        bool sRGB;
        bool should_set_resolution;
    };

    class Engine {

        SetVideoMode setVideoMode;

        CreateAppInstanceMethodPtr OnCreateInstance;
    public:
        std::string companyName;
        std::string gameName;
        sf::RenderWindow *window;
        AppBase *app;
        
        ReferenceCounter<openglWrapper::GLTexture*> textureReferenceCounter;
        ReferenceCounter<GLEngine::Component*> componentReferenceCounter;
        
        //after initialization this variable will hold the sRGB information about the buffer
        bool sRGBCapable;

        bool isNVidiaCard;
        bool isAMDCard;
        bool isIntelCard;
        
        static Engine *Instance();
        Engine();
        void initialSetup(const std::string &_companyName, const std::string &_gameName, const CreateAppInstanceMethodPtr& _OnCreateInstance);
        const std::vector<sf::VideoMode> &getResolutionList() const;
        void setResolution(const sf::VideoMode &videoMode, bool fullscreen, bool sRGB, int aa_level = 0 );
        void clear();
        void mainLoop();
        void onSingleWindowEvent(sf::Event &event);
    };

}

#endif
