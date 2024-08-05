#include <appkit-gl-engine/Components/ComponentFontToMesh.h>

#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentFontToMesh::Type = "ComponentFontToMesh";

            void ComponentFontToMesh::toMesh(AppKit::OpenGL::GLFont2Builder &builder, bool dynamic)
            {
                ITK_ABORT(getTransformCount() == 0, "Transform cannot be 0.\n");
                
                auto transform = getTransform();

                start();

                mesh->pos.clear();
                mesh->uv[0].clear();
                mesh->color[0].clear();
                mesh->indices.clear();

                //ReferenceCounter<AppKit::OpenGL::GLTexture *> *refCounter = &Engine::Instance()->textureReferenceCounter;

                material->type = AppKit::GLEngine::Components::MaterialUnlitTextureVertexColorFont;
                //refCounter->remove(material->unlit.tex);
                material->unlit.tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(&builder.glFont2.texture, [](AppKit::OpenGL::GLTexture *v) { } );

                for (size_t i = 0; i < builder.vertexAttrib.size(); i++)
                {
                    mesh->pos.push_back(builder.vertexAttrib[i].pos);
                    mesh->uv[0].push_back(MathCore::vec3f(builder.vertexAttrib[i].uv, 0.0f));
                    // mesh->color[0].push_back( vecToColor( builder.vertexAttrib[i].color ) );
                    mesh->color[0].push_back(builder.vertexAttrib[i].color);

                    // flip to CW orientation
                    if ((i % 3) == 0)
                    {
                        mesh->indices.push_back((uint16_t)i);
                        mesh->indices.push_back((uint16_t)(i + 2));
                        mesh->indices.push_back((uint16_t)(i + 1));
                    }
                }

                mesh->ComputeFormat();

                if (dynamic)
                    mesh->syncVBODynamic();
                else
                    mesh->syncVBOStatic();

                // mesh wrapper logic...
                auto meshWrapper = transform->findComponent<Components::ComponentMeshWrapper>();
                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<Components::ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                }
                // meshWrapper->updateMeshOBB();
                meshWrapper->updateMeshAABB();
                // meshWrapper->updateMeshSphere();
                // meshWrapper->computeFinalPositions(false);
                meshWrapper->debugCollisionShapes = true;
            }

            ComponentFontToMesh::ComponentFontToMesh() : Component(ComponentFontToMesh::Type)
            {
            }

            ComponentFontToMesh::~ComponentFontToMesh()
            {
            }

            void ComponentFontToMesh::createAuxiliaryComponents()
            {
                auto transform = getTransform();
                if (material == nullptr)
                {
                    material = transform->addNewComponent<ComponentMaterial>();
                    // material->type = AppKit::GLEngine::Components::MaterialUnlitTextureVertexColorFont;
                    material->type = AppKit::GLEngine::Components::MaterialNone;
                    material->unlit.color = MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
                    material->unlit.blendMode = AppKit::GLEngine::BlendModeAlpha;
                }
                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->always_clone = true;
                }
            }

            void ComponentFontToMesh::attachToTransform(std::shared_ptr<Transform> t)
            {
                createAuxiliaryComponents();
            }

            void ComponentFontToMesh::detachFromTransform(std::shared_ptr<Transform> t) {

            }

            void ComponentFontToMesh::start()
            {
                createAuxiliaryComponents();
            }

            std::shared_ptr<Component> ComponentFontToMesh::duplicate_ref_or_clone(bool force_clone){
                auto result = Component::CreateShared<ComponentFontToMesh>();

                result->material = this->material;
                result->mesh = this->mesh;

                return result;
            }
            void ComponentFontToMesh::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){
                auto found_material = componentMap.find(this->material);
                if (found_material != componentMap.end())
                    this->material = std::dynamic_pointer_cast<ComponentMaterial>(found_material->second);
                
                auto found_mesh = componentMap.find(this->mesh);
                if (found_mesh != componentMap.end())
                    this->mesh = std::dynamic_pointer_cast<ComponentMesh>(found_mesh->second);
            }


        }
    }
}