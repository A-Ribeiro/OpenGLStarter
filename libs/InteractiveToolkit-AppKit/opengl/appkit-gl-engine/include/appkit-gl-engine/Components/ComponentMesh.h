#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>

#include <InteractiveToolkit-Extension/model/Bone.h>

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentMesh : public Component
            {

                AppKit::OpenGL::GLVertexBufferObject *vbo_pos;
                AppKit::OpenGL::GLVertexBufferObject *vbo_normals;
                AppKit::OpenGL::GLVertexBufferObject *vbo_tangent;
                AppKit::OpenGL::GLVertexBufferObject *vbo_binormal;
                AppKit::OpenGL::GLVertexBufferObject *vbo_uv[8];
                AppKit::OpenGL::GLVertexBufferObject *vbo_color[8];

                // skin data
                AppKit::OpenGL::GLVertexBufferObject *vbo_skin_index;
                AppKit::OpenGL::GLVertexBufferObject *vbo_skin_weights;

                AppKit::OpenGL::GLVertexBufferObject *vbo_index;

                int vbo_indexCount;

                void allocateVBO();

                void uploadVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload);

            public:
                static const ComponentType Type;

                // Model::VertexFormat
                uint32_t format;

                std::vector<MathCore::vec3f> pos;
                std::vector<MathCore::vec3f> normals;
                std::vector<MathCore::vec3f> tangent;
                std::vector<MathCore::vec3f> binormal;
                std::vector<MathCore::vec3f> uv[8];
                std::vector<MathCore::vec4f> color[8];
                // std::vector<uint32_t> color[8];//RGBA

                std::vector<uint32_t> indices;

                // store the structure for vertex skinning
                std::vector<ITKExtension::Model::Bone> bones;
                std::vector<MathCore::vec4f> skin_index;
                std::vector<MathCore::vec4f> skin_weights;

                void ComputeFormat();

                ComponentMesh();

                ~ComponentMesh();

                void syncVBOStatic();

                void syncVBODynamic();

                void syncVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload);

                void setLayoutPointers(const DefaultEngineShader *shader);

                void draw();

                void unsetLayoutPointers(const DefaultEngineShader *shader);

                //
                // Another constructor
                //
                static ComponentMesh *createTriangle();

                static ComponentMesh *createPlaneXZ(float _width, float _height);

                static ComponentMesh *createPlaneXY(float _width, float _height);

                static ComponentMesh *createBox(const MathCore::vec3f &dimension);

                static ComponentMesh *createSphere(float radius, int sectorCount, int stackCount);
            };

        }
    }

}