// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include <appkit-gl-engine/components/2d/ComponentSprite.h>

#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

// #include "../shaders/SpriteShaderWithMask.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentSprite::Type = "ComponentSprite";

            void ComponentSprite::checkOrCreateAuxiliaryComponents(
                AppKit::GLEngine::ResourceMap *resourceMap,
                std::shared_ptr<AppKit::OpenGL::GLTexture> texture)
            {
                if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
                    return;
                auto transform = getTransform();

                if (material == nullptr)
                {
                    last_texture = texture;
                    uint64_t spriteMaterialId = (uint64_t)texture.get();
                    if (resourceMap->spriteMaterialMap.find(spriteMaterialId) != resourceMap->spriteMaterialMap.end())
                    {
                        material = transform->addComponent(resourceMap->spriteMaterialMap[spriteMaterialId]);
                    }
                    else
                    {
                        material = transform->addNewComponent<ComponentMaterial>();
                        material->setShader(resourceMap->spriteShader);
                        material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)texture);
                        resourceMap->spriteMaterialMap[spriteMaterialId] = material;
                    }
                }

                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;
                    mesh->always_clone = true;
                }

                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
                    // meshWrapper->always_clone = true;
                }
            }

            void ComponentSprite::setTexture(
                AppKit::GLEngine::ResourceMap *resourceMap,
                std::shared_ptr<AppKit::OpenGL::GLTexture> texture,
                const MathCore::vec2f &pivot,
                const MathCore::vec4f &color,
                const MathCore::vec2f &size_constraint,
                MeshUploadMode meshUploadMode)
            {
                // directTexture.texture = texture;
                // directTexture.pivot = pivot;
                // directTexture.color = color;
                // type = SpriteSourceDirectTexture;
                checkOrCreateAuxiliaryComponents(resourceMap, texture);

                bool onCloneNoModify =
                    meshUploadMode == MeshUploadMode_Direct_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Static_OnClone_NoModify;

                mesh->always_clone = !onCloneNoModify;
                this->always_clone = !onCloneNoModify;

                MathCore::vec3f size((float)texture->width, (float)texture->height, 0.0f);
                if (size_constraint.x > 0.0f && size_constraint.y > 0.0f)
                    size = MathCore::vec3f(size_constraint.x, size_constraint.y, 0.0f);
                else if (size_constraint.x > 0.0f)
                    size = MathCore::vec3f(size_constraint.x, (size.y * size_constraint.x) / size.x, 0.0f);
                else if (size_constraint.y > 0.0f)
                    size = MathCore::vec3f((size.x * size_constraint.y) / size.y, size_constraint.y, 0.0f);

                float xmin = -pivot.x;
                float xmax = 1.0f - pivot.x;

                float ymin = -pivot.y;
                float ymax = 1.0f - pivot.y;

                mesh->pos.clear();
                mesh->pos.push_back(size * MathCore::vec3f(xmax, ymax, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmax, ymin, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmin, ymin, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmin, ymax, 0.0f));

                mesh->uv[0].clear();
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(1.0f, 0.0f), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(1.0f, 1.0f), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(0.0f, 1.0f), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(0.0f, 0.0f), 0));

                mesh->color[0].clear();
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);

                // mesh->normals.clear();
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));

                // mesh->tangent.clear();
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));

                // mesh->binormal.clear();
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));

                mesh->indices.clear();
                mesh->indices.push_back(0);
                mesh->indices.push_back(2);
                mesh->indices.push_back(1);

                mesh->indices.push_back(0);
                mesh->indices.push_back(3);
                mesh->indices.push_back(2);

                if (meshUploadMode == MeshUploadMode_Static || meshUploadMode == MeshUploadMode_Static_OnClone_NoModify)
                    mesh->syncVBO(0, 0xffffffff);
                else if (meshUploadMode == MeshUploadMode_Dynamic || meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify)
                    mesh->syncVBO(0xffffffff, 0);

                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        mesh->pos[0],
                        mesh->pos[2]),
                    true);
            }

            void ComponentSprite::setTextureFromAtlas(
                AppKit::GLEngine::ResourceMap *resourceMap,
                std::shared_ptr<SpriteAtlas> atlas,
                const std::string &name,
                const MathCore::vec2f &pivot,
                const MathCore::vec4f &color,
                const MathCore::vec2f &size_constraint,
                MeshUploadMode meshUploadMode)
            {
                checkOrCreateAuxiliaryComponents(resourceMap, atlas->texture);

                bool onCloneNoModify =
                    meshUploadMode == MeshUploadMode_Direct_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Static_OnClone_NoModify;

                mesh->always_clone = !onCloneNoModify;
                this->always_clone = !onCloneNoModify;

                auto entry = atlas->getSprite(name);

                MathCore::vec3f size((float)entry.spriteSize.width, (float)entry.spriteSize.height, 0.0f);
                if (size_constraint.x > 0.0f && size_constraint.y > 0.0f)
                    size = MathCore::vec3f(size_constraint.x, size_constraint.y, 0.0f);
                else if (size_constraint.x > 0.0f)
                    size = MathCore::vec3f(size_constraint.x, (size.y * size_constraint.x) / size.x, 0.0f);
                else if (size_constraint.y > 0.0f)
                    size = MathCore::vec3f((size.x * size_constraint.y) / size.y, size_constraint.y, 0.0f);

                float xmin = -pivot.x;
                float xmax = 1.0f - pivot.x;

                float ymin = -pivot.y;
                float ymax = 1.0f - pivot.y;

                mesh->pos.clear();
                mesh->pos.push_back(size * MathCore::vec3f(xmax, ymax, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmax, ymin, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmin, ymin, 0.0f));
                mesh->pos.push_back(size * MathCore::vec3f(xmin, ymax, 0.0f));

                mesh->uv[0].clear();
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMax.x, entry.uvMin.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMax.x, entry.uvMax.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMin.x, entry.uvMax.y), 0));
                mesh->uv[0].push_back(MathCore::vec3f(MathCore::vec2f(entry.uvMin.x, entry.uvMin.y), 0));

                mesh->color[0].clear();
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);
                mesh->color[0].push_back(color);

                // mesh->normals.clear();
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));
                // mesh->normals.push_back(MathCore::vec3f(0.0f, 1.0f, 0.0f));

                // mesh->tangent.clear();
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));
                // mesh->tangent.push_back(MathCore::vec3f(1.0f, 0.0f, 0.0f));

                // mesh->binormal.clear();
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));
                // mesh->binormal.push_back(MathCore::vec3f(0.0f, 0.0f, 1.0f));

                mesh->indices.clear();
                mesh->indices.push_back(0);
                mesh->indices.push_back(2);
                mesh->indices.push_back(1);

                mesh->indices.push_back(0);
                mesh->indices.push_back(3);
                mesh->indices.push_back(2);

                if (meshUploadMode == MeshUploadMode_Static || meshUploadMode == MeshUploadMode_Static_OnClone_NoModify)
                    mesh->syncVBO(0, 0xffffffff);
                else if (meshUploadMode == MeshUploadMode_Dynamic || meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify)
                    mesh->syncVBO(0xffffffff, 0);

                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        mesh->pos[0],
                        mesh->pos[2]),
                    true);
            }

            void ComponentSprite::setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                                          std::shared_ptr<ComponentCamera> &camera,
                                          std::shared_ptr<ComponentRectangle> &mask)
            {
                this->mask = mask;

                auto transform = getTransform();
                if (mask == nullptr)
                {
                    if (last_texture != nullptr)
                    {
                        uint64_t spriteMaterialId = (uint64_t)last_texture.get();
                        if (resourceMap->spriteMaterialMap.find(spriteMaterialId) != resourceMap->spriteMaterialMap.end())
                        {
                            material = transform->replaceComponent<ComponentMaterial>(material, resourceMap->spriteMaterialMap[spriteMaterialId]);
                        }
                        else
                        {
                            material = transform->replaceComponent<ComponentMaterial>(material, Component::CreateShared<ComponentMaterial>());
                            material->setShader(resourceMap->spriteShader);
                            material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)last_texture);
                            resourceMap->spriteMaterialMap[spriteMaterialId] = material;
                        }
                    }
                }
                else
                {
                    auto new_material = Component::CreateShared<Components::ComponentMaterial>();
                    new_material->always_clone = true;
                    new_material->setShader(resourceMap->spriteShaderWithMask);
                    new_material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)last_texture);
                    new_material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    new_material->property_bag.getProperty("ComponentCamera").set<std::weak_ptr<Component>>(camera);
                    material = transform->replaceComponent<ComponentMaterial>(material, new_material);

                    mesh->always_clone = true;
                    this->always_clone = true;
                }
            }

            ComponentSprite::ComponentSprite() : Component(ComponentSprite::Type)
            {
                // type = SpriteSourceNone;
                // directTexture.color = MathCore::vec4f(1.0f);
                // directTexture.pivot = MathCore::vec2f(0.5f, 0.5f);
                // directTexture.texture = nullptr;

                // textureFromAtlas.color = MathCore::vec4f(1.0f);
                // textureFromAtlas.entry = SpriteAtlas::Entry();
                // textureFromAtlas.texture = nullptr;

                always_clone = true;
            }

            ComponentSprite::~ComponentSprite()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentSprite::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentSprite>();

                // result->type = this->type;
                // result->directTexture = this->directTexture;
                // result->textureFromAtlas = this->textureFromAtlas;
                result->always_clone = this->always_clone;

                result->material = this->material;
                result->mesh = this->mesh;
                result->meshWrapper = this->meshWrapper;

                result->mask = this->mask;

                result->last_texture = this->last_texture;

                //if (this->mask)
                //{
                //    printf("props before clone:\n");
                //    for (const auto &entry : material->property_bag.getProperties())
                //        printf("    \"%s\" = %s\n", entry.first.c_str(), entry.second.toString().c_str());
                //}

                return result;
            }
            void ComponentSprite::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                if (componentMap.find(material) != componentMap.end())
                    material = std::dynamic_pointer_cast<ComponentMaterial>(componentMap[material]);
                if (componentMap.find(mesh) != componentMap.end())
                    mesh = std::dynamic_pointer_cast<ComponentMesh>(componentMap[mesh]);
                if (componentMap.find(meshWrapper) != componentMap.end())
                    meshWrapper = std::dynamic_pointer_cast<ComponentMeshWrapper>(componentMap[meshWrapper]);
                if (componentMap.find(mask) != componentMap.end())
                {
                    mask = std::dynamic_pointer_cast<ComponentRectangle>(componentMap[mask]);
                    //material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    //printf("props after clone:\n");
                    //for (const auto &entry : material->property_bag.getProperties())
                    //    printf("    \"%s\" = %s\n", entry.first.c_str(), entry.second.toString().c_str());
                }
            }

            void ComponentSprite::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentSprite::Deserialize(rapidjson::Value &_value,
                                              std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                              std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                              ResourceSet &resourceSet)
            {
            }

        }
    }
}
