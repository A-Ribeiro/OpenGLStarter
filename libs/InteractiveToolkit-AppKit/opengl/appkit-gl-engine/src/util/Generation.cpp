#include <appkit-gl-engine/util/Generation.h>
#include <InteractiveToolkit/AlgorithmCore/Sorting/RadixCountingSort.h>

namespace AppKit
{
    namespace GLEngine
    {

        // https://stackoverflow.com/questions/28567166/uniformly-distribute-x-points-inside-a-circle
        std::vector<MathCore::vec3f> PatternPoints::sunFlower(int n, int alpha, bool geodesic)
        {
            std::vector<MathCore::vec3f> points;

            float angle_stride;
            if (geodesic)
                angle_stride = 360.0f * MathCore::CONSTANT<float>::PHI;
            else
                angle_stride = 2.0f * MathCore::CONSTANT<float>::PI / (MathCore::CONSTANT<float>::PHI * MathCore::CONSTANT<float>::PHI);

            float nFloat = (float)n;

            float b = round((float)alpha * sqrt(nFloat)); // number of boundary points

            float nMinusB = nFloat - b;
            float r;
            float kFloat;
            float inv_sqrt_nFloat = 1.0f / sqrt(nFloat - (b + 1.0f) / 2.0f);
            for (int k = 1; k <= n; k++)
            {
                kFloat = (float)k;
                if (kFloat > nMinusB)
                    r = 1.0f;
                else
                    r = sqrt(kFloat - 0.5f) * inv_sqrt_nFloat;
                float theta = kFloat * angle_stride;
                points.push_back(MathCore::vec3f(r * cos(theta), r * sin(theta), 0.0f));
            }

            return points;
        }

        void PatternPoints::optimize2DAccess(std::vector<MathCore::vec3f> *points, int texturesize, bool radial)
        {

            std::vector<AlgorithmCore::Sorting::SortIndexi> toSort;
            std::vector<AlgorithmCore::Sorting::SortIndexi> toSortAux;
            toSort.resize(points->size());
            toSortAux.resize(points->size());

            if (radial)
            {

                for (int i = 0; i < (int)toSort.size(); i++)
                {
                    toSort[i].toSort = 
                    AlgorithmCore::Sorting::SortTooli::floatToInt(
                        MathCore::OP<MathCore::vec3f>::length(points->at(i)));
                    toSort[i].index = i;
                }
                AlgorithmCore::Sorting::RadixCountingSorti::sortIndex(&toSort[0], (uint32_t)toSort.size(), &toSortAux[0]);
            }
            else
            {

                // sort x
                for (int i = 0; i < (int)toSort.size(); i++)
                {
                    toSort[i].toSort = AlgorithmCore::Sorting::SortTooli::floatToInt(
                        points->at(i).x
                        //+ floor( points->at(i).y * (float)texturesize )
                    );
                    toSort[i].index = i;
                }
                AlgorithmCore::Sorting::RadixCountingSorti::sortIndex(&toSort[0], (uint32_t)toSort.size(), &toSortAux[0]);

                // sort y
                for (int i = 0; i < (int)toSort.size(); i++)
                {
                    // toSort[i].toSort = AlgorithmCore::Sorting::SortTooli::floatToInt( points->at(toSort[i].index).y );
                    toSort[i].toSort = AlgorithmCore::Sorting::SortTooli::floatToInt(
                        MathCore::OP<float>::floor( points->at(toSort[i].index).y * (float)texturesize )
                    );
                }
                AlgorithmCore::Sorting::RadixCountingSorti::sortIndex(&toSort[0], (uint32_t)toSort.size(), &toSortAux[0]);
            }

            // set the final array
            std::vector<MathCore::vec3f> points_src = *points;

            MathCore::vec3f tmp;
            for (int i = 0; i < (int)toSort.size(); i++)
            {
                (*points)[i] = points_src[toSort[i].index];
                // printf("%f\n", (*points)[ i ].x);
            }
        }

    }

}
