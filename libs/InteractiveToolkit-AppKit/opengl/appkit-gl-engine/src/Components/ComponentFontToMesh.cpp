#include <appkit-gl-engine/Components/ComponentFontToMesh.h>

#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentFontToMesh::Type = "ComponentFontToMesh";

            void ComponentFontToMesh::toMesh(ResourceMap *resourceMap, AppKit::OpenGL::GLFont2Builder &builder, bool dynamic)
            {
                ITK_ABORT(getTransformCount() == 0, "Transform cannot be 0.\n");

                auto transform = getTransform();

                start();

                mesh->pos.clear();
                mesh->uv[0].clear();
                mesh->color[0].clear();
                mesh->indices.clear();

                bool is_polygon_font = builder.isConstructedFromPolygonCache();

                if (is_polygon_font)
                {
                    //material->type = AppKit::GLEngine::Components::MaterialUnlitVertexColor;
                    material->setShader(resourceMap->shaderUnlitVertexColor);
                    //material->unlit.tex = nullptr;

                    for (size_t i = 0; i < builder.vertexAttrib.size(); i++)
                    {
                        mesh->pos.push_back(builder.vertexAttrib[i].pos);
                        mesh->color[0].push_back(builder.vertexAttrib[i].color);
                    }

                    mesh->indices.insert(mesh->indices.end(),
                                         builder.triangles.begin(),
                                         builder.triangles.end());
                }
                else
                {
                    // texture font
                    //material->type = AppKit::GLEngine::Components::MaterialUnlitTextureVertexColorFont;
                    material->setShader(resourceMap->shaderUnlitTextureVertexColorAlpha);
                    //material->unlit.tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(&builder.glFont2.texture, [](AppKit::OpenGL::GLTexture *v) {});
                    auto tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(&builder.glFont2.texture, [](AppKit::OpenGL::GLTexture *v) {});
                    material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)tex);

                    for (size_t i = 0; i < builder.vertexAttrib.size(); i++)
                    {
                        mesh->pos.push_back(builder.vertexAttrib[i].pos);
                        mesh->uv[0].push_back(MathCore::vec3f(builder.vertexAttrib[i].uv, 0.0f));
                        mesh->color[0].push_back(builder.vertexAttrib[i].color);

                        // keep CCW orientation
                        if ((i % 3) == 0)
                        {
                            mesh->indices.push_back((uint16_t)i);
                            mesh->indices.push_back((uint16_t)(i + 1));
                            mesh->indices.push_back((uint16_t)(i + 2));
                        }
                    }
                }

                mesh->ComputeFormat(false);

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
                // meshWrapper->debugCollisionShapes = true;
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
                    // material->type = AppKit::GLEngine::Components::MaterialNone;
                    // material->unlit.color = MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
                    // material->unlit.blendMode = AppKit::GLEngine::BlendModeAlpha;
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

            void ComponentFontToMesh::detachFromTransform(std::shared_ptr<Transform> t)
            {
            }

            void ComponentFontToMesh::start()
            {
                createAuxiliaryComponents();
            }

            std::shared_ptr<Component> ComponentFontToMesh::duplicate_ref_or_clone(bool force_clone)
            {
                auto result = Component::CreateShared<ComponentFontToMesh>();

                result->material = this->material;
                result->mesh = this->mesh;

                return result;
            }
            void ComponentFontToMesh::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                auto found_material = componentMap.find(this->material);
                if (found_material != componentMap.end())
                    this->material = std::dynamic_pointer_cast<ComponentMaterial>(found_material->second);

                auto found_mesh = componentMap.find(this->mesh);
                if (found_mesh != componentMap.end())
                    this->mesh = std::dynamic_pointer_cast<ComponentMesh>(found_mesh->second);
            }

            void ComponentFontToMesh::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentFontToMesh::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
            }
            void ComponentFontToMesh::Deserialize(rapidjson::Value &_value,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                  ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentFontToMesh::Type) == 0)
                    return;
            }

        }
    }
}