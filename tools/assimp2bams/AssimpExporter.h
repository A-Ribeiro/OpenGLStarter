
#ifndef ASSIMP_EXPORTER_H
#define ASSIMP_EXPORTER_H

#include <data-model/data-model.h>
using namespace model;
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

/*
void splitString(std::string input, std::string *outFolder, std::string *outFilename, std::string *outFileWOExt, std::string *outFileExt) {
    //
    // normalize path separator
    //
    std::replace(input.begin(), input.end(), '\\', PlatformPath::SEPARATOR[0]);
    std::replace(input.begin(), input.end(), '/', PlatformPath::SEPARATOR[0]);
    
    
    std::string folder, filename, filename_wo_ext, ext;
    size_t path_directory_index = input.find_last_of(PlatformPath::SEPARATOR[0]);
    if (path_directory_index == -1) {
        folder = ".";
        filename = input;
    }
    else {
        folder = input.substr(0, path_directory_index);
        filename = input.substr(path_directory_index + 1, input.size() - 1 - path_directory_index);
    }
    
    
    path_directory_index = filename.find_last_of('.');
    if (path_directory_index == -1) {
        filename_wo_ext = filename;
        ext = "";
    }
    else {
        filename_wo_ext = filename.substr(0, path_directory_index);
        ext = filename.substr(path_directory_index + 1, filename.size() - 1 - path_directory_index);
    }
    
    
    
    *outFolder = folder;
    *outFilename = filename;
    *outFileWOExt = filename_wo_ext;
    *outFileExt = ext;
    
}
*/

bool starts_with(std::string const & value, std::string const & starting)
{
    return (value.rfind(starting, 0) == 0);
}



// MacOS fix importing
#ifdef __APPLE__
#define noexcept
#endif

#include <assimp/config.h>
#include <assimp/cimport.h> // Plain-C interface
#include <assimp/scene.h>   // Output data structure
#include <assimp/postprocess.h> // Post processing flags

unsigned int floatToColor(float c) {
    c = clamp(c, 0.0f, 1.0f);
    
    unsigned int result = (unsigned int) ( c * 255.0f + 0.5f );
    if (result > 255)
        result = 255;
    
    return result;
}




//aiTextureType_DIFFUSE
void processTextureType(Material &material, aiMaterial *aimaterial,  aiTextureType textureType, TextureType ourTextureType) {
    
    for (int j = 0; j < aimaterial->GetTextureCount(textureType); j++) {
        aiString path;
        aiTextureMapping mapping = aiTextureMapping_UV;
        unsigned int uvIndex;
        float blend;
        aiTextureOp texOP = (aiTextureOp)TextureOp_Default;
        aiTextureMapMode mapMode = aiTextureMapMode_Wrap;
        
        fprintf(stdout, "         tex(%i) => \n", j);
        if (aimaterial->GetTexture(textureType, j, &path, &mapping, &uvIndex, &blend, &texOP, &mapMode) == aiReturn_SUCCESS) {
            std::string folder,filename, file_wo_ext, ext;
            PlatformPath::splitPathString(path.data, &folder, &filename, &file_wo_ext, &ext);
            
            if ( mapping == aiTextureMapping_UV ) {
                
                
                fprintf(stdout, "                    file: %s\n", file_wo_ext.c_str());
                fprintf(stdout, "                    ext: %s\n", ext.c_str());
                fprintf(stdout, "                    type: %s\n", TextureTypeToStr(ourTextureType) );
                fprintf(stdout, "                    op: %s\n", TextureOpToStr((TextureOp)texOP) );
                fprintf(stdout, "                    mapMode: %s\n", TextureMapModeToStr((TextureMapMode)mapMode) );
                fprintf(stdout, "                    uvIndex: %i\n", uvIndex);
                
                Texture texture;
                texture.filename = file_wo_ext;
                texture.fileext = ext;
                texture.type = ourTextureType;//TextureType_DIFFUSE;
                texture.op = (TextureOp)texOP;
                texture.uvIndex = uvIndex;
                texture.mapMode = (TextureMapMode)mapMode;
                
                material.textures.push_back(texture);
            } else {
                fprintf(stdout, "                    NOT IMPORTED TEXTURE INFO (%s) -- mapping type only support UV mapping...\n", filename.c_str());
                
            }
        }
        else {
            fprintf(stdout, "                    Error to process...\n");
        }
    }
    
}

void recursiveInsertNodes(ModelContainer *result, aiNode * ainode, std::string output, int parentIndex, bool leftHanded) {
    Node node;
    
    node.name = ainode->mName.data;
    node.transform = transpose( mat4(
  ainode->mTransformation.a1, ainode->mTransformation.b1, ainode->mTransformation.c1, ainode->mTransformation.d1,
  ainode->mTransformation.a2, ainode->mTransformation.b2, ainode->mTransformation.c2, ainode->mTransformation.d2,
  ainode->mTransformation.a3, ainode->mTransformation.b3, ainode->mTransformation.c3, ainode->mTransformation.d3,
  ainode->mTransformation.a4, ainode->mTransformation.b4, ainode->mTransformation.c4, ainode->mTransformation.d4
    ) );
    
    //fix transform
    {
        mat4 m = node.transform;
        
        //vec3 translatevec =toVec3(m * vec4(0,0,0,1)) * vec3(1,1,-1);
        vec3 translatevec =toVec3(m * vec4(0,0,0,1));
        
        vec3 scalevec = aRibeiro::vec3(aRibeiro::length(m[0]),
                                       aRibeiro::length(m[1]),
                                       aRibeiro::length(m[2]));
        
        /*
        printf("%s scalevec %f, %f, %f\n",
               output.c_str(),
               scalevec.x,
               scalevec.y,
               scalevec.z);*/
        
        m = extractRotation(m);
        m[0] = normalize(m[0]);
        m[1] = normalize(m[1]);
        m[2] = normalize(m[2]);
        
        quat rotationquat = extractQuat( m );// * aRibeiro::quatFromEuler(0, 0, DEG2RAD(180.0));
        
        if (parentIndex == -1){
            rotationquat = aRibeiro::quatFromEuler(0, DEG2RAD(180.0f), 0) * rotationquat;
        }
        
        node.transform = translate(translatevec) * toMat4(rotationquat) * scale(scalevec);
        
    }
    
    
    for (int i = 0; i < ainode->mNumMeshes; i++) {
        node.geometries.push_back(ainode->mMeshes[i]);
    }
    
    result->nodes.push_back(node);
    int nodeIndex = result->nodes.size() - 1;
    
    if (parentIndex != -1) {
        result->nodes[parentIndex].children.push_back(nodeIndex);
    }
    
    if (ainode->mNumMeshes > 0) {
        fprintf(stdout, "%s+%s ( meshCount: %i ids: ", output.c_str(), ainode->mName.data, ainode->mNumMeshes);
        for (int i = 0; i < ainode->mNumMeshes; i++) {
            fprintf(stdout, "%i ", ainode->mMeshes[i]);
        }
        fprintf(stdout, ")\n");
    }
    else {
        fprintf(stdout, "%s+%s\n", output.c_str(), ainode->mName.data);
    }
    
    
    {
        vec3 LocalPosition = toVec3(node.transform * aRibeiro::vec4(0,0,0,1));
        
        vec3 LocalScale = aRibeiro::vec3(aRibeiro::length(node.transform[0]),
                                            aRibeiro::length(node.transform[1]),
                                            aRibeiro::length(node.transform[2]));
        
        mat4 m = aRibeiro::extractRotation(node.transform);
        
        m[0] = aRibeiro::normalize(m[0]);
        m[1] = aRibeiro::normalize(m[1]);
        m[2] = aRibeiro::normalize(m[2]);
        
        quat LocalRotation = aRibeiro::extractQuat( m );
        
        printf("%s LocalPosition %f, %f, %f\n",
               output.c_str(),
               LocalPosition.x,
               LocalPosition.y,
               LocalPosition.z);
        
        printf("%s LocalScale %f, %f, %f\n",
               output.c_str(),
               LocalScale.x,
               LocalScale.y,
               LocalScale.z);
        
        printf("%s LocalRotation %f, %f, %f, %f\n",
               output.c_str(),
               LocalRotation.x,
               LocalRotation.y,
               LocalRotation.z,
               LocalRotation.w);
        
    }


    for (int i = 0; i < ainode->mNumChildren; i++)
        recursiveInsertNodes(result, ainode->mChildren[i], output + std::string(" "), nodeIndex, leftHanded);
}


ModelContainer *ImportFromAssimp(const char* filename, bool leftHanded = true) {
    const aiScene* scene;
    aiPropertyStore *store = aiCreatePropertyStore();
    
    // disable the extra FBX nodes generation...
    //aiSetImportPropertyInteger(store, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, AI_FALSE);
    
    //aiSetImportPropertyInteger(store, AI_CONFIG_PP_SLM_VERTEX_LIMIT, AI_SLM_DEFAULT_MAX_VERTICES);

    aiSetImportPropertyInteger(store, AI_CONFIG_PP_SLM_VERTEX_LIMIT, 65536);
    //aiSetImportPropertyInteger(store, AI_SLM_DEFAULT_MAX_VERTICES, AI_SLM_DEFAULT_MAX_TRIANGLES);
    
    //assimp unit is cm... convert it to meters
    aiSetImportPropertyFloat(store, AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, 0.01f);
    

    unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality |
                         aiProcess_CalcTangentSpace
    | aiProcess_GlobalScale
    //| aiProcess_GenNormals
    ;
    // &(~aiProcess_SplitLargeMeshes);
    if (leftHanded)
        flags |= aiProcess_ConvertToLeftHanded;

    scene = aiImportFileExWithProperties(filename, flags, NULL, store);
    aiReleasePropertyStore(store);
    
    if (!scene) {
        fprintf(stderr, "[Loader] ERROR: %s\n", aiGetErrorString());
        exit(-1);
    }
    
    ModelContainer *result = new ModelContainer();
    //
    // MESH EXPORTING
    //
    for (int i = 0; i < scene->mNumMeshes; i++) {
        fprintf(stdout, "[Mesh] %i / %i\n", i + 1, scene->mNumMeshes);
        Geometry geometry;
        //geometry.format = 0;
        
        aiMesh* mesh = scene->mMeshes[i];
        
        fprintf(stdout, "         Name: %s\n", mesh->mName.data);
        geometry.name = mesh->mName.data;
        geometry.materialIndex = mesh->mMaterialIndex;
        
        int primitiveCount = 0;
        //check primitive count
        if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_POINT) {
            fprintf(stdout, "         Contains points\n");
            primitiveCount++;
            geometry.indiceCountPerFace = 1;
        }
        if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_LINE) {
            fprintf(stdout, "         Contains lines\n");
            primitiveCount++;
            geometry.indiceCountPerFace = 2;
        }
        if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) {
            fprintf(stdout, "         Contains triangles\n");
            primitiveCount++;
            geometry.indiceCountPerFace = 3;
        }
        if (mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_POLYGON) {
            fprintf(stderr, "[Loader] ERROR: Polygon import not supported. Try to split into triangles before importe...\n");
            exit(-1);
        }
        
        if (primitiveCount != 1) {
            fprintf(stderr, "[Loader] ERROR: Geometry with mixed faces or no face not supported (points with lines or lines with triangles)...\n");
            exit(-1);
        }
        
        if (mesh->HasPositions()) {
            fprintf(stdout, "         + positions (%i)\n", mesh->mNumVertices);
            geometry.format = geometry.format | VertexFormat::CONTAINS_POS;
        }
        else
        {
            fprintf(stderr, "[Loader] ERROR: Geometry without vertices...\n");
            exit(-1);
        }
        
        if (mesh->HasNormals()) {
            fprintf(stdout, "         + normals\n");
            geometry.format = geometry.format | VertexFormat::CONTAINS_NORMAL;
        }
        if (mesh->HasTangentsAndBitangents()) {
            fprintf(stdout, "         + tangents (binormals need to be derivated)\n");
            //fprintf(stdout, "         + binormals\n");
            geometry.format = geometry.format | VertexFormat::CONTAINS_TANGENT;// | VertexFormat::CONTAINS_BINORMAL;
        }
        
        for (int i = 0; i < 8; i++) {
            if (mesh->HasTextureCoords(i)) {
                fprintf(stdout, "         + uv %i\n", i);
                geometry.format = geometry.format | (VertexFormat::CONTAINS_UV0 << i);
            }
        }
        
        for (int i = 0; i < 8; i++) {
            if (mesh->HasVertexColors(i)) {
                fprintf(stdout, "         + vertex color %i\n", i);
                geometry.format = geometry.format | (VertexFormat::CONTAINS_COLOR0 << i);
            }
        }
        
        //
        // Vertex attrib import
        //
        geometry.vertexCount = mesh->mNumVertices;
        for (int i = 0; i < mesh->mNumVertices; i++) {
            geometry.pos.push_back( vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z) );
            if (mesh->HasNormals())
                geometry.normals.push_back( vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) );
            if (mesh->HasTangentsAndBitangents()) {
                geometry.tangent.push_back(vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
                //geometry.binormal.push_back(vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z));
            }
            for (int j = 0; j < 8; j++) {
                if (mesh->HasTextureCoords(j))
                    geometry.uv[j].push_back(vec3(mesh->mTextureCoords[j][i].x, mesh->mTextureCoords[j][i].y, mesh->mTextureCoords[j][i].z));
                if (mesh->HasVertexColors(j))
                    geometry.color[j].push_back(
                                                vec4(mesh->mColors[j][i].r,
                                                     mesh->mColors[j][i].g,
                                                     mesh->mColors[j][i].b,
                                                     mesh->mColors[j][i].a)
                                                /*
                                                (floatToColor(mesh->mColors[j][i].r) << 24) |
                                                (floatToColor(mesh->mColors[j][i].g) << 16) |
                                                (floatToColor(mesh->mColors[j][i].b) << 8) |
                                                floatToColor(mesh->mColors[j][i].a)
                                                */
                                                );
            }
        }
        
        //
        // Indices import
        //
        if (mesh->HasFaces() && geometry.indiceCountPerFace != 1) {
            for (int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                if (face.mNumIndices != geometry.indiceCountPerFace) {
                    fprintf(stderr, "[Loader] ERROR: Face indice count different from the geometry primitive type...\n");
                    exit(-1);
                }
                for (int i = 0; i < face.mNumIndices; i++) {
                    ARIBEIRO_ABORT(face.mIndices[i] >= 65536, "Max index value error (%u).\n", face.mIndices[i]);
                    geometry.indice.push_back(face.mIndices[i]);
                }
            }
        }
        
        //
        // Bones import
        //
        if (mesh->HasBones()) {
            fprintf(stdout, "         Contains bones\n");
            
            for (int i = 0; i < mesh->mNumBones; i++) {
                aiBone *aibone = mesh->mBones[i];
                
                Bone bone;
                bone.name = aibone->mName.data;

                /*
                bone.offset = transpose( mat4(
                                   aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.d1,
                                   aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.d2,
                                   aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.d3,
                                   aibone->mOffsetMatrix.a4, aibone->mOffsetMatrix.b4, aibone->mOffsetMatrix.c4, aibone->mOffsetMatrix.d4
                                   ) );
                
                //fix transform
                {
                    mat4 m = bone.offset;
                    vec3 translatevec =toVec3(m * vec4(0,0,0,1));
                    vec3 scalevec = aRibeiro::vec3(aRibeiro::length(m[0]),
                                                aRibeiro::length(m[1]),
                                                aRibeiro::length(m[2]));
                    m = extractRotation(m);
                    m[0] = normalize(m[0]);
                    m[1] = normalize(m[1]);
                    m[2] = normalize(m[2]);
                    quat rotationquat = extractQuat( m );
                    bone.offset = translate(translatevec) * toMat4(rotationquat) * scale(scalevec);
                }
                */


                for (int j = 0; j < aibone->mNumWeights; j++) {
                    VertexWeight bw;
                    bw.vertexID = aibone->mWeights[j].mVertexId;
                    bw.weight = aibone->mWeights[j].mWeight;
                    bone.weights.push_back(bw);
                }
                
                geometry.bones.push_back(bone);
            }
        }
        
        if (mesh->mNumAnimMeshes > 0) {
            fprintf(stdout, "         ERROR: AnimMesh not implemented...\n");
        }
        
        
        result->geometries.push_back(geometry);
    }
    
    //
    // ANIMATION EXPORTING
    //
    
    for (int i = 0; i < scene->mNumAnimations; i++) {
        fprintf(stdout, "[Animation] %i / %i\n", i + 1, scene->mNumAnimations);
        aiAnimation *aianimation = scene->mAnimations[i];
        Animation animation;
        
        animation.name = aianimation->mName.data;
        animation.durationTicks = (float)aianimation->mDuration;
        animation.ticksPerSecond = (float)aianimation->mTicksPerSecond;
        if (aianimation->mTicksPerSecond == 0)
            animation.ticksPerSecond = 1.0f;
        
        fprintf(stdout, "            name => %s\n", aianimation->mName.data);
        fprintf(stdout, "            durationTicks => %f\n", animation.durationTicks);
        fprintf(stdout, "            ticksPerSecond => %f\n", animation.ticksPerSecond);
        fprintf(stdout, "            bone channels => %i\n", aianimation->mNumChannels);
        
        //
        // export channels
        //
        
        for(int j=0;j<aianimation->mNumChannels;j++) {
            aiNodeAnim *ainodeanim =aianimation->mChannels[j];
            
            NodeAnimation na;
            na.nodeName = ainodeanim->mNodeName.data;
            na.preState = (AnimBehaviour)ainodeanim->mPreState;
            na.postState = (AnimBehaviour)ainodeanim->mPostState;
            
            for(int k =0; k< ainodeanim->mNumPositionKeys; k++){
                aiVectorKey aivectorkey = ainodeanim->mPositionKeys[k];
                
                Vec3Key vec3key;
                vec3key.time = (float)aivectorkey.mTime;
                vec3key.value = vec3(aivectorkey.mValue.x,aivectorkey.mValue.y,aivectorkey.mValue.z);
                
                na.positionKeys.push_back(vec3key);
            }
            
            for(int k =0; k< ainodeanim->mNumScalingKeys; k++){
                aiVectorKey aivectorkey = ainodeanim->mScalingKeys[k];
                
                Vec3Key vec3key;
                vec3key.time = (float)aivectorkey.mTime;
                vec3key.value = vec3(aivectorkey.mValue.x,aivectorkey.mValue.y,aivectorkey.mValue.z);
                
                na.scalingKeys.push_back(vec3key);
            }
            
            
            for(int k =0; k< ainodeanim->mNumRotationKeys; k++){
                aiQuatKey aiquatkey = ainodeanim->mRotationKeys[k];
                
                QuatKey quatkey;
                quatkey.time = (float)aiquatkey.mTime;
                quatkey.value = quat(aiquatkey.mValue.x,
                                     aiquatkey.mValue.y,
                                     aiquatkey.mValue.z,
                                     aiquatkey.mValue.w);
                
                na.rotationKeys.push_back(quatkey);
            }
            
            animation.channels.push_back(na);
            
        }
        
        
        if (aianimation->mNumMeshChannels > 0) {
            fprintf(stdout, "         ERROR: MeshChannels not implemented...\n");
        }
        if (aianimation->mNumMorphMeshChannels > 0) {
            fprintf(stdout, "         ERROR: MorphMeshChannels not implemented...\n");
        }
        
        
        result->animations.push_back(animation);
    }
    
    //
    // CAMERA EXPORTING
    //
    
    for (int i = 0; i < scene->mNumCameras; i++) {
        fprintf(stdout, "[Camera] %i / %i\n", i + 1, scene->mNumCameras);
        
        
        aiCamera *aicamera = scene->mCameras[i];
        
        Camera camera;
        
        camera.name = aicamera->mName.data;
        fprintf(stderr, "         name: %s\n", aicamera->mName.data);
        
        camera.pos = vec3( aicamera->mPosition.x, aicamera->mPosition.y, aicamera->mPosition.z );
        camera.up = vec3( aicamera->mUp.x, aicamera->mUp.y, aicamera->mUp.z );
        camera.forward = vec3( aicamera->mLookAt.x, aicamera->mLookAt.y, aicamera->mLookAt.z );
        
        camera.horizontalFOVrad = aicamera->mHorizontalFOV;
        camera.nearPlane = aicamera->mClipPlaneNear;
        camera.farPlane = aicamera->mClipPlaneFar;
        camera.aspect = aicamera->mAspect;
        
        if (camera.aspect == 0.0f) {
            camera.aspect = 1.0f;
            camera.verticalFOVrad = camera.horizontalFOVrad;
        } else
            camera.verticalFOVrad = camera.horizontalFOVrad / camera.aspect;
        
        fprintf(stderr, "         pos: %f, %f, %f\n", camera.pos.x, camera.pos.y, camera.pos.z);
        fprintf(stderr, "         up: %f, %f, %f\n", camera.up.x, camera.up.y, camera.up.z);
        fprintf(stderr, "         forward: %f, %f, %f\n", camera.forward.x, camera.forward.y, camera.forward.z);
        fprintf(stderr, "         nearPlane: %f\n", camera.nearPlane);
        fprintf(stderr, "         farPlane: %f\n", camera.farPlane);
        fprintf(stderr, "         aspect: %f\n", camera.aspect);
        fprintf(stderr, "         horizontalFOV: %f\n", RAD2DEG(camera.horizontalFOVrad));
        fprintf(stderr, "         verticalFOV: %f\n", RAD2DEG(camera.verticalFOVrad));
        
        
        result->cameras.push_back(camera);
        
    }
    
    //
    // LIGHT EXPORTING
    //
    
    for (int i = 0; i < scene->mNumLights; i++) {
        fprintf(stdout, "[Light] %i / %i\n", i + 1, scene->mNumLights);
        
        aiLight *ailight = scene->mLights[i];
        
        Light light;
        
        light.name = ailight->mName.data;
        
        fprintf(stdout, "        name => %s\n", ailight->mName.data);
        
        light.type = (LightType)ailight->mType;
        
        //common attrs
        light.attenuationConstant = ailight->mAttenuationConstant;
        light.attenuationLinear = ailight->mAttenuationLinear;
        light.attenuationQuadratic = ailight->mAttenuationQuadratic;
        
        fprintf(stdout, "        attenuationConstant => %f\n", light.attenuationConstant);
        fprintf(stdout, "        attenuationLinear => %f\n", light.attenuationLinear);
        fprintf(stdout, "        attenuationQuadratic => %f\n", light.attenuationQuadratic);
        
        light.colorDiffuse = vec3( ailight->mColorDiffuse.r, ailight->mColorDiffuse.g, ailight->mColorDiffuse.b );
        light.colorSpecular = vec3( ailight->mColorSpecular.r, ailight->mColorSpecular.g, ailight->mColorSpecular.b );
        light.colorAmbient = vec3( ailight->mColorAmbient.r, ailight->mColorAmbient.g, ailight->mColorAmbient.b );
        
        fprintf(stdout, "        colorDiffuse => vec3( %f, %f, %f )\n", light.colorDiffuse.r, light.colorDiffuse.g, light.colorDiffuse.b);
        fprintf(stdout, "        colorSpecular => vec3( %f, %f, %f )\n", light.colorSpecular.r, light.colorSpecular.g, light.colorSpecular.b);
        fprintf(stdout, "        colorAmbient => vec3( %f, %f, %f )\n", light.colorAmbient.r, light.colorAmbient.g, light.colorAmbient.b);
        
        fprintf(stdout, "        type => %s\n", LightTypeToStr(light.type) );
        
        switch(light.type) {
            case LightType_NONE:
                break;
            case LightType_DIRECTIONAL:
                light.directional.direction = vec3( ailight->mDirection.x, ailight->mDirection.y, ailight->mDirection.z );
                light.directional.up = vec3( ailight->mUp.x, ailight->mUp.y, ailight->mUp.z );
                
                fprintf(stdout, "        direction => vec3( %f, %f, %f )\n", light.directional.direction.x, light.directional.direction.y, light.directional.direction.z);
                fprintf(stdout, "        up => vec3( %f, %f, %f )\n", light.directional.up.x, light.directional.up.y, light.directional.up.z);
                
                break;
            case LightType_POINT:
                light.point.position = vec3( ailight->mPosition.x, ailight->mPosition.y, ailight->mPosition.z );
                
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.point.position.x, light.point.position.y, light.point.position.z);
                
                break;
            case LightType_SPOT:
                light.spot.position = vec3( ailight->mPosition.x, ailight->mPosition.y, ailight->mPosition.z );
                light.spot.direction = vec3( ailight->mDirection.x, ailight->mDirection.y, ailight->mDirection.z );
                light.spot.up = vec3( ailight->mUp.x, ailight->mUp.y, ailight->mUp.z );
                light.spot.angleInnerCone = ailight->mAngleInnerCone;
                light.spot.angleOuterCone = ailight->mAngleOuterCone;
                
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.spot.position.x, light.spot.position.y, light.spot.position.z);
                fprintf(stdout, "        direction => vec3( %f, %f, %f )\n", light.spot.direction.x, light.spot.direction.y, light.spot.direction.z);
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.spot.up.x, light.spot.up.y, light.spot.up.z);
                fprintf(stdout, "        angleInnerCone => %f\n", light.spot.angleInnerCone);
                fprintf(stdout, "        angleOuterCone => %f\n", light.spot.angleOuterCone);
                
                break;
            case LightType_AMBIENT:
                light.ambient.position = vec3( ailight->mPosition.x, ailight->mPosition.y, ailight->mPosition.z );
                light.ambient.direction = vec3( ailight->mDirection.x, ailight->mDirection.y, ailight->mDirection.z );
                light.ambient.up = vec3( ailight->mUp.x, ailight->mUp.y, ailight->mUp.z );
                
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.ambient.position.x, light.ambient.position.y, light.ambient.position.z);
                fprintf(stdout, "        direction => vec3( %f, %f, %f )\n", light.ambient.direction.x, light.ambient.direction.y, light.ambient.direction.z);
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.ambient.up.x, light.ambient.up.y, light.ambient.up.z);
                
                break;
            case LightType_AREA:
                light.area.position = vec3( ailight->mPosition.x, ailight->mPosition.y, ailight->mPosition.z );
                light.area.direction = vec3( ailight->mDirection.x, ailight->mDirection.y, ailight->mDirection.z );
                light.area.up = vec3( ailight->mUp.x, ailight->mUp.y, ailight->mUp.z );
                light.area.size = vec2( ailight->mSize.x, ailight->mSize.y );
                
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.area.position.x, light.area.position.y, light.area.position.z);
                fprintf(stdout, "        direction => vec3( %f, %f, %f )\n", light.area.direction.x, light.area.direction.y, light.area.direction.z);
                fprintf(stdout, "        position => vec3( %f, %f, %f )\n", light.area.up.x, light.area.up.y, light.area.up.z);
                fprintf(stdout, "        size => vec3( %f, %f )\n", light.area.size.x, light.area.size.y);
                
                break;
        }
        
        result->lights.push_back(light);
    }
    
    //
    // TEXTURE EXPORTING
    //
    
    for (int i = 0; i < scene->mNumTextures; i++) {
        fprintf(stdout, "[Texture] %i / %i\n", i + 1, scene->mNumTextures);
        fprintf(stderr, "[Texture] not implemented...\n");
    }
    
    //
    // MATERIAL EXPORTING
    //
    
    for (int i = 0; i < scene->mNumMaterials; i++) {
        fprintf(stdout, "[Material] %i / %i\n", i + 1, scene->mNumMaterials);
        aiMaterial* aimaterial = scene->mMaterials[i];
        
        Material material;
        
        for (int j = 0; j < aimaterial->mNumProperties; j++) {
            aiMaterialProperty *materialProperty = aimaterial->mProperties[j];
            
            if (starts_with(materialProperty->mKey.data, "$tex.")) {
                fprintf(stdout, "         %s => processed ahead...\n", materialProperty->mKey.data);
                continue;
            }
            else if (starts_with(materialProperty->mKey.data, "$raw.")) {
                fprintf(stdout, "         %s => not processed...\n", materialProperty->mKey.data);
                continue;
            }
            else if (materialProperty->mType == aiPTI_String) {
                aiString str;
                if (aimaterial->Get(materialProperty->mKey.data, materialProperty->mSemantic, materialProperty->mIndex, str) == aiReturn_SUCCESS) {
                    fprintf(stdout, "         %s => %s\n", materialProperty->mKey.data, str.data);
                    if (strcmp(materialProperty->mKey.data, "?mat.name") == 0) {
                        material.name = str.data;
                        fprintf(stdout, "               storing material name\n");
                    } else {
                        fprintf(stdout, "               not stored\n");
                    }
                } else {
                    fprintf(stdout, "               ERROR TO READ VALUE...\n");
                }
            }
            else if (materialProperty->mType == aiPTI_Float) {
                ai_real floats[16];
                unsigned int iMax = 16;
                
                aiReturn eRet = aiGetMaterialFloatArray(aimaterial, materialProperty->mKey.data, materialProperty->mSemantic, materialProperty->mIndex, floats, &iMax);
                
                if (eRet == aiReturn_SUCCESS) {
                    //color 3 or 4 floats check
                    if (iMax == 3 && (starts_with(materialProperty->mKey.data, "$clr."))) {
                        iMax = 4;
                        floats[3] = 1.0f;
                    }
                    
                    if (iMax == 1) {
                        float v = floats[0];
                        fprintf(stdout, "         %s => float (%f)\n", materialProperty->mKey.data, v);
                        if (starts_with(materialProperty->mKey.data, "$mat.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$mat.")]);
                            material.floatValue[&materialProperty->mKey.data[strlen("$mat.")]] = v;
                        } else
                            material.floatValue[materialProperty->mKey.data] = v;
                    }
                    else if (iMax == 2) {
                        vec2 v(floats[0], floats[1]);
                        fprintf(stdout, "         %s => vec2 (%f, %f)\n", materialProperty->mKey.data, v.x, v.y);
                        if (starts_with(materialProperty->mKey.data, "$mat.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$mat.")]);
                            material.vec2Value[&materialProperty->mKey.data[strlen("$mat.")]] = v;
                        }
                        else
                            material.vec2Value[materialProperty->mKey.data] = v;
                    }
                    else if (iMax == 3) {
                        vec3 v(floats[0], floats[1], floats[2]);
                        fprintf(stdout, "         %s => vec3 (%f, %f, %f)\n", materialProperty->mKey.data, v.x, v.y, v.z);
                        if (starts_with(materialProperty->mKey.data, "$mat.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$mat.")]);
                            material.vec3Value[&materialProperty->mKey.data[strlen("$mat.")]] = v;
                        }
                        else
                            material.vec3Value[materialProperty->mKey.data] = v;
                    }
                    else if (iMax == 4) {
                        vec4 v(floats[0], floats[1], floats[2], floats[3]);
                        fprintf(stdout, "         %s => vec4 (%f, %f, %f, %f)\n", materialProperty->mKey.data, v.x, v.y, v.z, v.w);
                        if (starts_with(materialProperty->mKey.data, "$mat.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$mat.")]);
                            material.vec4Value[&materialProperty->mKey.data[strlen("$mat.")]] = v;
                        }
                        else if (starts_with(materialProperty->mKey.data, "$clr.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$clr.")]);
                            material.vec4Value[&materialProperty->mKey.data[strlen("$clr.")]] = v;
                        }
                        else
                            material.vec4Value[materialProperty->mKey.data] = v;
                    }
                    else if (iMax == 16) {
                        /*
                         mat4 m = mat4(
                         floats[0], floats[4], floats[8], floats[12],
                         floats[1], floats[5], floats[9], floats[13],
                         floats[2], floats[6], floats[10], floats[14],
                         floats[3], floats[7], floats[11], floats[15]
                         );
                         */
                        fprintf(stdout, "         %s => mat4 not implemented (...)\n", materialProperty->mKey.data);
                    }
                    else {
                        fprintf(stdout, "         %s => float(%i) not implemented...\n", materialProperty->mKey.data, iMax);
                    }
                    
                }
                else {
                    fprintf(stdout, "         %s => ERROR TO READ VALUE...\n", materialProperty->mKey.data);
                }
            }
            else if (materialProperty->mType == aiPTI_Integer)
            {
                int integers[16];
                unsigned int iMax = 16;
                
                aiReturn eRet = aiGetMaterialIntegerArray(aimaterial, materialProperty->mKey.data, materialProperty->mSemantic, materialProperty->mIndex, integers, &iMax);
                if (eRet == aiReturn_SUCCESS) {
                    if (iMax == 1) {
                        int v = integers[0];
                        fprintf(stdout, "         %s => int (%i) \n", materialProperty->mKey.data, v);
                        
                        if (starts_with(materialProperty->mKey.data, "$mat.")) {
                            fprintf(stdout, "               storing name: %s\n", &materialProperty->mKey.data[strlen("$mat.")]);
                            material.intValue[&materialProperty->mKey.data[strlen("$mat.")]] = v;
                        }
                        else
                            material.intValue[materialProperty->mKey.data] = v;
                    }
                    else {
                        fprintf(stdout, "         %s => int(%i) not implemented...\n", materialProperty->mKey.data, iMax);
                    }
                }
                else {
                    fprintf(stdout, "         %s => ERROR TO READ VALUE...\n", materialProperty->mKey.data);
                }
            }
            
            else {
                fprintf(stdout, "         %s => not processed...\n", materialProperty->mKey.data);
            }
        }
        
        processTextureType( material, aimaterial, aiTextureType_DIFFUSE     , TextureType_DIFFUSE     );
        processTextureType( material, aimaterial, aiTextureType_SPECULAR    , TextureType_SPECULAR    );
        processTextureType( material, aimaterial, aiTextureType_AMBIENT     , TextureType_AMBIENT     );
        processTextureType( material, aimaterial, aiTextureType_EMISSIVE    , TextureType_EMISSIVE    );
        processTextureType( material, aimaterial, aiTextureType_HEIGHT      , TextureType_HEIGHT      );
        processTextureType( material, aimaterial, aiTextureType_NORMALS     , TextureType_NORMALS     );
        processTextureType( material, aimaterial, aiTextureType_SHININESS   , TextureType_SHININESS   );
        processTextureType( material, aimaterial, aiTextureType_OPACITY     , TextureType_OPACITY     );
        processTextureType( material, aimaterial, aiTextureType_DISPLACEMENT, TextureType_DISPLACEMENT);
        processTextureType( material, aimaterial, aiTextureType_LIGHTMAP    , TextureType_LIGHTMAP    );
        processTextureType( material, aimaterial, aiTextureType_REFLECTION  , TextureType_REFLECTION  );
        processTextureType( material, aimaterial, aiTextureType_UNKNOWN     , TextureType_UNKNOWN     );
        
        result->materials.push_back(material);
    }
    
    //
    // NODES EXPORTING (Hierarchy)
    //
    
    fprintf(stdout, "[Hierarchy]\n");
    aiNode * node = scene->mRootNode;
    std::string output = " ";
    recursiveInsertNodes(result, node, output, -1,leftHanded);
    
    aiReleaseImport(scene);
    
    return result;
}





#endif
