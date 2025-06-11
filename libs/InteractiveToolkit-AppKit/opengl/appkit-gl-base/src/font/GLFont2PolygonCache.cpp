#include <appkit-gl-base/font/GLFont2PolygonCache.h>
#include <InteractiveToolkit/AlgorithmCore/Polygon/Polygon2D.h>
#include <InteractiveToolkit/AlgorithmCore/Polygon/TriangulatorEarClipping.h>

namespace AppKit
{

    namespace OpenGL
    {

        void GLFont2PolygonCache::setFromGLFont2(const GLFont2 &glFont2, float size, float max_distance_tolerance, Platform::ThreadPool *threadPool)
        {
            this->size = size;
            float size_to_apply = size / glFont2.size;
            if (size_to_apply <= 0.0f)
                return;
            float max_distance_tolerance_to_apply = max_distance_tolerance / size_to_apply;
            if (max_distance_tolerance_to_apply <= 0.0f)
                return;

            triangulated_glyphs.clear();
            if (glFont2.polygonGlyphSrc.empty())
                return; // No polygon glyphs to triangulate
            if (threadPool != nullptr)
            {
                Platform::Mutex mutex;
                Platform::Semaphore semaphore;

                for (size_t i = 0; i < glFont2.polygonGlyphSrc.size(); ++i)
                    threadPool->postTask(
                        [&mutex, &semaphore, &glFont2, this, i, size_to_apply, max_distance_tolerance_to_apply]()
                        {
                            const auto &glyph = glFont2.polygonGlyphSrc[i];

                            std::vector<MathCore::vec2f> sampled_points;

                            GLFont2TriangulatedGlyph triangulated_glyph;
                            triangulated_glyph.advancex = glyph.advancex * size_to_apply; // Scale the advance x according to the font size

                            std::vector<AlgorithmCore::Polygon::EarClipping::ContourSampled> sampled_contours;
                            sampled_contours.reserve(glyph.contour.size());
                            for (const auto &contour : glyph.contour)
                            {
                                sampled_points.clear();
                                contour.samplePoints(&sampled_points, max_distance_tolerance_to_apply);
                                for (auto &point : sampled_points)
                                    point *= size_to_apply; // Scale the points according to the font size
                                sampled_contours.push_back({sampled_points, contour.isHole()});
                            }

                            // Triangulate the sampled contours considering the holes
                            AlgorithmCore::Polygon::EarClipping::triangulate(
                                sampled_contours,
                                &triangulated_glyph.vertices,
                                &triangulated_glyph.triangles);

                            {
                                Platform::AutoLock lock(&mutex);
                                triangulated_glyphs[glyph.charcode] = std::move(triangulated_glyph);
                            }
                            semaphore.release();
                        });
                // Wait for all tasks to finish
                for (size_t i = 0; i < glFont2.polygonGlyphSrc.size(); ++i)
                    semaphore.blockingAcquire();
            }
            else
            {
                std::vector<MathCore::vec2f> sampled_points;
                for (const auto &glyph : glFont2.polygonGlyphSrc)
                {
                    GLFont2TriangulatedGlyph triangulated_glyph;
                    triangulated_glyph.advancex = glyph.advancex * size_to_apply; // Scale the advance x according to the font size

                    std::vector<AlgorithmCore::Polygon::EarClipping::ContourSampled> sampled_contours;
                    sampled_contours.reserve(glyph.contour.size());
                    for (const auto &contour : glyph.contour)
                    {
                        sampled_points.clear();
                        contour.samplePoints(&sampled_points, max_distance_tolerance_to_apply);
                        for (auto &point : sampled_points)
                            point *= size_to_apply; // Scale the points according to the font size
                        sampled_contours.push_back({sampled_points, contour.isHole()});
                    }

                    // Triangulate the sampled contours considering the holes
                    AlgorithmCore::Polygon::EarClipping::triangulate(
                        sampled_contours,
                        &triangulated_glyph.vertices,
                        &triangulated_glyph.triangles);

                    triangulated_glyphs[glyph.charcode] = std::move(triangulated_glyph);
                }
            }
        }

        const GLFont2TriangulatedGlyph *GLFont2PolygonCache::getGlyph(uint32_t charcode) const
        {
            auto it = triangulated_glyphs.find(charcode);
            if (it == triangulated_glyphs.end())
                return nullptr;
            return &(it->second);
        }

    } // namespace OpenGL

} // namespace AppKit