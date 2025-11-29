// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include <appkit-gl-engine/Components/2d/ComponentFont.h>
#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

// #include "../../shaders/ShaderUnlitTextureVertexColorAlphaWithMask.h"
// #include "../../shaders/ShaderUnlitVertexColorWithMask.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentFont::Type = "ComponentFont";

            void ComponentFont::checkOrCreateAuxiliaryComponents(
                AppKit::GLEngine::ResourceMap *resourceMap)
            {

                if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
                {
                    // if (material != materialp)
                    //     material = getTransform()->replaceComponent<ComponentMaterial>(material, materialp);
                    std::shared_ptr<Components::ComponentMaterial> target_material;

                    if (mask != nullptr && camera != nullptr)
                        target_material = resourceMap->mask_query_or_create_font(last_fontResource, camera, mask);
                    else
                        target_material = last_fontResource->material;

                    if (target_material != material)
                        material = getTransform()->replaceComponent<ComponentMaterial>(material, target_material);
                    return;
                }

                auto transform = getTransform();

                if (material == nullptr)
                {
                    // material = transform->addComponent<ComponentMaterial>(materialp);

                    if (mask != nullptr && camera != nullptr)
                        material = transform->addComponent(resourceMap->mask_query_or_create_font(last_fontResource, camera, mask));
                    else
                        material = transform->addComponent(last_fontResource->material);
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

            CollisionCore::AABB<MathCore::vec3f> ComponentFont::computeBox(
                AppKit::GLEngine::ResourceMap *resourceMap,
                const std::string &font_path,
                // 0 = texture, > 0 = polygon
                float polygon_size,
                float polygon_distance_tolerance,
                Platform::ThreadPool *polygon_threadPool,
                bool is_srgb,

                const std::string &text,
                float size, ///< current state of the font size
                float max_width,

                AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                float lineHeight,

                AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                char32_t wordSeparatorChar)
            {
                std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> fontResource;

                if (polygon_size > 0.0f)
                    fontResource = resourceMap->getPolygonFont(font_path, polygon_size, polygon_distance_tolerance, polygon_threadPool, is_srgb);
                else
                    fontResource = resourceMap->getTextureFont(font_path, is_srgb);

                last_fontResource = fontResource;

                auto builder = fontResource->fontBuilder.get();

                builder->size = size;
                // builder->faceColor = faceColor;
                // builder->strokeColor = strokeColor;
                // builder->strokeOffset = strokeOffset;
                builder->horizontalAlign = horizontalAlign;
                builder->verticalAlign = verticalAlign;
                builder->lineHeight = lineHeight;
                builder->wrapMode = wrapMode;
                builder->firstLineHeightMode = firstLineHeightMode;
                builder->wordSeparatorChar = wordSeparatorChar;

                // builder->drawFace = faceColor.a > 0.0f;
                // builder->drawStroke = strokeColor.a > 0.0f;

                return builder->richComputeBox(text.c_str(), max_width);
            }

            const std::string &ComponentFont::getText() const
            {
                return last_text;
            }

            const CollisionCore::AABB<MathCore::vec3f> &ComponentFont::currentBox() const
            {
                return last_box;
            }

            void ComponentFont::setColor(const MathCore::vec4f &faceColor,
                                         const MathCore::vec4f &strokeColor)
            {
                if (hasDrawFace && hasDrawStroke)
                {

                    bool face_stroke = true;
                    int count6 = 0;
                    for (auto &v : mesh->color[0])
                    {
                        v = (face_stroke) ? faceColor : strokeColor;
                        if (count6++ >= 6)
                        {
                            count6 = 0;
                            face_stroke = !face_stroke;
                        }
                    }
                }
                else if (hasDrawFace)
                {
                    for (auto &v : mesh->color[0])
                        v = faceColor;
                }
                else if (hasDrawStroke)
                {
                    for (auto &v : mesh->color[0])
                        v = strokeColor;
                }
            }

            void ComponentFont::setText(
                AppKit::GLEngine::ResourceMap *resourceMap,

                const std::string &font_path,

                // 0 = texture, > 0 = polygon
                float polygon_size,
                float polygon_distance_tolerance,
                Platform::ThreadPool *polygon_threadPool,

                bool is_srgb,

                const std::string &text,
                float size, ///< current state of the font size

                float max_width,

                // .a == 0 turn off the drawing
                const MathCore::vec4f &faceColor,   ///< current state of the face color
                const MathCore::vec4f &strokeColor, ///< current state of the stroke color

                const MathCore::vec3f &strokeOffset,
                AppKit::OpenGL::GLFont2HorizontalAlign horizontalAlign,
                AppKit::OpenGL::GLFont2VerticalAlign verticalAlign,
                float lineHeight,

                AppKit::OpenGL::GLFont2WrapMode wrapMode,
                AppKit::OpenGL::GLFont2FirstLineHeightMode firstLineHeightMode,
                char32_t wordSeparatorChar,
                MeshUploadMode meshUploadMode)
            {
                this->last_text = text;
                std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> fontResource;

                if (polygon_size > 0.0f)
                    fontResource = resourceMap->getPolygonFont(font_path, polygon_size, polygon_distance_tolerance, polygon_threadPool, is_srgb);
                else
                    fontResource = resourceMap->getTextureFont(font_path, is_srgb);

                last_fontResource = fontResource;

                auto builder = fontResource->fontBuilder.get();

                checkOrCreateAuxiliaryComponents(resourceMap);

                bool onCloneNoModify =
                    meshUploadMode == MeshUploadMode_Direct_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Static_OnClone_NoModify;

                mesh->always_clone = !onCloneNoModify;
                this->always_clone = !onCloneNoModify;

                builder->size = size;
                builder->faceColor = faceColor;
                builder->strokeColor = strokeColor;
                builder->strokeOffset = strokeOffset;
                builder->horizontalAlign = horizontalAlign;
                builder->verticalAlign = verticalAlign;
                builder->lineHeight = lineHeight;
                builder->wrapMode = wrapMode;
                builder->firstLineHeightMode = firstLineHeightMode;
                builder->wordSeparatorChar = wordSeparatorChar;

                builder->drawFace = faceColor.a > 0.0f;
                builder->drawStroke = strokeColor.a > 0.0f;

                builder->richBuild(text.c_str(), is_srgb, max_width, fontResource->polygonFontCache);

                auto transform = getTransform();

                mesh->pos.clear();
                mesh->uv[0].clear();
                mesh->color[0].clear();
                mesh->indices.clear();

                // material->setShader(resourceMap->shaderUnlitTextureVertexColorAlpha);
                // auto tex = std::shared_ptr<AppKit::OpenGL::GLTexture>(&builder->glFont2.texture, [](AppKit::OpenGL::GLTexture *v) {});
                // material->property_bag.getProperty("uTexture").set((std::shared_ptr<AppKit::OpenGL::VirtualTexture>)tex);

                MathCore::vec3f min, max;

                if (builder->isConstructedFromPolygonCache())
                {
                    hasDrawFace = true;
                    hasDrawStroke = false;

                    for (size_t i = 0; i < builder->vertexAttrib.size(); i++)
                    {
                        if (i == 0)
                            min = max = builder->vertexAttrib[i].pos;
                        else
                        {
                            min = MathCore::OP<MathCore::vec3f>::minimum(min, builder->vertexAttrib[i].pos);
                            max = MathCore::OP<MathCore::vec3f>::maximum(max, builder->vertexAttrib[i].pos);
                        }

                        mesh->pos.push_back(builder->vertexAttrib[i].pos);
                        mesh->color[0].push_back(builder->vertexAttrib[i].color);
                    }

                    mesh->indices.insert(mesh->indices.end(),
                                         builder->triangles.begin(),
                                         builder->triangles.end());
                }
                else
                {
                    hasDrawFace = builder->drawFace;
                    hasDrawStroke = builder->drawStroke;

                    for (size_t i = 0; i < builder->vertexAttrib.size(); i++)
                    {
                        if (i == 0)
                            min = max = builder->vertexAttrib[i].pos;
                        else
                        {
                            min = MathCore::OP<MathCore::vec3f>::minimum(min, builder->vertexAttrib[i].pos);
                            max = MathCore::OP<MathCore::vec3f>::maximum(max, builder->vertexAttrib[i].pos);
                        }

                        mesh->pos.push_back(builder->vertexAttrib[i].pos);
                        mesh->uv[0].push_back(MathCore::vec3f(builder->vertexAttrib[i].uv, 0.0f));
                        mesh->color[0].push_back(builder->vertexAttrib[i].color);

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

                // meshWrapper->updateMeshAABB();

                last_box = CollisionCore::AABB<MathCore::vec3f>(min, max);

                meshWrapper->setShapeAABB(last_box, true);
            }

            void ComponentFont::setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                                        std::shared_ptr<ComponentCamera> &camera,
                                        std::shared_ptr<ComponentRectangle> &mask)
            {
                this->camera = camera;
                this->mask = mask;

                if (last_fontResource == nullptr)
                    return;

                auto transform = getTransform();
                if (mask == nullptr)
                {
                    // material = transform->replaceComponent<ComponentMaterial>(material, last_fontResource->material);
                    if (last_fontResource != nullptr)
                        material = transform->replaceComponent<ComponentMaterial>(material, last_fontResource->material);
                }
                else
                {
                    if (last_fontResource != nullptr)
                        material = transform->replaceComponent<ComponentMaterial>(material, resourceMap->mask_query_or_create_font(last_fontResource, camera, mask));

                    // if (last_fontResource->polygonFontCache)
                    // {
                    //     auto new_material = Component::CreateShared<Components::ComponentMaterial>();
                    //     new_material->always_clone = true;
                    //     new_material->setShader(resourceMap->shaderUnlitVertexColorWithMask);
                    //     new_material->property_bag.getProperty("BlendMode").set<int>((int)AppKit::GLEngine::BlendModeAlpha);
                    //     new_material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    //     new_material->property_bag.getProperty("ComponentCamera").set<std::weak_ptr<Component>>(camera);
                    //     material = transform->replaceComponent<ComponentMaterial>(material, new_material);
                    // }
                    // else
                    // {
                    //     // auto new_material = std::dynamic_pointer_cast<ComponentMaterial>(last_fontResource->material_mask->duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, true));
                    //     auto new_material = Component::CreateShared<Components::ComponentMaterial>();
                    //     new_material->always_clone = true;
                    //     new_material->setShader(resourceMap->shaderUnlitTextureVertexColorAlphaWithMask);
                    //     auto tex = last_fontResource->material->property_bag.getProperty("uTexture").get<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>();
                    //     new_material->property_bag.getProperty("uTexture").set(tex);
                    //     new_material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    //     new_material->property_bag.getProperty("ComponentCamera").set<std::weak_ptr<Component>>(camera);
                    //     material = transform->replaceComponent<ComponentMaterial>(material, new_material);
                    // }
                }
            }

            ComponentFont::ComponentFont() : Component(ComponentFont::Type)
            {
                always_clone = true;
                hasDrawFace = false;
                hasDrawStroke = false;
            }

            ComponentFont::~ComponentFont()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentFont::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentFont>();

                result->always_clone = this->always_clone;

                result->material = this->material;
                result->mesh = this->mesh;
                result->meshWrapper = this->meshWrapper;

                result->camera = this->camera;
                result->mask = this->mask;

                result->last_fontResource = this->last_fontResource;

                return result;
            }
            void ComponentFont::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                if (componentMap.find(material) != componentMap.end())
                    material = std::dynamic_pointer_cast<ComponentMaterial>(componentMap[material]);
                if (componentMap.find(mesh) != componentMap.end())
                    mesh = std::dynamic_pointer_cast<ComponentMesh>(componentMap[mesh]);
                if (componentMap.find(meshWrapper) != componentMap.end())
                    meshWrapper = std::dynamic_pointer_cast<ComponentMeshWrapper>(componentMap[meshWrapper]);
                if (componentMap.find(camera) != componentMap.end())
                    camera = std::dynamic_pointer_cast<ComponentCamera>(componentMap[camera]);
                if (componentMap.find(mask) != componentMap.end())
                {
                    mask = std::dynamic_pointer_cast<ComponentRectangle>(componentMap[mask]);
                    // material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    if (camera != nullptr && last_fontResource != nullptr)
                    {
                        auto new_material = resourceMap->mask_query_or_create_font(last_fontResource, camera, mask);
                        // for (auto &entry : mTransform)
                        // {
                        //     auto transform = entry.weak_ptr.lock();
                        //     if (!transform)
                        //     {
                        //         printf("[ComponentSprite] error, clonning null transform list");
                        //         continue;
                        //     }
                        //     transform->replaceComponent(material, new_material);
                        // }
                        getTransform()->replaceComponent(material, new_material);
                        material = new_material;
                    }
                }
            }

            void ComponentFont::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentFont::Deserialize(rapidjson::Value &_value,
                                            std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                            std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                            ResourceSet &resourceSet)
            {
            }
        }
    }
}
