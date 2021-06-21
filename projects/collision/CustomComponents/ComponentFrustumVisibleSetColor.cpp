#include "ComponentFrustumVisibleSetColor.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

const GLEngine::ComponentType ComponentFrustumVisibleSetColor::Type = "ComponentFrustumVisibleSetColor";

ComponentFrustumVisibleSetColor::ComponentFrustumVisibleSetColor():
    Component(ComponentFrustumVisibleSetColor::Type) {
}

ComponentFrustumVisibleSetColor::~ComponentFrustumVisibleSetColor() {
    if (frustumCulling != NULL) {
        frustumCulling->OnBecomeVisible.remove(this, &ComponentFrustumVisibleSetColor::OnBecomeVisible);
        frustumCulling->OnBecomeInvisible.remove(this, &ComponentFrustumVisibleSetColor::OnBecomeInvisible);
    }
}

void ComponentFrustumVisibleSetColor::start() {
    colorMesh = (ComponentColorMesh *)transform[0]->findComponent(ComponentColorMesh::Type);
    frustumCulling = (ComponentFrustumCulling *)transform[0]->findComponent(ComponentFrustumCulling::Type);

    frustumCulling->OnBecomeVisible.add(this, &ComponentFrustumVisibleSetColor::OnBecomeVisible);
    frustumCulling->OnBecomeInvisible.add(this, &ComponentFrustumVisibleSetColor::OnBecomeInvisible);
}

void ComponentFrustumVisibleSetColor::OnBecomeVisible(Component *f, Component *camera) {
    colorMesh->color = vec4(0.5f, 0.5f, 0.0f, 1.0f);
}

void ComponentFrustumVisibleSetColor::OnBecomeInvisible(Component *f, Component *camera) {
    colorMesh->color = vec4(0.5f, 0.0f, 0.0f, 1.0f);
}
