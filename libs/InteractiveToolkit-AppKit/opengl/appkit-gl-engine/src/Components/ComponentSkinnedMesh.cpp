#include <appkit-gl-engine/Components/ComponentSkinnedMesh.h>

#include <appkit-gl-engine/shaders/FrankenShader.h>

#include <InteractiveToolkit-Extension/model/ModelContainer.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            const ComponentType ComponentSkinnedMesh::Type = "ComponentSkinnedMesh";

            bool ComponentSkinnedMesh::countBonesLinkedToTransforms(std::shared_ptr<Transform> element, void *userData)
            {
                for (int i = 0; i < componentMesh->bones.size(); i++)
                {
                    ITKExtension::Model::Bone *bone = &componentMesh->bones[i];

                    // skip if this bone is already used
                    if (std::find(model_bone_list.begin(), model_bone_list.end(), bone) != model_bone_list.end())
                        continue;

                    if (bone->name.compare(element->Name) == 0)
                    {
                        transform_list.push_back(element);
                        int &count = *(int *)userData;
                        count++;

                        // ITKExtension::Model::Bone *bone = &componentMesh->bones[i];

                        // if (std::find(model_bone_list.begin(), model_bone_list.end(), bone) == model_bone_list.end())
                        model_bone_list.push_back(bone);
                        break;
                    }
                }
                return true;
            }

            void ComponentSkinnedMesh::computeSkinningInformation()
            {

                int count = 0;
                model_base->traversePreOrder_DepthFirst(
                    EventCore::CallbackWrapper(&ComponentSkinnedMesh::countBonesLinkedToTransforms, this),
                    &count);

                transform_to_bone.resize(count);
                for (int i = 0; i < transform_to_bone.size(); i++)
                    transform_to_bone[i] = i;
                // bone_target.resize(count);
                bone_inverse.resize(count);
                bone_gradient.resize(count);
                bone_processed.resize(count);
                for (int i = 0; i < bone_processed.size(); i++)
                    bone_processed[i] = false;

                for (int i = 0; i < transform_list.size(); i++)
                {
                    auto transform = ToShared(transform_list[i]);
                    transform->userData = &transform_to_bone[i];
                }

                ITK_ABORT(transform_list.size() != componentMesh->bones.size(), "The number of transforms is not equal to the number of bones.\n");
                ITK_ABORT(model_bone_list.size() != componentMesh->bones.size(), "The number of bones in sequence is not equal to the number of bones from model.\n");

                vertex_weight.resize(componentMesh->pos.size());

                model_base->resetVisited();
                model_base->preComputeTransforms();

                for (int i = 0; i < model_bone_list.size(); i++)
                {
                    ITKExtension::Model::Bone *bone = model_bone_list[i];

                    // fill up the vertex weight information
                    for (int j = 0; j < bone->weights.size(); j++)
                    {
                        ITKExtension::Model::VertexWeight &modelWeight = bone->weights[j];

                        SkinnedMesh_vertexWeight weight;
                        weight.bone_index = i;
                        weight.weight = modelWeight.weight;

                        ITK_ABORT(modelWeight.vertexID >= vertex_weight.size(), "Wrong vertex index inside the model bone information.\n");

                        vertex_weight[modelWeight.vertexID].push_back(weight);
                    }

                    // compute inverse matrix
                    auto transform = ToShared(transform_list[i]);

                    bone_inverse[i] = transform->getMatrixInverse(true);
                    // bone_target[i] = transform_list[i]->getMatrix(true);
                    bone_gradient[i] = MathCore::mat4f(); // identity

                    transform->OnVisited.add(&ComponentSkinnedMesh::OnTransformVisited, this);
                }
            }

            void ComponentSkinnedMesh::OnTransformVisited(std::shared_ptr<Transform> t)
            {
                if (t->userData == nullptr)
                    return;

                int bone_index = *(int *)t->userData;

                // bone_target[bone_index] = t->getMatrix(true);
                const MathCore::mat4f &bone_target = t->getMatrix(true);

                bone_gradient[bone_index] = bone_target * bone_inverse[bone_index];

                if (isGPUSkinning)
                {
                    if (componentMaterial != nullptr)
                        componentMaterial->skin_gradient_matrix_dirty = true;
                }
                else
                {
                    bone_processed[bone_index] = false;
                    skinning_dirty = true;
                }
            }

            void ComponentSkinnedMesh::applySkinning()
            {

                // skip the CPU skinning application
                if (isGPUSkinning)
                    return;

                if (!skinning_dirty)
                    return;

                skinning_dirty = false;

                bool have_normals = src_normals.size() > 0;
                bool have_tangent = src_tangent.size() > 0;
                bool have_binormal = src_binormal.size() > 0;

                // omp_set_num_threads(8);

// #pragma omp parallel for
// #pragma omp parallel default(none)
#pragma omp parallel for
                for (int i = 0; i < src_pos.size(); i++)
                {
                    std::vector<SkinnedMesh_vertexWeight> &pos_weights = vertex_weight[i];

                    bool already_processed = true;
                    for (int j = 0; j < pos_weights.size(); j++)
                    {
                        SkinnedMesh_vertexWeight &_weight = pos_weights[j];
                        already_processed = already_processed && bone_processed[_weight.bone_index];
                    }

                    if (already_processed)
                        continue;

                    MathCore::mat4f gradient = MathCore::mat4f(0);
                    for (int j = 0; j < pos_weights.size(); j++)
                    {
                        SkinnedMesh_vertexWeight &_weight = pos_weights[j];
                        // gradient += bone_target[_weight.bone_index] * bone_inverse[_weight.bone_index] * _weight.weight;
                        gradient += bone_gradient[_weight.bone_index] * _weight.weight;
                    }

                    componentMesh->pos[i] = MathCore::CVT<MathCore::vec4f>::toVec3(gradient * src_pos[i]);

                    if (have_normals)
                    {

                        MathCore::mat4f gradient_it = gradient.inverse_transpose_3x3();

                        MathCore::vec3f N = MathCore::OP<MathCore::vec3f>::normalize(MathCore::CVT<MathCore::vec4f>::toVec3(gradient_it * src_normals[i]));

                        componentMesh->normals[i] = N;

                        if (have_tangent)
                        {
                            MathCore::vec3f T = MathCore::OP<MathCore::vec3f>::normalize(MathCore::CVT<MathCore::vec4f>::toVec3(gradient * src_tangent[i]));
                            // fix ortogonality of the tangent
                            T = MathCore::OP<MathCore::vec3f>::normalize(T - MathCore::OP<MathCore::vec3f>::dot(T, N) * N);

                            componentMesh->tangent[i] = T;

                            if (have_binormal)
                            {
                                MathCore::vec3f B = MathCore::OP<MathCore::vec3f>::cross(T, N);
                                componentMesh->binormal[i] = B;
                            }
                        }
                    }
                }

                for (int i = 0; i < bone_processed.size(); i++)
                {
                    if (!bone_processed[i])
                        bone_processed[i] = true;
                }

                componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0);
            }

            ComponentSkinnedMesh::ComponentSkinnedMesh(ResourceHelper *_resourceHelper, bool _isGPUSkinning) : Component(ComponentSkinnedMesh::Type)
            {
                isGPUSkinning = _isGPUSkinning;
                model_base = nullptr;
                componentMesh = nullptr;
                componentMaterial = nullptr;
                resourceHelper = _resourceHelper;

                skinning_dirty = true;
            }

            void ComponentSkinnedMesh::loadModelBase(const std::string &filename)
            {

                printf("[ComponentSkinnedMesh] loadModelBase %s\n", filename.c_str());

                ITK_ABORT(model_base != nullptr, "trying to load base model twice");
                model_base = Transform::CreateShared();
                model_base->setName("__root__");
                model_base->addChild(resourceHelper->createTransformFromModel(
                    filename,
                    0, 0
                    // SkinnedMesh_VBO_Upload_Bitflag,0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag
                    ));

                // check amount of meshs
                auto all_meshs = model_base->findComponentsInChildren<ComponentMesh>();
                ITK_ABORT(all_meshs.size() != 1, "Trying to load a skinned model with more than 1 mesh data.\n");
                componentMesh = all_meshs[0];

                // bake vertex data
                auto component_transform = componentMesh->getTransform();

                MathCore::mat4f &modelToWorld = component_transform->getMatrix();
                MathCore::mat4f &modelToWorld_IT = component_transform->getMatrixInverseTranspose();
                // MathCore::mat4f modelToWorld_IT = transpose(inv(modelToWorld));

                src_pos.resize(componentMesh->pos.size());
                src_normals.resize(componentMesh->normals.size());
                src_tangent.resize(componentMesh->tangent.size());
                src_binormal.resize(componentMesh->binormal.size());

                // process the mesh data: make them global
                for (int i = 0; i < componentMesh->pos.size(); i++)
                {
                    componentMesh->pos[i] = MathCore::CVT<MathCore::vec4f>::toVec3(modelToWorld * MathCore::CVT<MathCore::vec3f>::toPtn4(componentMesh->pos[i]));
                    src_pos[i] = MathCore::CVT<MathCore::vec3f>::toPtn4(componentMesh->pos[i]);
                    if (componentMesh->normals.size() > 0)
                    {
                        MathCore::vec3f N = MathCore::OP<MathCore::vec3f>::normalize(MathCore::CVT<MathCore::vec4f>::toVec3(modelToWorld_IT * MathCore::CVT<MathCore::vec3f>::toVec4(componentMesh->normals[i])));
                        componentMesh->normals[i] = N;
                        src_normals[i] = MathCore::CVT<MathCore::vec3f>::toVec4(componentMesh->normals[i]);
                        if (componentMesh->tangent.size() > 0)
                        {
                            MathCore::vec3f T = MathCore::OP<MathCore::vec3f>::normalize(MathCore::CVT<MathCore::vec4f>::toVec3(modelToWorld * MathCore::CVT<MathCore::vec3f>::toVec4(componentMesh->tangent[i])));
                            // fix ortogonality of the tangent
                            T = MathCore::OP<MathCore::vec3f>::normalize(T - MathCore::OP<MathCore::vec3f>::dot(T, N) * N);
                            componentMesh->tangent[i] = T;
                            src_tangent[i] = MathCore::CVT<MathCore::vec3f>::toVec4(componentMesh->tangent[i]);
                            if (componentMesh->binormal.size() > 0)
                            {
                                MathCore::vec3f B = MathCore::OP<MathCore::vec3f>::cross(T, N);
                                componentMesh->binormal[i] = B;
                                src_binormal[i] = MathCore::CVT<MathCore::vec3f>::toVec4(componentMesh->binormal[i]);
                            }
                        }
                    }
                }

                computeSkinningInformation();

                // the model was loaded without any material... we need to create a dummy that will be replaced later
                {
                    auto transform = componentMesh->getTransform();

                    // Components::ComponentMaterial *material;
                    // ReferenceCounter<AppKit::GLEngine::Component *> *refCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;
                    auto material = transform->addNewComponent<Components::ComponentMaterial>();
                    material->type = Components::MaterialPBR;
                }

                if (isGPUSkinning)
                {
                    // check for GPU eligibility
                    //  if pass: create the GPU data
                    int matrix_uniform_count = (int)bone_gradient.size();
                    int max_uniform_matrix = 0;

                    if (matrix_uniform_count <= 16)
                    {
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_VERTEX_WEIGHT16;
                        max_uniform_matrix = 16;
                    }
                    else if (matrix_uniform_count <= 32)
                    {
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_VERTEX_WEIGHT32;
                        max_uniform_matrix = 32;
                    }
                    else if (matrix_uniform_count <= 64)
                    {
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_VERTEX_WEIGHT64;
                        max_uniform_matrix = 64;
                    }
                    else if (matrix_uniform_count <= 96)
                    {
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_VERTEX_WEIGHT96;
                        max_uniform_matrix = 96;
                    }
                    else if (matrix_uniform_count <= 128)
                    {
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = ITKExtension::Model::CONTAINS_VERTEX_WEIGHT128;
                        max_uniform_matrix = 128;
                    }
                    else
                        SkinnedMesh_GPU_VBO_Upload_Bitflag = 0;

                    bool max_4_influence = true;

                    if (SkinnedMesh_GPU_VBO_Upload_Bitflag != 0)
                    {
                        // passed through the matrix count

                        // now check the max vertex influence of 4
                        for (int i = 0; i < vertex_weight.size(); i++)
                        {
                            if (vertex_weight[i].size() > 4)
                            {
                                max_4_influence = false;
                                break;
                            }
                        }

                        if (max_4_influence)
                        {
                            // guarantee the max size for the upload
                            // bone_gradient.resize(max_uniform_matrix);

                            // compute the 4 vertex influence vertex attribute
                            componentMesh->skin_index.resize(vertex_weight.size());
                            componentMesh->skin_weights.resize(vertex_weight.size());
                            for (int i = 0; i < vertex_weight.size(); i++)
                            {
                                int last_index = 0;
                                for (int j = 0; j < 4; j++)
                                {
                                    if (j < vertex_weight[i].size())
                                    {
                                        last_index = j;
                                        componentMesh->skin_index[i][j] = float(vertex_weight[i][j].bone_index) + 0.2f;
                                        componentMesh->skin_weights[i][j] = vertex_weight[i][j].weight;
                                    }
                                    else
                                    {
                                        // access the last bone matrix to increase cache hit on non-processed weights
                                        componentMesh->skin_index[i][j] = float(vertex_weight[i][last_index].bone_index) + 0.2f;
                                        componentMesh->skin_weights[i][j] = 0;
                                    }
                                }
                            }

                            // sync all static...
                            componentMesh->syncVBO(0, 0xffffffff);
                            componentMesh->always_clone = false;
                        }
                    }

                    // if doesnt pass the GPU check... set to false and use CPU
                    if (SkinnedMesh_GPU_VBO_Upload_Bitflag == 0 || !max_4_influence)
                    {
                        printf("[ComponentSkinnedMesh] Mesh does not match requirements to be processed by the GPU (less than 128 bones and max of 4 bone weight per vertex)\n");
                        isGPUSkinning = false;
                    }
                }

                if (!isGPUSkinning)
                {
                    // SkinnedMesh_VBO_Upload_Bitflag,0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag
                    // componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0);

                    componentMesh->syncVBO(SkinnedMesh_VBO_Upload_Bitflag, 0xffffffff ^ SkinnedMesh_VBO_Upload_Bitflag);
                    componentMesh->always_clone = true;
                }
            }

            void ComponentSkinnedMesh::moveMeshToTransform()
            {

                auto all_materials = model_base->findComponentsInChildren<ComponentMaterial>();
                ITK_ABORT(all_materials.size() != 1, "Trying to load a skinned model with %u material data (it must be 1).\n", (uint32_t)all_materials.size());
                componentMaterial = all_materials[0];

                if (isGPUSkinning)
                {
                    componentMaterial->skin_gradient_matrix_dirty = true;
                    componentMaterial->skin_gradient_matrix = &bone_gradient;
                    switch (SkinnedMesh_GPU_VBO_Upload_Bitflag)
                    {
                    case ITKExtension::Model::CONTAINS_VERTEX_WEIGHT16:
                        componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_16;
                        break;
                    case ITKExtension::Model::CONTAINS_VERTEX_WEIGHT32:
                        componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_32;
                        break;
                    case ITKExtension::Model::CONTAINS_VERTEX_WEIGHT64:
                        componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_64;
                        break;
                    case ITKExtension::Model::CONTAINS_VERTEX_WEIGHT96:
                        componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_96;
                        break;
                    case ITKExtension::Model::CONTAINS_VERTEX_WEIGHT128:
                        componentMaterial->skin_shader_matrix_size_bitflag = ShaderAlgorithms_skinGradientMatrix_128;
                        break;
                    }
                }

                printf("[ComponentSkinnedMesh] moveMeshToTransform\n");
                ITK_ABORT(getTransformCount() <= 0, "You need to add this class to a scene node before call loadModelBase.\n");
                ITK_ABORT(getTransformCount() > 1, "You can attach this component to one node only.\n");
                ITK_ABORT(componentMesh == nullptr || componentMaterial == nullptr, "You need to call loadModelBase before moveMeshToTransform.\n");

                auto transform = getTransform();

                auto component_transform = componentMaterial->getTransform();
                component_transform->removeComponent(componentMaterial);
                transform->addComponent(componentMaterial);

                component_transform = componentMesh->getTransform();
                component_transform->removeComponent(componentMesh);
                transform->addComponent(componentMesh);

                // transform[0]->addChild(model_base);
                // model_base = nullptr;
            }

            void ComponentSkinnedMesh::loadAnimation(const std::string &clip_name, const std::string &filename)
            {
                printf("[ComponentSkinnedMesh] loadAnimation %s\n", filename.c_str());

                ITK_ABORT(model_base == nullptr, "you need to call loadModelBase before load a new animation clip.\n");

                ITKExtension::Model::ModelContainer *container = new ITKExtension::Model::ModelContainer();
                container->read(filename.c_str());

                ITK_ABORT(container->animations.size() != 1, "loadanimation method only supports one animation per file.\n");

                for (int i = 0; i < container->animations.size(); i++)
                {
                    // the mixer will delete the animation clip automatically
                    mixer.addClip(new AnimationClip(clip_name, model_base, container->animations[i]));
                }

                delete container;
            }

            void ComponentSkinnedMesh::done()
            {
                printf("[ComponentSkinnedMesh] done\n");

                // clip_motion = MathCore::vec3f(0);
                mixer.computeTransitions();
                // Transform* root_node = mixer.rootNode();
                // root_node->setPosition(clip_motion);

                // every time we sample the scene graph, we need to precompute all transforms to update the visited flag
                model_base->resetVisited();
                model_base->preComputeTransforms();

                applySkinning();
            }

            void ComponentSkinnedMesh::start()
            {
                // AppBase* app = Engine::Instance()->app;
                auto transform = getTransform();

                renderWindowRegionRef = transform->renderWindowRegion;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                renderWindowRegion->OnPreUpdate.add(&ComponentSkinnedMesh::OnPreUpdate, this);
            }

            void ComponentSkinnedMesh::OnPreUpdate(Platform::Time *time)
            {
                mixer.update(time->deltaTime);

                // every time we sample the scene graph, we need to precompute all transforms to update the visited flag
                model_base->resetVisited();
                model_base->preComputeTransforms();

                applySkinning();
            }

            ComponentSkinnedMesh::~ComponentSkinnedMesh()
            {
                // AppBase* app = Engine::Instance()->app;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                {
                    renderWindowRegion->OnPreUpdate.remove(&ComponentSkinnedMesh::OnPreUpdate, this);
                }

                // release model_base
                model_base = nullptr;
                // ResourceHelper::releaseTransformRecursive(&model_base);
                //  AppBase* app = Engine::Instance()->app;
            }

            // always clone
            std::shared_ptr<Component> ComponentSkinnedMesh::duplicate_ref_or_clone(bool force_clone)
            {
                auto result = Component::CreateShared<ComponentSkinnedMesh>();

                result->mixer.copy(this->mixer);

                result->model_base = this->model_base; // the actually animation hierarchy
                result->resourceHelper = this->resourceHelper;

                result->componentMesh = this->componentMesh;
                result->componentMaterial = this->componentMaterial;

                //
                // bone processing aux structure
                //
                result->bone_processed = this->bone_processed;

                result->bone_gradient = this->bone_gradient;
                // std::vector<MathCore::mat4f> bone_target;
                result->bone_inverse = this->bone_inverse;

                result->vertex_weight = this->vertex_weight;

                result->src_pos = this->src_pos;
                result->src_normals = this->src_normals;
                result->src_tangent = this->src_tangent;
                result->src_binormal = this->src_binormal;

                result->transform_to_bone = this->transform_to_bone;
                result->transform_list = this->transform_list;
                result->model_bone_list = this->model_bone_list;

                //
                // GPU Skinning
                //
                result->isGPUSkinning = this->isGPUSkinning;
                result->SkinnedMesh_GPU_VBO_Upload_Bitflag = this->SkinnedMesh_GPU_VBO_Upload_Bitflag; // model::CONTAINS_VERTEX_WEIGHT16;

                return result;
            }
            void ComponentSkinnedMesh::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {

                TransformMapT internalModelMap;

                this->model_base = this->model_base->clone(false, &internalModelMap);

                {
                    mixer.fix_internal_references(internalModelMap);
                }

                {
                    auto found = componentMap.find(componentMesh);
                    if (found != componentMap.end())
                    {
                        auto newComponentMesh = std::dynamic_pointer_cast<ComponentMesh>(found->second);

                        // remap bone from old mesh to new mesh
                        std::unordered_map<ITKExtension::Model::Bone *, ITKExtension::Model::Bone *> boneMap;
                        for (int i = 0; i < componentMesh->bones.size(); i++)
                            boneMap[&componentMesh->bones[i]] = &newComponentMesh->bones[i];
                        
                        componentMesh = newComponentMesh;

                        for(auto &item: model_bone_list)
                            item = boneMap[item];
                    }
                }
                {
                    auto found = componentMap.find(componentMaterial);
                    if (found != componentMap.end())
                        componentMaterial = std::dynamic_pointer_cast<ComponentMaterial>(found->second);
                }
                {
                    for (int i = 0; i < transform_list.size(); i++)
                    {
                        auto transform = ToShared(transform_list[i]);

                        auto found = internalModelMap.find(transform);
                        if (found != internalModelMap.end())
                        {
                            transform = found->second;
                            transform_list[i] = transform;
                            transform->userData = &transform_to_bone[i];
                        } else {
                            printf("some error occured in the transform map\n");
                        }

                    }
                }
            }

            void ComponentSkinnedMesh::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentSkinnedMesh::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
                
            }
            void ComponentSkinnedMesh::Deserialize(rapidjson::Value &_value,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                  std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                  ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentSkinnedMesh::Type) == 0)
                    return;
                
            }

        }
    }
}
