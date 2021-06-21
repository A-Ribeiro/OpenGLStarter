
#ifndef GLPostProcess__h
#define GLPostProcess__h

#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLFramebufferObject.h>
#include <opengl-wrapper/GLPostProcessingShader.h>
#include <vector>


namespace openglWrapper {

    class GLPostProcess;
    
    /// \brief Interface a class need to extend to implement a post-processing technique.
    ///
    /// The post processing main class deal with the screen resolution buffer creation.
    ///
    /// The subclass need to use the source color and source depth and write to the dst FBO.
    ///
    /// The last stage in the pipeline need to write to the screen.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <opengl-wrapper/opengl-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace openglWrapper;
    ///
    /// class FX: public iPostProcess {
    /// public:
    ///     virtual void render(GLPostProcess *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst) {
    ///         if (dst == NULL){
    ///             GLFramebufferObject::disable();
    ///             glViewport(0,0,postProcess->width,postProcess->height);
    ///         } else {
    ///             dst->enable();
    ///             glViewport(0,0,dst->width,dst->height);
    ///         }
    ///
    ///         // draw code
    ///         ...
    ///
    ///     }
    /// };
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class iPostProcess{
    public:
        virtual void render(GLPostProcess *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst) = 0;
    };
    

    /// \brief Manage a post-fx pipeline
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <opengl-wrapper/opengl-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace openglWrapper;
    ///
    /// class FX: public iPostProcess {
    /// public:
    ///     virtual void render(GLPostProcess *postProcess, GLTexture *colorSrc, GLTexture *depthSrc, GLFramebufferObject *dst) {
    ///         if (dst == NULL){
    ///             GLFramebufferObject::disable();
    ///             glViewport(0,0,postProcess->width,postProcess->height);
    ///         } else {
    ///             dst->enable();
    ///             glViewport(0,0,dst->width,dst->height);
    ///         }
    ///
    ///         // draw code
    ///         ...
    ///
    ///     }
    /// };
    ///
    /// GLPostProcess postProcess;
    /// 
    /// // Construct the post-fx pipeline
    /// postProcess.pipeline.push_back(new FX());
    ///
    /// postProcess.resize(screenWidth, screenHeight);
    ///
    /// ...
    ///
    /// postProcess.beginDraw();
    ///
    /// // OpenGL draw routines
    ///
    /// postProcess.endDraw();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLPostProcess{
        
    private:
        
        GLFramebufferObject* firstRender;
        GLFramebufferObject* fbo[2];
        int current;
        
        //private copy constructores, to avoid copy...
        GLPostProcess(const GLPostProcess& v);
        void operator=(const GLPostProcess& v);
        
    public:
        std::vector<iPostProcess*> pipeline;///< The post-fx pipeline sequence
        int width;///< the internal texture width
        int height;///< the internal texture height
        
        GLPostProcess();
        virtual ~GLPostProcess();
        
        /// \brief Resize all internal textures to the parameter size
        ///
        /// You can use the display width and height as the parameter.
        ///
        /// \author Alessandro Ribeiro
        /// \param w width
        /// \param h height
        ///
        void resize(int w, int h);

        /// \brief Should call this method before OpenGL draw commands.
        ///
        /// \author Alessandro Ribeiro
        ///
        void beginDraw();

        /// \brief Should call this method after OpenGL draw commands.
        ///
        /// \author Alessandro Ribeiro
        ///
        void endDraw();
        
        /// \brief Issue two triangles draw to the OpenGL pipeline.
        ///
        /// All shaders that use this method need to extend the #GLPostProcessingShader class.
        ///
        /// The quad bounds are: 
        /// 
        /// x: (-1, 1) UV: (0,1)
        ///
        /// y: (-1, 1) UV: (0,1)
        ///
        /// \author Alessandro Ribeiro
        /// \param shader the shader to use to draw this quad
        ///
        static void drawQuad(GLPostProcessingShader *shader);

        SSE2_CLASS_NEW_OPERATOR
    };

}

#endif

