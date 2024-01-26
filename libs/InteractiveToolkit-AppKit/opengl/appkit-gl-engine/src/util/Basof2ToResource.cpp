#include <appkit-gl-engine/util/Basof2ToResource.h>

// #include <appkit-gl-engine/mini-gl-engine.h>
// #include <aRibeiroData/aRibeiroData.h>
// #include <aRibeiroCore/aRibeiroCore.h>

#include <appkit-gl-engine/util/ResourceHelper.h>

namespace AppKit
{
    namespace GLEngine
    {

        Transform *Basof2ToResource::nodeTraverse(const std::string &spaces,
                                                  int currentIndex,
                                                  const ITKExtension::Model::Node &node,
                                                  const ITKExtension::Model::ModelContainer *container,
                                                  std::map<int, Components::ComponentMesh *> &geometryCache,
                                                  std::map<int, Components::ComponentMaterial *> &materialCache,
                                                  Transform *root,
                                                  Components::ComponentMaterial *defaultMaterial,
                                                  uint32_t model_dynamic_upload, uint32_t model_static_upload)
        {

            Transform *result = root;

            if (result == NULL)
            {
                result = new Transform();

                // mat4 m = inv(transpose(node.transform));
                // mat4 m = transpose(node.transform);
                MathCore::mat4f m = node.transform;

                // m[2] = aRibeiro::toVec4( aRibeiro::cross( MathCore::CVT<MathCore::vec4f>::toVec3(m[1]), MathCore::CVT<MathCore::vec4f>::toVec3(m[0]) ) );

                /*
                result->LocalPosition = MathCore::CVT<MathCore::vec4f>::toVec3(m * vec4(0,0,0,1)) * MathCore::vec3f(1,1,-1);
                result->LocalRotation = extractQuat( m ) * MathCore::GEN<MathCore::quatf>::fromEuler(0, 0, MathCore::OP<float>::deg_2_rad(180.0));
                result->LocalScale = MathCore::vec3f(aRibeiro::length(m[0]),
                                                    aRibeiro::length(m[1]),
                                                    aRibeiro::length(m[2]));
                 */

                result->LocalPosition = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::vec4f(0, 0, 0, 1));

                result->LocalScale = MathCore::vec3f(MathCore::OP<MathCore::vec4f>::length(m[0]),
                                                     MathCore::OP<MathCore::vec4f>::length(m[1]),
                                                     MathCore::OP<MathCore::vec4f>::length(m[2]));

                m = MathCore::OP<MathCore::mat4f>::extractRotation(m);

                m[0] = MathCore::OP<MathCore::vec4f>::normalize(m[0]);
                m[1] = MathCore::OP<MathCore::vec4f>::normalize(m[1]);
                m[2] = MathCore::OP<MathCore::vec4f>::normalize(m[2]);

                result->LocalRotation = MathCore::GEN<MathCore::quatf>::fromMat4(m);

                result->Name = node.name;

                printf("%s(%s)\n", spaces.c_str(), node.name.c_str());
                printf("%sLocalPosition %f, %f, %f\n",
                       spaces.c_str(),
                       ((MathCore::vec3f)result->LocalPosition).x,
                       ((MathCore::vec3f)result->LocalPosition).y,
                       ((MathCore::vec3f)result->LocalPosition).z);

                printf("%sLocalScale %f, %f, %f\n",
                       spaces.c_str(),
                       ((MathCore::vec3f)result->LocalScale).x,
                       ((MathCore::vec3f)result->LocalScale).y,
                       ((MathCore::vec3f)result->LocalScale).z);

                printf("%sLocalRotation %f, %f, %f, %f\n",
                       spaces.c_str(),
                       ((MathCore::quatf)result->LocalRotation).x,
                       ((MathCore::quatf)result->LocalRotation).y,
                       ((MathCore::quatf)result->LocalRotation).z,
                       ((MathCore::quatf)result->LocalRotation).w);
            }

            if (node.geometries.size() > 0)
            {
                ReferenceCounter<AppKit::GLEngine::Component *> *refCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;

                /*
                Components::ComponentMaterial *material = NULL;
                if (defaultMaterial != NULL)
                    result->addComponent(refCount->add(material = defaultMaterial));
                else
                    result->addComponent(refCount->add(material = new Components::ComponentMaterial()));
                */

                int material_index = -1;
                for (size_t i = 0; i < node.geometries.size(); i++)
                {

                    // process the material
                    if (model_dynamic_upload != 0 || model_static_upload != 0)
                    {
                        uint32_t gidx = node.geometries[i];
                        const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];
                        const ITKExtension::Model::Material *mat = &container->materials[geom->materialIndex];

                        if (material_index != geom->materialIndex)
                        {
                            material_index = geom->materialIndex;

                            if (materialCache.find(geom->materialIndex) == materialCache.end())
                            {

                                printf("Processing Material: %s\n", mat->name.c_str());

                                Components::ComponentMaterial *material;
                                result->addComponent(refCount->add(material = new Components::ComponentMaterial()));

                                material->type = Components::MaterialPBR;

                                std::map<std::string, MathCore::vec4f>::const_iterator it = mat->vec4Value.find("diffuse");
                                if (it != mat->vec4Value.end())
                                {
                                    material->pbr.albedoColor = MathCore::CVT<MathCore::vec4f>::toVec3(it->second);
                                    // material->pbr.albedoColor = mat->vec4Value.at("diffuse");

                                    material->pbr.albedoColor = ResourceHelper::vec3ColorGammaToLinear(material->pbr.albedoColor);
                                }

                                // materialCache.insert_or_assign(geom->materialIndex, material);
                                materialCache[geom->materialIndex] = material;
                            }
                            else
                            {
                                result->addComponent(refCount->add(materialCache[geom->materialIndex]));
                            }
                        }
                    }

                    uint32_t gidx = node.geometries[i];
                    if (geometryCache.find(gidx) == geometryCache.end())
                    {
                        Components::ComponentMesh *mesh;
                        result->addComponent(refCount->add(mesh = new Components::ComponentMesh()));

                        const ITKExtension::Model::Geometry *geom = &container->geometries[gidx];

                        if (geom->pos.size() > 0)
                            mesh->pos = geom->pos;
                        if (geom->normals.size() > 0)
                            mesh->normals = geom->normals;
                        if (geom->tangent.size() > 0)
                            mesh->tangent = geom->tangent;
                        if (geom->binormal.size() > 0)
                            mesh->binormal = geom->binormal;
                        for (int j = 0; j < 8; j++)
                        {
                            if (geom->uv[j].size() > 0)
                                mesh->uv[j] = geom->uv[j];
                            if (geom->color[j].size() > 0)
                                mesh->color[j] = geom->color[j];
                        }

                        mesh->indices = geom->indice;

                        if (model_dynamic_upload != 0 || model_static_upload != 0)
                            mesh->syncVBO(model_dynamic_upload, model_static_upload);

                        if (geom->bones.size() > 0)
                            mesh->bones = geom->bones;

                        geometryCache[gidx] = mesh;
                    }
                    else
                    {
                        result->addComponent(refCount->add(geometryCache[gidx]));
                    }
                }
            }

            for (size_t i = 0; i < node.children.size(); i++)
            {
                uint32_t chidx = node.children[i];
                result->addChild(nodeTraverse(spaces + ".",
                                              chidx,
                                              container->nodes[chidx],
                                              container,
                                              geometryCache,
                                              materialCache,
                                              NULL,
                                              defaultMaterial,
                                              model_dynamic_upload, model_static_upload));
            }

            return result;
        }

        Transform *Basof2ToResource::loadAndConvert(
            const std::string &filename,
            Components::ComponentMaterial *defaultMaterial,
            Transform *rootNode,
            uint32_t model_dynamic_upload, uint32_t model_static_upload)
        {

            std::map<int, Components::ComponentMesh *> geometryCache;
            std::map<int, Components::ComponentMaterial *> materialCache;

            ITKExtension::Model::ModelContainer *container = new ITKExtension::Model::ModelContainer();
            container->read(filename.c_str());

            Transform *result = nodeTraverse(".",
                                             0,
                                             container->nodes[0],
                                             container,
                                             geometryCache,
                                             materialCache,
                                             rootNode,
                                             defaultMaterial,
                                             model_dynamic_upload, model_static_upload);

            delete container;

            /*
            if (resetRootTransform){
                result->LocalPosition = MathCore::vec3f(0);
                result->LocalRotation = MathCore::quatf();
                result->LocalScale = MathCore::vec3f(1.0f);
            }*/

            // result->LocalScale = MathCore::vec3f(1.0f);

            return result;
        }
    }
}