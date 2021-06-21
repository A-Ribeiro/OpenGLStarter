#include "ShaderManager.h"
#include "common/RenderSystem.h"

ShaderManager::ShaderManager(RenderSystem *renderSystem) {

    render = renderSystem;

    groups.push_back("detail");
    groups.push_back("filters");
    groups.push_back("ilumination");
    groups.push_back("radiometric");
    groups.push_back("texture");

    insideGroup["detail"].push_back("BumpMappingRock");
    insideGroup["detail"].push_back("ConeStepMappingRock");
    insideGroup["detail"].push_back("BumpMappingWood");
    insideGroup["detail"].push_back("ConeStepMappingWood");

    insideGroup["filters"].push_back("Laplace");
    insideGroup["filters"].push_back("Blur");

    //insideGroup["ilumination"].push_back("GLShaderTextureColor");
    insideGroup["ilumination"].push_back("BlinPhongVertex");
    insideGroup["ilumination"].push_back("BlinPhongPixel");

    insideGroup["radiometric"].push_back("BrightnessContrast");
    insideGroup["radiometric"].push_back("GrayScaleInternet");
    insideGroup["radiometric"].push_back("GrayScale\nHumanVisualSystem");
    insideGroup["radiometric"].push_back("GrayScale\nHueSaturationValue");
    insideGroup["texture"].push_back("TwoTextures");
    insideGroup["texture"].push_back("ProceduralTexture");

    //
    // shader to draw primitives with just a texture
    //
    shaders["GLShaderTextureColor"] = new GLShaderTextureColor();
    shaders_draw_map["GLShaderTextureColor"] = &ShaderManager::draw_GLShaderTextureColor;
    shaders_is_post_processing["GLShaderTextureColor"] = false;

    //detail
    shaders["BumpMappingRock"] = new ShaderBumpMapping();
    shaders_draw_map["BumpMappingRock"] = &ShaderManager::draw_ShaderBumpMapping;
    shaders_is_post_processing["BumpMappingRock"] = false;

    shaders["BumpMappingWood"] = new ShaderBumpMapping();
    shaders_draw_map["BumpMappingWood"] = &ShaderManager::draw_ShaderBumpMappingWood;
    shaders_is_post_processing["BumpMappingWood"] = false;

    shaders["ConeStepMappingRock"] = new ShaderConeStepMappingOriginal();
    shaders_draw_map["ConeStepMappingRock"] = &ShaderManager::draw_ShaderConeStepMappingOriginal;
    shaders_is_post_processing["ConeStepMappingRock"] = false;

    shaders["ConeStepMappingWood"] = new ShaderConeStepMappingOriginal();
    shaders_draw_map["ConeStepMappingWood"] = &ShaderManager::draw_ShaderConeStepMappingOriginalWood;
    shaders_is_post_processing["ConeStepMappingWood"] = false;

    //filter
    shaders["Laplace"] = new ShaderLaplace();
    shaders_draw_map["Laplace"] = &ShaderManager::draw_ShaderLaplace;
    shaders_is_post_processing["Laplace"] = true;

    shaders["Blur"] = new ShaderBlur();
    shaders_draw_map["Blur"] = &ShaderManager::draw_ShaderBlur;
    shaders_is_post_processing["Blur"] = true;

    //ilumination
    shaders["BlinPhongVertex"] = new ShaderBlinPhongVertex();
    shaders_draw_map["BlinPhongVertex"] = &ShaderManager::draw_ShaderBlinPhongVertex;
    shaders_is_post_processing["BlinPhongVertex"] = false;

    shaders["BlinPhongPixel"] = new ShaderBlinPhongPixel();
    shaders_draw_map["BlinPhongPixel"] = &ShaderManager::draw_BlinPhongPixel;
    shaders_is_post_processing["BlinPhongPixel"] = false;

    //radiometric
    shaders["BrightnessContrast"] = new ShaderBrightnessContrast();
    shaders_draw_map["BrightnessContrast"] = &ShaderManager::draw_ShaderBrightnessContrast;
    shaders_is_post_processing["BrightnessContrast"] = true;

    shaders["GrayScaleInternet"] = new ShaderGrayScaleInternet();
    shaders_draw_map["GrayScaleInternet"] = &ShaderManager::draw_ShaderGrayScaleInternet;
    shaders_is_post_processing["GrayScaleInternet"] = true;

    shaders["GrayScale\nHumanVisualSystem"] = new ShaderGrayScaleHumanVisualSystem();
    shaders_draw_map["GrayScale\nHumanVisualSystem"] = &ShaderManager::draw_ShaderGrayScaleHumanVisualSystem;
    shaders_is_post_processing["GrayScale\nHumanVisualSystem"] = true;

    shaders["GrayScale\nHueSaturationValue"] = new ShaderGrayScaleHueSaturationValue();
    shaders_draw_map["GrayScale\nHueSaturationValue"] = &ShaderManager::draw_ShaderGrayScaleHueSaturationValue;
    shaders_is_post_processing["GrayScale\nHueSaturationValue"] = true;

    //texture
    shaders["TwoTextures"] = new ShaderTwoTextures();
    shaders_draw_map["TwoTextures"] = &ShaderManager::draw_ShaderTwoTextures;
    shaders_is_post_processing["TwoTextures"] = false;

    shaders["ProceduralTexture"] = new ShaderProceduralTexture();
    shaders_draw_map["ProceduralTexture"] = &ShaderManager::draw_ShaderProceduralTexture;
    shaders_is_post_processing["ProceduralTexture"] = false;

    //
    // Initialize Parameters
    //

    wood = GLTexture::loadFromFile("resources/images/wood.png");
    woodBumpAndCone = GLTexture::loadFromFile("resources/images/wood+bump+cone.png");

    defaultTexture = GLTexture::loadFromFile("resources/images/jesus1.png");

    diffuse = GLTexture::loadFromFile("resources/images/rock.png");
    bumpmap = GLTexture::loadFromFile("resources/images/rock+bump+cone.png");

    lightPosition = vec3(0.0f, 0.0f, 1.5f);
    lightAmbient = vec3(0.2f, 0.2f, 0.2f);
    lightDiffuse = vec3(0.7f, 0.7f, 0.9f);
    lightSpecular = vec3(0.4f, 0.7f, 0.4f);
    lightShine = 256;// 32;//256

    conemap = GLTexture::loadFromFile("resources/images/rock+bump+cone.png");
    conedepth = 0.5f;

    //framebuffer = new GLTexture();

    framebuffer = new GLFramebufferObject();
    //framebuffer->depth = new GLTexture();
    framebuffer->useRenderbufferDepth = true;
    framebuffer->color.push_back(new GLTexture());
    framebuffer->setSize(32,32);
    framebuffer->attachTextures();

    framebufferBlur = new GLFramebufferObject();
    //framebufferBlur->depth = new GLTexture();
    framebufferBlur->color.push_back(new GLTexture());
    framebufferBlur->setSize(32,32);
    framebufferBlur->attachTextures();

    framebufferNeighbor = vec2(1);
    laplaceIntensity = 1.0f;
    laplaceBlend = 1.0f;
    blurSteps = 3;
    objectColor = vec3(1.0f);
    brightness = 0.5f;
    contrast = 0.5f;
    diffuseB = GLTexture::loadFromFile("resources/images/jesus2.png");
    blend = 0.5f;

    frequency = 0.5f;

    normalStrength = 1.0f;

}

ShaderManager::~ShaderManager() {

    GLShader::disable();

    for (std::map<std::string, GLShader *>::iterator it = shaders.begin(); it != shaders.end(); ++it)
        setNullAndDelete(it->second);

    shaders.clear();
    insideGroup.clear();
    groups.clear();

    setNullAndDelete(framebuffer->color[0]);
    setNullAndDelete(framebuffer->depth);
    setNullAndDelete(framebuffer);

    setNullAndDelete(framebufferBlur->color[0]);
    setNullAndDelete(framebufferBlur->depth);
    setNullAndDelete(framebufferBlur);


    setNullAndDelete(defaultTexture);
    setNullAndDelete(diffuse);
    setNullAndDelete(bumpmap);
    setNullAndDelete(conemap);
    setNullAndDelete(diffuseB);

    setNullAndDelete(wood);
    setNullAndDelete(woodBumpAndCone);

}

std::vector<std::string> &ShaderManager::getGroupList() {
    return groups;
}

std::vector<std::string> &ShaderManager::getInsideGroupList(const char* group) {
    return insideGroup[group];
}

void ShaderManager::setActiveShader(const std::string &shader) {
    activeShaderName = shader;
    activeShader = shaders[shader];

    activeShader->enable();
}

bool ShaderManager::isPostProcessing(const std::string &shader) {
    return shaders_is_post_processing[shader];
}

void ShaderManager::draw_GLShaderTextureColor(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    GLShaderTextureColor *shader = (GLShaderTextureColor*)baseshader;

    shader->enable();

    defaultTexture->active(0);
    shader->setTexture(0);

    shader->setColor(vec4(objectColor,1.0f));

    shader->setMatrix(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glVertexAttribPointer(shader->vPosition, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->vUV));
    OPENGL_CMD(glVertexAttribPointer(shader->vUV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glDisableVertexAttribArray(shader->vUV));

    defaultTexture->deactive(0);
}

void ShaderManager::draw_ShaderBumpMapping(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderBumpMapping *shader = (ShaderBumpMapping*)baseshader;

    shader->enable();

    diffuse->active(0);
    shader->setDiffuse(0);

    bumpmap->active(1);
    shader->setBumpMap(1);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);
    shader->setStrength(normalStrength);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Tangent, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].tangent));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Binormal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Binormal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].binormal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Binormal));

    bumpmap->deactive(1);
    diffuse->deactive(0);
}



void ShaderManager::draw_ShaderBumpMappingWood(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderBumpMapping *shader = (ShaderBumpMapping*)baseshader;

    shader->enable();

    wood->active(0);
    shader->setDiffuse(0);

    woodBumpAndCone->active(1);
    shader->setBumpMap(1);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);
    shader->setStrength(normalStrength);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Tangent, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].tangent));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Binormal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Binormal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].binormal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Binormal));

    woodBumpAndCone->deactive(1);
    wood->deactive(0);
}

void ShaderManager::draw_ShaderConeStepMappingOriginalWood(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderConeStepMappingOriginal *shader = (ShaderConeStepMappingOriginal*)baseshader;

    shader->enable();

    wood->active(0);
    shader->setDiffuse(0);

    woodBumpAndCone->active(1);
    shader->setBumpMap(1);

    woodBumpAndCone->active(2);
    shader->setConeMap(2);

    shader->setConeDepth(conedepth);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Tangent, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].tangent));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Binormal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Binormal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].binormal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Binormal));

    woodBumpAndCone->deactive(2);
    woodBumpAndCone->deactive(1);
    wood->deactive(0);

}

void ShaderManager::draw_ShaderConeStepMappingOriginal(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderConeStepMappingOriginal *shader = (ShaderConeStepMappingOriginal*)baseshader;

    shader->enable();

    diffuse->active(0);
    shader->setDiffuse(0);

    bumpmap->active(1);
    shader->setBumpMap(1);

    conemap->active(2);
    shader->setConeMap(2);

    shader->setConeDepth(conedepth);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Tangent, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].tangent));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Binormal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Binormal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].binormal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Tangent));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Binormal));

    conemap->deactive(2);
    bumpmap->deactive(1);
    diffuse->deactive(0);

}

void ShaderManager::draw_ShaderLaplace(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderLaplace *shader = (ShaderLaplace*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setFramebuffer(0);

    shader->setFramebufferTexelNeighbor(framebufferNeighbor);

    shader->setModelViewProjection(render->getMVP());

    shader->setIntensity(laplaceIntensity);
    shader->setBlend(laplaceBlend);

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);

}

void ShaderManager::draw_ShaderBlur(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {

    if (blurSteps == 0){

        glClear(GL_COLOR_BUFFER_BIT);

        RenderSystem * render = RenderSystem::getSingleton();

        render->projection.push();
        render->projection.top = mat4::IdentityMatrix;

        render->model.push();
        render->model.top = mat4::IdentityMatrix;// * scale (1,-1,1);

        render->shader->enable();
        render->shader->setColor(vec4(1.0f));
        //normalized mapped quad draw..

        const vec3 vpos[] = {

            vec3(-1, -1, 0.0f),
            vec3(1, -1, 0.0f),
            vec3(1, 1, 0.0f),

            vec3(-1, -1, 0.0f),
            vec3(1, 1, 0.0f),
            vec3(-1, 1, 0.0f)
        };

        const vec2 vuv[] = {
            vec2(0, 0),
            vec2(1, 0),
            vec2(1, 1),

            vec2(0, 0),
            vec2(1, 1),
            vec2(0, 1)
        };

        render->drawTexture(framebuffer->color[0], GL_TRIANGLES, vpos, vuv, 6);

        render->model.pop();
        render->projection.pop();
        return;
    }


    ShaderBlur *shader = (ShaderBlur*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setFramebuffer(0);

    shader->setFramebufferTexelNeighbor(framebufferNeighbor);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    for (int i = 0; i < blurSteps; i++) {
        framebuffer->color[0]->active(0);
        framebufferBlur->enable();
        shader->setHorizontal();
        OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));
        //framebuffer->copyFrameBuffer();

        framebufferBlur->color[0]->active(0);

        if (i== blurSteps-1)
            GLFramebufferObject::disable();
        else
            framebuffer->enable();

        shader->setVertical();
        OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));
        //framebuffer->copyFrameBuffer();
    }

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);

}

void ShaderManager::draw_ShaderBlinPhongVertex(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderBlinPhongVertex *shader = (ShaderBlinPhongVertex*)baseshader;

    shader->enable();

    //diffuse->active(0);
    defaultTexture->active(0);
    shader->setDiffuse(0);

    shader->setObjectColor(objectColor);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));

    //diffuse->deactive(0);
    defaultTexture->deactive(0);
}

void ShaderManager::draw_BlinPhongPixel(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderBlinPhongPixel *shader = (ShaderBlinPhongPixel*)baseshader;

    shader->enable();

    //diffuse->active(0);
    defaultTexture->active(0);
    shader->setDiffuse(0);

    shader->setObjectColor(objectColor);

    shader->setModelViewProjection(render->getMVP());
    shader->setModelView(render->model.top);
    shader->setModelViewInverseTranspose(render->getModelIT());

    shader->setLightPosition(lightPosition);
    shader->setLightAmbient(lightAmbient);
    shader->setLightDiffuse(lightDiffuse);
    shader->setLightSpecular(lightSpecular);
    shader->setLightShine(lightShine);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Normal));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Normal, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].normal));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Normal));

    defaultTexture->deactive(0);
    //diffuse->deactive(0);

}

void ShaderManager::draw_ShaderBrightnessContrast(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderBrightnessContrast *shader = (ShaderBrightnessContrast*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setTexture(0);

    shader->setModelViewProjection(render->getMVP());

    shader->setBrightness(brightness);
    shader->setContrast(contrast);


    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);

}

void ShaderManager::draw_ShaderGrayScaleInternet(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderGrayScaleInternet *shader = (ShaderGrayScaleInternet*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setTexture(0);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);
}

void ShaderManager::draw_ShaderGrayScaleHumanVisualSystem(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderGrayScaleHumanVisualSystem *shader = (ShaderGrayScaleHumanVisualSystem*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setTexture(0);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);
}

void ShaderManager::draw_ShaderGrayScaleHueSaturationValue(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderGrayScaleHueSaturationValue *shader = (ShaderGrayScaleHueSaturationValue*)baseshader;

    shader->enable();

    framebuffer->color[0]->active(0);
    shader->setTexture(0);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    framebuffer->color[0]->deactive(0);
}

void ShaderManager::draw_ShaderTwoTextures(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderTwoTextures *shader = (ShaderTwoTextures*)baseshader;

    shader->enable();

    diffuse->active(0);
    shader->setTextureA(0);

    diffuseB->active(1);
    shader->setTextureB(1);

    shader->setBlend(blend);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));

    diffuseB->deactive(1);
    diffuse->deactive(0);

}

void ShaderManager::draw_ShaderProceduralTexture(GLShader *baseshader, GLint ogl_primitive, const VertexAttrib *vertexBuffer, int vertexCount) {
    ShaderProceduralTexture *shader = (ShaderProceduralTexture*)baseshader;

    shader->enable();

    shader->setFrequency(frequency);

    shader->setModelViewProjection(render->getMVP());

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec3Position, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->aVec2UV));
    OPENGL_CMD(glVertexAttribPointer(shader->aVec2UV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertexBuffer[0].uv));

    OPENGL_CMD(glDrawArrays(ogl_primitive, 0, vertexCount));

    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec3Position));
    OPENGL_CMD(glDisableVertexAttribArray(shader->aVec2UV));
}


//
// all parameters
//

void ShaderManager::setupShaderParametersAndDraw(GLint ogl_primitive, const VertexAttrib *buffer, int vertexCount) {
    (this->*(shaders_draw_map[activeShaderName]))(activeShader, ogl_primitive, buffer, vertexCount);
}
