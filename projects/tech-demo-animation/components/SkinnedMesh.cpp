#include "SkinnedMesh.h"
#include <mini-gl-engine/FrankenShader.h>

namespace GLEngine {
    namespace Components {

        const ComponentType SkinnedMesh::Type = "SkinnedMesh";


        bool SkinnedMesh::countBonesLinkedToTransforms(Transform *element, void* userData) {
            for (int i = 0; i < componentMesh->bones.size(); i++) {
                if (componentMesh->bones[i].name.compare(element->Name) == 0) {
                    transform_list.push_back(element);
                    int &count = *(int*)userData;
                    count++;

                    model::Bone *bone = &componentMesh->bones[i];

                    if (std::find(model_bone_list.begin(), model_bone_list.end(), bone) == model_bone_list.end())
                        model_bone_list.push_back(bone);
                }
            }
            return true;
        }

        void SkinnedMesh::computeSkinningInformation() {

            int count = 0;
            model_base->traversePreOrder_DepthFirst(TransformTraverseMethod(this, &SkinnedMesh::countBonesLinkedToTransforms), &count);

            transform_to_bone.resize(count);
            for (int i = 0; i < transform_to_bone.size(); i++)
                transform_to_bone[i] = i;
            //bone_target.resize(count);
            bone_inverse.resize(count);
            bone_gradient.resize(count);
            bone_processed.resize(count);
            for (int i = 0; i < bone_processed.size(); i++)
                bone_processed[i] = false;


            for (int i = 0; i < transform_list.size(); i++)
                transform_list[i]->userData = &transform_to_bone[i];

            ARIBEIRO_ABORT(transform_list.size() != componentMesh->bones.size(), "The number of transforms is not equal to the number of bones.\n");
            ARIBEIRO_ABORT(model_bone_list.size() != componentMesh->bones.size(), "The number of bones in sequence is not equal to the number of bones from model.\n");

            vertex_weight.resize(componentMesh->pos.size());

            model_base->resetVisited();
            model_base->preComputeTransforms();

            for (int i = 0; i < model_bone_list.size(); i++) {
                model::Bone *bone = model_bone_list[i];

                //fill up the vertex weight information
                for (int j = 0; j < bone->weights.size(); j++) {
                    model::VertexWeight &modelWeight = bone->weights[j];

                    SkinnedMesh_vertexWeight weight;
                    weight.bone_index = i;
                    weight.weight = modelWeight.weight;

                    ARIBEIRO_ABORT(modelWeight.vertexID >= vertex_weight.size(), "Wrong vertex index inside the model bone information.\n");

                    vertex_weight[modelWeight.vertexID].push_back(weight);
                }


                // compute inverse matrix
                bone_inverse[i] = transform_list[i]->getMatrixInverse(true);
                //bone_target[i] = transform_list[i]->getMatrix(true);
                bone_gradient[i] = aRibeiro::mat4();//identity

                transform_list[i]->OnVisited.add(this, &SkinnedMesh::OnTransformVisited);
            }
        }

        void SkinnedMesh::OnTransformVisited(Transform* t) {
            if (t->userData == NULL)
                return;

            int bone_index = *(int*)t->userData;

            //bone_target[bone_index] = t->getMatrix(true);
            const aRibeiro::mat4 &bone_target = t->getMatrix(true);

            bone_gradient[bone_index] = bone_target * bone_inverse[bone_index];
            
            if (isGPUSkinning) {
                if (componentMaterial != NULL)
                    componentMaterial->skin_gradient_matrix_dirty = true;
            }
            else {
                bone_processed[bone_index] = false;
                skinning_dirty = true;
            }

        }

        void SkinnedMesh::applySkinning() {

            //skip the CPU skinning application
            if (isGPUSkinning)
                return;

            if (!skinning_dirty)
                return;

            skinning_dirty = false;

            bool have_normals = src_normals.size() > 0;
            bool have_tangent = src_tangent.size() > 0;
            bool have_binormal = src_binormal.size() > 0;

            //omp_set_num_threads(8);

//#pragma omp parallel for
//#pragma omp parallel default(none)
#pragma omp parallel for
            for (int i = 0; i < src_pos.size(); i++)
            {
                std::vector< SkinnedMesh_vertexWeight > &pos_weights = vertex_weight[i];

                bool already_processed = true;
                for (int j = 0; j < pos_weights.size(); j++) {
                    SkinnedMesh_vertexWeight &_weight = pos_weights[j];
                    already_processed = already_processed && bone_processed[_weight.bone_index];
                }

                if (already_processed)
                    continue;

                aRibeiro::mat4 gradient = aRibeiro::mat4(0);
                for (int j = 0; j < pos_weights.size(); j++) {
                    SkinnedMesh_vertexWeight &_weight = pos_weights[j];
                    //gradient += bone_target[_weight.bone_index] * bone_inverse[_weight.bone_index] * _weight.weight;
                    gradient += bone_gradient[_weight.bone_index] * _weight.weight;
                }

                componentMesh->pos[i] = aRibeiro::toVec3(gradient * src_pos[i]);

                if (have_normals) {

                    aRibeiro::mat4 gradient_it = aRibeiro::inverse_transpose_rotation_3(gradient);

                    aRibeiro::vec3 N = aRibeiro::normalize(aRibeiro::toVec3(gradient_it * src_normals[i]));

                    componentMesh->normals[i] = N;

                    if (have_tangent) {
                        aRibeiro::vec3 T = aRibeiro::normalize(aRibeiro::toVec3(gradient * src_tangent[i]));
                        //fix ortogonality of the tangent
                        T = aRibeiro::normalize(T - aRibeiro::dot(T, N) * N);

                        componentMesh->tangent[i] = T;

                        if (have_binormal) {
                            aRibeiro::vec3 B = aRibeiro::cross(T, N);
                            componentMesh->binormal[i] = B;
                        }
                    }
                }
            }

            for (int i = 0; i < bone_processed.size(); i++) {
                if (!bone_processed[i])
                    bone_processed[i] = true;
            }

            componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0);
        }

        SkinnedMesh::SkinnedMesh(ResourceHelper *_resourceHelper, bool _isGPUSkinning) :Component(SkinnedMesh::Type) {
            isGPUSkinning = _isGPUSkinning;
            model_base = NULL;
            componentMesh = NULL;
            componentMaterial = NULL;
            resourceHelper = _resourceHelper;

            skinning_dirty = true;
        }

        void SkinnedMesh::loadModelBase(const std::string &filename) {

            printf("[SkinnedMesh] loadModelBase %s\n", filename.c_str());

            ARIBEIRO_ABORT(model_base != NULL, "trying to load base model twice");
            model_base = new Transform();
            model_base->setName("__root__");
            model_base->addChild(resourceHelper->createTransformFromModel(
                filename,
                0,0
                //SkinnedMesh_VBO_Upload_Bitflag,0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag
            ));

            //check amount of meshs
            std::vector<Component*> all_meshs = model_base->findComponentsInChildren(ComponentMesh::Type);
            ARIBEIRO_ABORT(all_meshs.size() != 1, "Trying to load a skinned model with more than 1 mesh data.\n");
            componentMesh = (ComponentMesh*)all_meshs[0];

            //bake vertex data
            Transform *component_transform = componentMesh->transform[0];

            aRibeiro::mat4 &modelToWorld = component_transform->getMatrix();
            aRibeiro::mat4 &modelToWorld_IT = component_transform->getMatrixInverseTranspose();
            //aRibeiro::mat4 modelToWorld_IT = transpose(inv(modelToWorld));


            src_pos.resize(componentMesh->pos.size());
            src_normals.resize(componentMesh->normals.size());
            src_tangent.resize(componentMesh->tangent.size());
            src_binormal.resize(componentMesh->binormal.size());

            // process the mesh data: make them global
            for (int i = 0; i < componentMesh->pos.size(); i++) {
                componentMesh->pos[i] = aRibeiro::toVec3(modelToWorld * aRibeiro::toPtn4(componentMesh->pos[i]));
                src_pos[i] = toPtn4(componentMesh->pos[i]);
                if (componentMesh->normals.size() > 0) {
                    aRibeiro::vec3 N = aRibeiro::normalize(aRibeiro::toVec3(modelToWorld_IT * aRibeiro::toVec4(componentMesh->normals[i])));
                    componentMesh->normals[i] = N;
                    src_normals[i] = aRibeiro::toVec4(componentMesh->normals[i]);
                    if (componentMesh->tangent.size() > 0) {
                        aRibeiro::vec3 T = aRibeiro::normalize(aRibeiro::toVec3(modelToWorld * aRibeiro::toVec4(componentMesh->tangent[i])));
                        //fix ortogonality of the tangent
                        T = normalize(T - dot(T, N) * N);
                        componentMesh->tangent[i] = T;
                        src_tangent[i] = aRibeiro::toVec4(componentMesh->tangent[i]);
                        if (componentMesh->binormal.size() > 0) {
                            aRibeiro::vec3 B = cross(T, N);
                            componentMesh->binormal[i] = B;
                            src_binormal[i] = aRibeiro::toVec4(componentMesh->binormal[i]);
                        }
                    }
                }
            }

            computeSkinningInformation();

            // the model was loaded without any material... we need to create a dummy that will be replaced later
            {
                Components::ComponentMaterial *material;
                ReferenceCounter<GLEngine::Component*> *refCount = &GLEngine::Engine::Instance()->componentReferenceCounter;
                componentMesh->transform[0]->addComponent(refCount->add(material = new Components::ComponentMaterial()));
                material->type = Components::MaterialPBR;
            }

            if (isGPUSkinning)
            {
                //check for GPU eligibility
                // if pass: create the GPU data
                int matrix_uniform_count = bone_gradient.size();
                int max_uniform_matrix = 0;

                if (matrix_uniform_count <= 16) {
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = model::CONTAINS_VERTEX_WEIGHT16;
                    max_uniform_matrix = 16;
                }
                else if (matrix_uniform_count <= 32) {
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = model::CONTAINS_VERTEX_WEIGHT32;
                    max_uniform_matrix = 32;
                }
                else if (matrix_uniform_count <= 64) {
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = model::CONTAINS_VERTEX_WEIGHT64;
                    max_uniform_matrix = 64;
                }
                else if (matrix_uniform_count <= 96) {
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = model::CONTAINS_VERTEX_WEIGHT96;
                    max_uniform_matrix = 96;
                }
                else if (matrix_uniform_count <= 128) {
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = model::CONTAINS_VERTEX_WEIGHT128;
                    max_uniform_matrix = 128;
                }
                else
                    SkinnedMesh_GPU_VBO_Upload_Bitflag = 0;
                
                bool max_4_influence = true;

                if (SkinnedMesh_GPU_VBO_Upload_Bitflag != 0) {
                    // passed through the matrix count

                    //now check the max vertex influence of 4
                    for (int i = 0; i < vertex_weight.size(); i++) {
                        if (vertex_weight[i].size() > 4) {
                            max_4_influence = false;
                            break;
                        }
                    }

                    if (max_4_influence) {
                        // guarantee the max size for the upload
                        //bone_gradient.resize(max_uniform_matrix);

                        // compute the 4 vertex influence vertex attribute
                        componentMesh->skin_index.resize(vertex_weight.size());
                        componentMesh->skin_weights.resize(vertex_weight.size());
                        for (int i = 0; i < vertex_weight.size(); i++) {
                            int last_index = 0;
                            for (int j = 0; j < 4; j++) {
                                if (j < vertex_weight[i].size()) {
                                    last_index = j;
                                    componentMesh->skin_index[i][j] = float(vertex_weight[i][j].bone_index) + 0.2f;
                                    componentMesh->skin_weights[i][j] = vertex_weight[i][j].weight;
                                }
                                else {
                                    //access the last bone matrix to increase cache hit on non-processed weights
                                    componentMesh->skin_index[i][j] = float(vertex_weight[i][last_index].bone_index) + 0.2f;
                                    componentMesh->skin_weights[i][j] = 0;
                                }
                            }
                        }

                        // sync all static...
                        componentMesh->syncVBO(0, 0xffffffff);

                    }
                }

                //if doesnt pass the GPU check... set to false and use CPU
                if (SkinnedMesh_GPU_VBO_Upload_Bitflag == 0 || !max_4_influence) {
                    printf("[SkinnedMesh] Mesh does not match requirements to be processed by the GPU (less than 128 bones and max of 4 bone weight per vertex)\n");
                    isGPUSkinning = false;
                }
            }

            if (!isGPUSkinning){
                //SkinnedMesh_VBO_Upload_Bitflag,0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag
                //componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0);

                componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag);
            }

        }

        void SkinnedMesh::moveMeshToTransform() {


            std::vector<Component*> all_materials = model_base->findComponentsInChildren(ComponentMaterial::Type);
            ARIBEIRO_ABORT(all_materials.size() != 1, "Trying to load a skinned model with %i material data (it must be 1).\n", all_materials.size());
            componentMaterial = (ComponentMaterial*)all_materials[0];

            if (isGPUSkinning) {
                componentMaterial->skin_gradient_matrix_dirty = true;
                componentMaterial->skin_gradient_matrix = &bone_gradient;
                switch (SkinnedMesh_GPU_VBO_Upload_Bitflag) {
                case model::CONTAINS_VERTEX_WEIGHT16:
                    componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_16;
                    break;
                case model::CONTAINS_VERTEX_WEIGHT32:
                    componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_32;
                    break;
                case model::CONTAINS_VERTEX_WEIGHT64:
                    componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_64;
                    break;
                case model::CONTAINS_VERTEX_WEIGHT96:
                    componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_96;
                    break;
                case model::CONTAINS_VERTEX_WEIGHT128:
                    componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_128;
                    break;
                }
            }

            printf("[SkinnedMesh] moveMeshToTransform\n");
            ARIBEIRO_ABORT(transform.size() <= 0, "You need to add this class to a scene node before call loadModelBase.\n");
            ARIBEIRO_ABORT(transform.size() > 1, "You can attach this component to one node only.\n");
            ARIBEIRO_ABORT(componentMesh == NULL || componentMaterial == NULL, "You need to call loadModelBase before moveMeshToTransform.\n");

            Transform *component_transform = componentMaterial->transform[0];
            component_transform->removeComponent(componentMaterial);
            transform[0]->addComponent(componentMaterial);

            component_transform = componentMesh->transform[0];
            component_transform->removeComponent(componentMesh);
            transform[0]->addComponent(componentMesh);



            //transform[0]->addChild(model_base);
            //model_base = NULL;
        }

        void SkinnedMesh::loadAnimation(const std::string &clip_name, const std::string &filename) {
            printf("[SkinnedMesh] loadAnimation %s\n", filename.c_str());

            ARIBEIRO_ABORT(model_base == NULL, "you need to call loadModelBase before load a new animation clip.\n");

            model::ModelContainer *container = new model::ModelContainer();
            container->read(filename.c_str());

            ARIBEIRO_ABORT(container->animations.size() != 1, "loadanimation method only supports one animation per file.\n");

            for (int i = 0; i < container->animations.size(); i++) {
                // the mixer will delete the animation clip automatically
                mixer.addClip(new AnimationClip(clip_name, model_base, container->animations[i]));
            }

            aRibeiro::setNullAndDelete(container);

        }

        void SkinnedMesh::done() {
            printf("[SkinnedMesh] done\n");
            
            //clip_motion = aRibeiro::vec3(0);
            mixer.computeTransitions();
            //Transform* root_node = mixer.rootNode();
            //root_node->setPosition(clip_motion);

            //every time we sample the scene graph, we need to precompute all transforms to update the visited flag
            model_base->resetVisited();
            model_base->preComputeTransforms();

            applySkinning();
        }

        void SkinnedMesh::start() {
            AppBase* app = Engine::Instance()->app;
            app->OnPreUpdate.add(this, &SkinnedMesh::OnPreUpdate);
        }

        void SkinnedMesh::OnPreUpdate(aRibeiro::PlatformTime *time) {
            mixer.update(time->deltaTime);

            //every time we sample the scene graph, we need to precompute all transforms to update the visited flag
            model_base->resetVisited();
            model_base->preComputeTransforms();

            applySkinning();
        }

        SkinnedMesh::~SkinnedMesh() {
            AppBase* app = Engine::Instance()->app;
            app->OnPreUpdate.remove(this, &SkinnedMesh::OnPreUpdate);

            ResourceHelper::releaseTransformRecursive(&model_base);
            //AppBase* app = Engine::Instance()->app;
        }

    }

};
