#include "Basof2ToResource.h"

#include <mini-gl-engine/mini-gl-engine.h>
#include <data-model/data-model.h>
#include <aribeiro/aribeiro.h>

namespace GLEngine {
    
    Transform* Basof2ToResource::nodeTraverse(const std::string &spaces,
                                              int currentIndex,
                                   const model::Node &node,
                                   const model::ModelContainer *container,
                                   std::map<int, Components::ComponentMesh *> &geometryCache,
                                   std::map<int, Components::ComponentMaterial *> &materialCache,
                                   Transform *root,
                                   Components::ComponentMaterial *defaultMaterial,
                                    uint32_t model_dynamic_upload, uint32_t model_static_upload) {
        
        Transform* result = root;
        
        if (result == NULL){
            result = new Transform();
            
            //mat4 m = inv(transpose(node.transform));
            //mat4 m = transpose(node.transform);
            aRibeiro::mat4 m = node.transform;
            
            //m[2] = aRibeiro::toVec4( aRibeiro::cross( aRibeiro::toVec3(m[1]), aRibeiro::toVec3(m[0]) ) );
            
            /*
            result->LocalPosition = toVec3(m * vec4(0,0,0,1)) * vec3(1,1,-1);
            result->LocalRotation = extractQuat( m ) * aRibeiro::quatFromEuler(0, 0, DEG2RAD(180.0));
            result->LocalScale = aRibeiro::vec3(aRibeiro::length(m[0]),
                                                aRibeiro::length(m[1]),
                                                aRibeiro::length(m[2]));
             */
            
            result->LocalPosition = toVec3(m * aRibeiro::vec4(0,0,0,1));
            
            result->LocalScale = aRibeiro::vec3(aRibeiro::length(m[0]),
                                                aRibeiro::length(m[1]),
                                                aRibeiro::length(m[2]));
            
            m = aRibeiro::extractRotation(m);
            
            m[0] = aRibeiro::normalize(m[0]);
            m[1] = aRibeiro::normalize(m[1]);
            m[2] = aRibeiro::normalize(m[2]);
            
            result->LocalRotation = aRibeiro::extractQuat( m );
            
            result->Name = node.name;
            
            printf("%s(%s)\n", spaces.c_str(), node.name.c_str());
            printf("%sLocalPosition %f, %f, %f\n",
                   spaces.c_str(),
                   ((aRibeiro::vec3)result->LocalPosition).x,
                   ((aRibeiro::vec3)result->LocalPosition).y,
                   ((aRibeiro::vec3)result->LocalPosition).z);
            
            printf("%sLocalScale %f, %f, %f\n",
                   spaces.c_str(),
                   ((aRibeiro::vec3)result->LocalScale).x,
                   ((aRibeiro::vec3)result->LocalScale).y,
                   ((aRibeiro::vec3)result->LocalScale).z);
            
            printf("%sLocalRotation %f, %f, %f, %f\n",
                   spaces.c_str(),
                   ((aRibeiro::quat)result->LocalRotation).x,
                   ((aRibeiro::quat)result->LocalRotation).y,
                   ((aRibeiro::quat)result->LocalRotation).z,
                   ((aRibeiro::quat)result->LocalRotation).w);
            
        }
            
        
        
        if ( node.geometries.size() > 0 ){
            ReferenceCounter<GLEngine::Component*> *refCount = &GLEngine::Engine::Instance()->componentReferenceCounter;
            
            

            /*
            Components::ComponentMaterial *material = NULL;
            if (defaultMaterial != NULL)
                result->addComponent(refCount->add(material = defaultMaterial));
            else
                result->addComponent(refCount->add(material = new Components::ComponentMaterial()));
            */
            
            
            int material_index = -1;
            for(size_t i=0;i<node.geometries.size();i++){

                //process the material
                if (model_dynamic_upload != 0 || model_static_upload != 0)
                {
                    uint32_t gidx = node.geometries[i];
                    const model::Geometry *geom = &container->geometries[gidx];
                    const model::Material *mat = &container->materials[geom->materialIndex];

                    if (material_index != geom->materialIndex) {
                        material_index = geom->materialIndex;

                        if (materialCache.find(geom->materialIndex) == materialCache.end()) {

                            printf("Processing Material: %s\n", mat->name.c_str());

                            Components::ComponentMaterial *material;
                            result->addComponent(refCount->add(material = new Components::ComponentMaterial()));

                            material->type = Components::MaterialPBR;

                            aRibeiro::aligned_map<std::string, aRibeiro::vec4>::const_iterator it = mat->vec4Value.find("diffuse");
                            if (it != mat->vec4Value.end()) {
                                material->pbr.albedoColor = it->second;
                                //material->pbr.albedoColor = mat->vec4Value.at("diffuse");

                                material->pbr.albedoColor = ResourceHelper::vec4ColorGammaToLinear(material->pbr.albedoColor);
                            }

                            //materialCache.insert_or_assign(geom->materialIndex, material);
                            materialCache[geom->materialIndex] = material;
                        }
                        else {
                            result->addComponent(refCount->add(materialCache[geom->materialIndex]));
                        }
                    }

                }

                uint32_t gidx = node.geometries[i];
                if (geometryCache.find(gidx) == geometryCache.end()) {
                    Components::ComponentMesh *mesh;
                    result->addComponent(refCount->add(mesh = new Components::ComponentMesh()));
                    
                    const model::Geometry *geom = &container->geometries[gidx];

                    if (geom->pos.size()>0)
                        mesh->pos = geom->pos;
                    if (geom->normals.size()>0)
                        mesh->normals = geom->normals;
                    if (geom->tangent.size()>0)
                        mesh->tangent = geom->tangent;
                    if (geom->binormal.size()>0)
                        mesh->binormal = geom->binormal;
                    for(int j=0;j<8;j++){
                        if (geom->uv[j].size()>0)
                            mesh->uv[j] = geom->uv[j];
                        if (geom->color[j].size()>0)
                            mesh->color[j] = geom->color[j];
                    }
                    
                    mesh->indices = geom->indice;
                    
                    if (model_dynamic_upload != 0 || model_static_upload != 0)
                        mesh->syncVBO(model_dynamic_upload, model_static_upload);

                    if (geom->bones.size() > 0)
                        mesh->bones = geom->bones;
                    
                    geometryCache[gidx] = mesh;
                } else {
                    result->addComponent(refCount->add(geometryCache[gidx]));
                }
            }
        }
        
        for(size_t i=0;i<node.children.size();i++){
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
    

    Transform* Basof2ToResource::loadAndConvert(
        const std::string &filename,
        Components::ComponentMaterial *defaultMaterial,
        Transform *rootNode,
        uint32_t model_dynamic_upload, uint32_t model_static_upload) {
        
        std::map<int, Components::ComponentMesh *> geometryCache;
        std::map<int, Components::ComponentMaterial *> materialCache;
        
        model::ModelContainer *container = new model::ModelContainer();
        container->read(filename.c_str());
        
        Transform* result = nodeTraverse(".",
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
            result->LocalPosition = aRibeiro::vec3(0);
            result->LocalRotation = aRibeiro::quat();
            result->LocalScale = aRibeiro::vec3(1.0f);
        }*/
        
        //result->LocalScale = aRibeiro::vec3(1.0f);
        
        return result;
    }
}
