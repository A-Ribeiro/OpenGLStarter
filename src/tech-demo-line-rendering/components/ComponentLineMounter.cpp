// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentLineMounter.h"

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentLineMounter::Type = "ComponentLineMounter";

            void ComponentLineMounter::checkOrCreateAuxiliaryComponents()
            {
                auto transform = getTransform();
                if (material == nullptr)
                {
                    material = transform->addNewComponent<ComponentMaterial>();
                    // material->type = AppKit::GLEngine::Components::MaterialUnlitTextureVertexColorFont;
                    material->type = AppKit::GLEngine::Components::MaterialNone;
                }
                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->always_clone = true;
                }
                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
                }
                transform->makeLastComponent(self());
            }

            void ComponentLineMounter::setLineShader(std::shared_ptr<LineShader> lineShader)
            {
                checkOrCreateAuxiliaryComponents();

                this->lineShader = lineShader;
                if (lineShader == nullptr)
                {
                    material->type = AppKit::GLEngine::Components::MaterialNone;
                    return;
                }

                material->type = AppKit::GLEngine::Components::MaterialCustomShader;
                material->custom_shader = lineShader;
                material->custom_shader_property_bag = lineShader->createDefaultBag();
            }

            ComponentLineMounter::ComponentLineMounter() : Component(ComponentLineMounter::Type)
            {
                always_clone = false;
            }

            ComponentLineMounter::~ComponentLineMounter()
            {
            }

            void ComponentLineMounter::clear()
            {
                mesh->pos.clear();
                mesh->uv[1].clear();
                mesh->uv[2].clear();
                mesh->uv[3].clear();
                mesh->color[0].clear();

                mesh->indices.clear();

                aabb.makeEmpty();
                meshWrapper->clearShape();
            }

            void ComponentLineMounter::addLine(const MathCore::vec3f &a, const MathCore::vec3f &b,
                                               float thickness,
                                               const MathCore::vec4f &color)
            {

                aabb = AABBType::joinAABB( aabb, AABBType::fromLineSegment(a,b) );

                //aabb = AABBType::joinAABB( aabb, AABBType::fromSphere(a, thickness * 0.5) );
                //aabb = AABBType::joinAABB( aabb, AABBType::fromSphere(b, thickness * 0.5) );


                // "attribute vec2 aPosition;" // 2d square position
                // "attribute vec4 aUV1;"      // line point 1
                // "attribute vec4 aUV2;"      // line point 2
                // // x = current position lrp in line [0..1]
                // // y = line_thickness_px
                // "attribute vec2 aUV3;"
                // "attribute vec4 aColor0;"

                //
                //(-1,1)  (0,1)         (1,1) (2,1)
                //         *--------------*
                //         |              |
                //        A|              |B
                //         |              |
                //         *--------------*
                //(-1,-1) (0,-1)        (1,-1) (2,-1)
                uint32_t start_index;
                start_index = (uint32_t)mesh->pos.size();

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 1);
                mesh->indices.push_back(start_index + 2);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 2);
                mesh->indices.push_back(start_index + 3);


                // // a ear
                // //
                // //(-1,1)  (0,1)         (1,1) (2,1)
                // //         *--------------*
                // //         |              |
                // //        A|              |B
                // //         |              |
                // //         *--------------*
                // //(-1,-1) (0,-1)        (1,-1) (2,-1)
                start_index = (uint32_t)mesh->pos.size();

                mesh->pos.push_back(MathCore::vec3f(-1, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(-1, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 1);
                mesh->indices.push_back(start_index + 2);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 2);
                mesh->indices.push_back(start_index + 3);


                // b ear
                //
                //(-1,1)  (0,1)         (1,1) (2,1)
                //         *--------------*
                //         |              |
                //        A|              |B
                //         |              |
                //         *--------------*
                //(-1,-1) (0,-1)        (1,-1) (2,-1)
                start_index = (uint32_t)mesh->pos.size();

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(1, -1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(1, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                            // line point 1
                mesh->uv[2].push_back(b);                            // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 1);
                mesh->indices.push_back(start_index + 2);

                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 2);
                mesh->indices.push_back(start_index + 3);



                mesh->format = ITKExtension::Model::CONTAINS_POS |
                               ITKExtension::Model::CONTAINS_UV1 | ITKExtension::Model::CONTAINS_UV2 | ITKExtension::Model::CONTAINS_UV3 |
                               ITKExtension::Model::CONTAINS_COLOR0;

                meshWrapper->debugCollisionShapes = true;
                meshWrapper->setShapeAABB(aabb);
            }

        }
    }
}
