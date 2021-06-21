#include "ComponentFontToMesh.h"

namespace GLEngine {

    namespace Components {

        const ComponentType ComponentFontToMesh::Type = "ComponentFontToMesh";

        void ComponentFontToMesh::toMesh(openglWrapper::GLFont2Builder &builder, bool dynamic){
            ARIBEIRO_ABORT(transform.size() == 0,"Transform cannot be 0.\n");
            start();

            mesh->pos.clear();
            mesh->uv[0].clear();
            mesh->color[0].clear();
            mesh->indices.clear();

            ReferenceCounter<openglWrapper::GLTexture*> *refCounter = &Engine::Instance()->textureReferenceCounter;
            
            refCounter->remove(material->unlit.tex);
            material->unlit.tex = refCounter->add( &builder.glFont2.texture );
            
            for(size_t i=0;i<builder.vertexAttrib.size();i++){
                mesh->pos.push_back( builder.vertexAttrib[i].pos );
                mesh->uv[0].push_back( aRibeiro::vec3(builder.vertexAttrib[i].uv,0.0f) );
                //mesh->color[0].push_back( vecToColor( builder.vertexAttrib[i].color ) );
                mesh->color[0].push_back( builder.vertexAttrib[i].color );
                
                //flip to CW orientation
                if ((i%3)==0) {
                    mesh->indices.push_back((uint16_t)i);
                    mesh->indices.push_back((uint16_t)(i+2));
                    mesh->indices.push_back((uint16_t)(i+1));
                }
            }

            mesh->ComputeFormat();

            if (dynamic)
                mesh->syncVBODynamic();
            else
                mesh->syncVBOStatic();
            
        }

        ComponentFontToMesh::ComponentFontToMesh():Component(ComponentFontToMesh::Type) {
            
        }
        
        ComponentFontToMesh::~ComponentFontToMesh() {
        }

        void ComponentFontToMesh::createAuxiliaryComponents() {
            if (material == NULL){
                material = (ComponentMaterial*)transform[0]->addComponent(new ComponentMaterial());
                material->type = GLEngine::Components::MaterialUnlitTextureVertexColorFont;
                material->unlit.color = aRibeiro::vec4(1.0f,1.0f,1.0f,1.0f);
                material->unlit.blendMode = GLEngine::BlendModeAlpha;
            }
            if (mesh == NULL){
                mesh = (ComponentMesh*)transform[0]->addComponent(new ComponentMesh());
            }
        }
        
        void ComponentFontToMesh::attachToTransform(Transform *t) {
            createAuxiliaryComponents();
        }

        void ComponentFontToMesh::start(){
            createAuxiliaryComponents();
        }
        
    }
}