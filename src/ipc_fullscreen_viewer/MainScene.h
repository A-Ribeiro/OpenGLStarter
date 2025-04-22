#pragma once

#include <appkit-gl-engine/AppBase/SceneBase.h>
#include <appkit-gl-engine/util/Button.h>

#include <InteractiveToolkit/Platform/IPC/LowLatencyQueueIPC.h>
#include <InteractiveToolkit-Extension/image/PNG.h>

ITK_INLINE
static int iClamp(int v)
{
    if (v > 255)
        return 255;
    else if (v < 0)
        return 0;
    else
        return v;
}
// from: https://en.wikipedia.org/wiki/YUV - Y′UV444 to RGB888 conversion
//
// C = (Y - 16)*298
// D = U - 128
// E = V - 128
//
#define YUV2RO(C, D, E) iClamp((C + 409 * (E) + 128) >> 8)
#define YUV2GO(C, D, E) iClamp((C - 100 * (D)-208 * (E) + 128) >> 8)
#define YUV2BO(C, D, E) iClamp((C + 516 * (D) + 128) >> 8)
// from: https://en.wikipedia.org/wiki/YUV - Y′UV444 to RGB888 conversion
//
// RGB -> YUV conversion macros
//
#define RGB2Y(r, g, b) (((66 * (r) + 129 * (g) + 25 * (b) + 128) >> 8) + 16)
#define RGB2U(r, g, b) (((-38 * (r)-74 * (g) + 112 * (b) + 128) >> 8) + 128)
#define RGB2V(r, g, b) (((112 * (r)-94 * (g)-18 * (b) + 128) >> 8) + 128)

struct YUV2RGB_Multithread_Job
{
    int block;
    int blockStart;
    int blockEnd;
    int width;
    int height;
    const uint8_t *in_buffer;
    uint8_t *out_buffer;
};

class YUV2RGB_Multithread : public EventCore::HandleCallback
{
public:
    std::vector<Platform::Thread *> threads;
    int jobDivider;
    int threadCount;

    Platform::Semaphore semaphore;
    Platform::ObjectQueue<YUV2RGB_Multithread_Job> queue;

    void threadRun()
    {
        bool isSignaled;
        while (!Platform::Thread::isCurrentThreadInterrupted())
        {
            YUV2RGB_Multithread_Job job = queue.dequeue(&isSignaled);
            if (isSignaled)
                break;

            // processing...
            for (int y = job.blockStart; y < job.blockEnd; y++)
            {
                // for (int y = 0; y < job.height; y++) {
                for (int x = 0; x < job.width; x++)
                {
                    int index = x + y * job.width;

                    int y = job.in_buffer[index * 2 + 0];

                    int u, v;
                    if (x % 2 == 0)
                    {
                        u = job.in_buffer[index * 2 + 1];
                        v = job.in_buffer[(index + 1) * 2 + 1];
                    }
                    else
                    {
                        u = job.in_buffer[(index - 1) * 2 + 1];
                        v = job.in_buffer[index * 2 + 1];
                    }

                    y = (y - 16) * 298;
                    u = u - 128;
                    v = v - 128;

                    job.out_buffer[index * 4 + 0] = YUV2RO(y, u, v);
                    job.out_buffer[index * 4 + 1] = YUV2GO(y, u, v);
                    job.out_buffer[index * 4 + 2] = YUV2BO(y, u, v);
                    job.out_buffer[index * 4 + 3] = 255;

                }
            }

            semaphore.release();
        }
    }

public:
    void yuy2_to_rgba(const uint8_t *in_buffer, uint8_t *out_buffer, int width, int height)
    {

        YUV2RGB_Multithread_Job job;

        job.in_buffer = in_buffer;
        job.out_buffer = out_buffer;
        job.width = width;
        job.height = height;

        int division = jobDivider;
        int h_per_block = height / jobDivider; // +(((height % jobDivider) > 0) ? 1 : 0);

        if (h_per_block == 0)
            h_per_block++;

        // if (height % jobDivider)
        // {
        //     // or we choose divison + 1 or we choose h_per_block + 1
        //     int div_plus_1_total = (division + 1) * h_per_block;
        //     int h_per_block_plus_1_total = division * (h_per_block + 1);
        //     if (div_plus_1_total < h_per_block_plus_1_total)
        //         division++;
        //     else
        //         h_per_block++;
        // }

        int jobs_dispatched = 0;
        int block = 0;

        job.block = block;
        job.blockStart = block * h_per_block;
        job.blockEnd = (block + 1) * h_per_block;

        while (job.blockStart < height)
        {
            if (height < job.blockEnd)
                job.blockEnd = height;

            queue.enqueue(job);

            jobs_dispatched ++;
            block ++;
            job.block = block;
            job.blockStart = block * h_per_block;
            job.blockEnd = (block + 1) * h_per_block;
        }

        for (int block = 0; block < jobs_dispatched; block++)
            semaphore.blockingAcquire();

        // job.blockStart = 0;
        // job.blockEnd = height;

        // queue.enqueue(job);
        // semaphore.blockingAcquire();

        // ITKExtension::Image::PNG::writePNG("output.png", width*2, height, 1, (char*)in_buffer);
        // ITK_ABORT(true,"Exit\n");
    }

    YUV2RGB_Multithread(int threadCount, int jobDivider) : semaphore(0)
    {
        this->jobDivider = jobDivider;
        this->threadCount = threadCount;
        for (int i = 0; i < threadCount; i++)
        {
            threads.push_back(new Platform::Thread(EventCore::CallbackWrapper(&YUV2RGB_Multithread::threadRun, this)));
        }
        for (int i = 0; i < threads.size(); i++)
            threads[i]->start();
    }

    void finalizeThreads()
    {
        printf("[YUV2RGB_Multithread] finalize threads start\n");

        for (int i = 0; i < threads.size(); i++)
            threads[i]->interrupt();
        for (int i = 0; i < threads.size(); i++)
            threads[i]->wait();

        printf("[YUV2RGB_Multithread] finalize threads done\n");
    }

    ~YUV2RGB_Multithread()
    {
        for (int i = 0; i < threads.size(); i++)
            threads[i]->interrupt();
        for (int i = 0; i < threads.size(); i++)
            threads[i]->wait();
        for (int i = 0; i < threads.size(); i++)
            delete threads[i];
        threads.clear();
        threadCount = 0;
    }
};

class MainScene : public AppKit::GLEngine::SceneBase
{
protected:
    // to load skybox, textures, cubemaps, 3DModels and setup materials
    virtual void loadResources();
    // to load the scene graph
    virtual void loadGraph();
    // to bind the resources to the current graph
    virtual void bindResourcesToGraph();

    // clear all loaded scene
    virtual void unloadAll();

public:
    YUV2RGB_Multithread m_YUV2RGB_Multithread;

    Platform::IPC::LowLatencyQueueIPC yuy2_queue;
    Platform::ObjectBuffer data_buffer;
    Platform::ObjectBuffer aux_rgb_buffer;

    AppKit::OpenGL::GLFont2Builder fontBuilder;

    std::shared_ptr<AppKit::OpenGL::GLTexture> texture;
    std::shared_ptr<AppKit::GLEngine::Transform> imageNode;

    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> componentFontToMesh;
    std::shared_ptr<AppKit::GLEngine::Transform> scaleNode;

    std::string text;
    float text_size;
    float text_margin;

    MainScene(
        Platform::Time *_time,
        AppKit::GLEngine::RenderPipeline *_renderPipeline,
        AppKit::GLEngine::ResourceHelper *_resourceHelper,
        AppKit::GLEngine::ResourceMap *_resourceMap,
        std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow);
    ~MainScene();

    virtual void draw();

    void resize(const MathCore::vec2i &size);

    void setText(const std::string &text, float _size, float horiz_margin);
};
