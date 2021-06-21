#ifndef _ComponentMesh_h_
#define _ComponentMesh_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentColorMesh.h>
#include <data-model/data-model.h>

#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {
    
    namespace Components {
        
        class ComponentMesh: public Component {
            
            openglWrapper::GLVertexBufferObject *vbo_pos;
            openglWrapper::GLVertexBufferObject *vbo_normals;
            openglWrapper::GLVertexBufferObject *vbo_tangent;
            openglWrapper::GLVertexBufferObject *vbo_binormal;
            openglWrapper::GLVertexBufferObject *vbo_uv[8];
            openglWrapper::GLVertexBufferObject *vbo_color[8];
            
            //skin data
            openglWrapper::GLVertexBufferObject *vbo_skin_index;
            openglWrapper::GLVertexBufferObject *vbo_skin_weights;

            openglWrapper::GLVertexBufferObject *vbo_index;

            int vbo_indexCount;
            
            
            void allocateVBO();
            
            void uploadVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload);
            
        public:
            
            static const ComponentType Type;
            
            //Model::VertexFormat
            uint32_t format;
            
            aRibeiro::aligned_vector<aRibeiro::vec3> pos;
            aRibeiro::aligned_vector<aRibeiro::vec3> normals;
            aRibeiro::aligned_vector<aRibeiro::vec3> tangent;
            aRibeiro::aligned_vector<aRibeiro::vec3> binormal;
            aRibeiro::aligned_vector<aRibeiro::vec3> uv[8];
            aRibeiro::aligned_vector<aRibeiro::vec4> color[8];
            //std::vector<uint32_t> color[8];//RGBA
            
            std::vector<uint16_t> indices;

            // store the structure for vertex skinning
            aRibeiro::aligned_vector<model::Bone> bones;
            aRibeiro::aligned_vector<aRibeiro::vec4> skin_index;
            aRibeiro::aligned_vector<aRibeiro::vec4> skin_weights;
            
            void ComputeFormat();
            
            ComponentMesh();
            
            virtual ~ComponentMesh();
            
            void syncVBOStatic();
            
            void syncVBODynamic();

            void syncVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload);
            
            void setLayoutPointers(const DefaultEngineShader *shader);
            
            void draw();
            
            void unsetLayoutPointers(const DefaultEngineShader *shader);
            
            //
            // Another constructor
            //
            static ComponentMesh* createTriangle();
            
            static ComponentMesh* createPlaneXZ(float _width, float _height);
            
            static ComponentMesh* createPlaneXY(float _width, float _height);
            
            static ComponentMesh* createBox(const aRibeiro::vec3 &dimension);
            
            static ComponentMesh* createSphere(float radius, int sectorCount, int stackCount);
            
        };
        
    }
}

#endif
