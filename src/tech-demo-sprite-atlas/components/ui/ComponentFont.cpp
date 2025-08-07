// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentFont.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentFont::Type = "ComponentFont";

            void ComponentFont::checkOrCreateAuxiliaryComponents(
                AppKit::GLEngine::ResourceMap *resourceMap,
                std::shared_ptr<ComponentMaterial> &materialp)
            {

                if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
                {
                    if (material != materialp)
                        material = getTransform()->replaceComponent<ComponentMaterial>(material, materialp);
                    return;
                }

                auto transform = getTransform();

                if (material == nullptr)
                {
                    material = transform->addComponent<ComponentMaterial>(materialp);
                }

                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;
                }

                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
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
                char32_t wordSeparatorChar)
            {
                std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> fontResource;

                if (polygon_size > 0.0f)
                    fontResource = resourceMap->getPolygonFont(font_path, polygon_size, polygon_distance_tolerance, polygon_threadPool, is_srgb);
                else
                    fontResource = resourceMap->getTextureFont(font_path, is_srgb);

                auto builder = fontResource->fontBuilder.get();

                checkOrCreateAuxiliaryComponents(resourceMap, fontResource->material);

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

                meshWrapper->setShapeAABB(CollisionCore::AABB<MathCore::vec3f>(min, max), true);
            }

            ComponentFont::ComponentFont() : Component(ComponentFont::Type)
            {
                always_clone = false;
            }

            ComponentFont::~ComponentFont()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentFont::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentFont>();

                result->always_clone = this->always_clone;

                return result;
            }
            void ComponentFont::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
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
