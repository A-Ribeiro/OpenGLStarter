#ifndef ComponentFrustumVisibleSetColor__H
#define ComponentFrustumVisibleSetColor__H

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentColorMesh.h>
#include <mini-gl-engine/ComponentFrustumCulling.h>

class ComponentFrustumVisibleSetColor : public GLEngine::Component {
public:
    
    static const GLEngine::ComponentType Type;

    GLEngine::SharedPointer<GLEngine::Components::ComponentColorMesh> colorMesh;
    GLEngine::SharedPointer<GLEngine::Components::ComponentFrustumCulling> frustumCulling;

    ComponentFrustumVisibleSetColor();
    ~ComponentFrustumVisibleSetColor();
    void start();

    void OnBecomeVisible(Component *f, Component *camera);
    void OnBecomeInvisible(Component *f, Component *camera);

};


#endif
