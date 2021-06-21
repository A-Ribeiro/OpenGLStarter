#ifndef shader_manager__H
#define shader_manager__H

#include "shaders/detail.h"
#include "shaders/filters.h"
#include "shaders/ilumination.h"
#include "shaders/radiometric.h"
#include "shaders/texture.h"

#include <vector>
#include <map>

struct _SSE2_ALIGN_PRE VertexAttrib {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    
    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;

class RenderSystem;

#include "common/RenderSystem.h"

class ShaderManager {

    std::vector<std::string> groups;
    std::map< std::string, std::vector<std::string> > insideGroup;

    std::map<std::string, GLShader *> shaders;
    std::map<std::string, void (ShaderManager::*)(GLShader *, GLint , const VertexAttrib *, int ) > shaders_draw_map;
    std::map<std::string, bool> shaders_is_post_processing;

    RenderSystem *render;


    void draw_GLShaderTextureColor(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderBumpMapping(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderBumpMappingWood(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);

    void draw_ShaderConeStepMappingOriginal(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderConeStepMappingOriginalWood(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);

    void draw_ShaderLaplace(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderBlur(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderBlinPhongVertex(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_BlinPhongPixel(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderBrightnessContrast(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderGrayScaleInternet(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderGrayScaleHumanVisualSystem(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderGrayScaleHueSaturationValue(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderTwoTextures(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);
    void draw_ShaderProceduralTexture(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount);

public:
    std::string activeShaderName;
    GLShader *activeShader;

    //
    //aux textures
    //
    GLTexture *wood;
    GLTexture *woodBumpAndCone;


    //
    // shader parameters
    //

    //GLShaderTextureColor
    GLTexture *defaultTexture;
    //vec3 objectColor;

    //ShaderBumpMapping
    GLTexture *diffuse;
    GLTexture *bumpmap;
    vec3 lightPosition;
    vec3 lightAmbient;
    vec3 lightDiffuse;
    vec3 lightSpecular;
    float lightShine;
    float normalStrength;

    //ShaderConeStepMappingOriginal
    //GLTexture *diffuse;
    //GLTexture *bumpmap;
    GLTexture *conemap;
    float conedepth;
    //vec3 lightPosition;
    //vec3 lightAmbient;
    //vec3 lightDiffuse;
    //vec3 lightSpecular;
    //float lightShine;

    //ShaderLaplace
    //GLTexture *framebuffer;
    GLFramebufferObject *framebuffer;
    GLFramebufferObject *framebufferBlur;
    vec2 framebufferNeighbor;
    float laplaceIntensity;
    float laplaceBlend;

    //ShaderBlur
    //GLTexture *framebuffer;
    //vec2 framebufferNeighbor;
    int blurSteps;

    //ShaderBlinPhongVertex
    //GLTexture *diffuse;
    vec3 objectColor;
    //vec3 lightPosition;
    //vec3 lightAmbient;
    //vec3 lightDiffuse;
    //vec3 lightSpecular;
    //float lightShine;

    //BlinPhongPixel
    //GLTexture *diffuse;
    //vec3 objectColor;
    //vec3 lightPosition;
    //vec3 lightAmbient;
    //vec3 lightDiffuse;
    //vec3 lightSpecular;
    //float lightShine;

    //ShaderBrightnessContrast
    //GLTexture *framebuffer;
    float brightness;
    float contrast;

    //ShaderGrayScaleInternet
    //GLTexture *framebuffer;

    //ShaderGrayScaleHumanVisualSystem
    //GLTexture *framebuffer;

    //ShaderGrayScaleHueSaturationValue
    //GLTexture *framebuffer;

    //ShaderTwoTextures
    //GLTexture *diffuse;
    GLTexture *diffuseB;
    float blend;

    //ShaderProceduralTexture
    float frequency;


    ShaderManager(RenderSystem *renderSystem);

    ~ShaderManager();

    std::vector<std::string> &getGroupList();

    std::vector<std::string> &getInsideGroupList(const char* group);

    void setActiveShader(const std::string &shader);

    bool isPostProcessing(const std::string &shader);

    //
    // all parameters
    //

    void setupShaderParametersAndDraw(GLint ogl_primitive, const VertexAttrib *buffer, int vertexCount);

    SSE2_CLASS_NEW_OPERATOR
};

#endif
