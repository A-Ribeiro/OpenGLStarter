#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

//
// This value is multiplied by the xy of the normal calculated... it can be used to make the normal stronger and lighter...
//   
//
#define NORMAL_STRENGTH 15.0f

void computeConeStepMapping(const float *inputbuffer, int w, int h, const std::string &path, const std::string &filename_wo_ext, bool tile);
void writeColorVec4(const vec4 *inputbuffer, int w, int h, const std::string &outputfilename);
void writeNormals(const vec3 *inputbuffer, int w, int h, const std::string &outputfilename);
void readPNGAndCompute(const std::string &path, const std::string &filename, const std::string &filename_wo_ext, bool tile);
void printUsage();
bool ends_with(std::string const & value, std::string const & ending);
void computeNormals(vec3 *output, int w, int h, const float* depthmap, bool tile);

void computeConeStepMappingInBuffer(vec4 *workingbuffer, int w, int h, float tile);

#include "conestep.h"


void computeConeStepMapping(const float *depthmap, int w, int h, const std::string &path, const std::string &filename_wo_ext, bool tile) {
    vec3 *normals = new vec3[w*h];
    fprintf(stdout, "  1st step: compute normals\n");
    if (tile)
        fprintf(stdout, "            Using texture as tile\n");

    computeNormals(normals, w, h, depthmap, tile);

    writeNormals(normals, w, h, path + PlatformPath::SEPARATOR + filename_wo_ext + std::string("-normal.png"));
    

    //
    fprintf(stdout, "  2st step: compute cones\n");

    vec4 *cone = new vec4[w*h];

    //create an RGBA image to pass to the algorithm
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            cone[x + y * h] = vec4(vec3(depthmap[x + y * h]), 1.0f);
        }
    }

    computeConeStepMappingInBuffer(cone, w, h, tile);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            
            //copy the calculated cone to z coord
            cone[x + y * h].z = cone[x + y * h].z;

            //copy normal xy component (the z is computed in shader)
            vec3 normal = normals[x + y * h];
            normal = clamp(normal, vec3(-1), vec3(1));
            normal = normal * 0.5f + 0.5f; 
            normal = clamp(normal, vec3(0), vec3(1));

            cone[x + y * h].x = normal.x;
            cone[x + y * h].y = normal.y;
            //copy the depthmap to the cone texture
            cone[x + y * h].w = depthmap[x + y * h];
        }
    }

    writeColorVec4(cone, w, h, path + PlatformPath::SEPARATOR + filename_wo_ext + std::string("-normalxy+cone+depthmap.png"));

    delete[] normals;
    delete[]cone;

}


int main(int argc, char *argv[]) {
    fprintf(stdout, "conestep-generator - alessandroribeiro.thegeneralsolution.com\n");

    if (argc != 2) {
        printUsage();
        return 0;
    }

    std::string filepath = std::string(argv[1]);

    /*
    //
    // normalize path separator
    //
    std::replace(filepath.begin(), filepath.end(), '\\', PlatformPath::SEPARATOR[0]);
    std::replace(filepath.begin(), filepath.end(), '/', PlatformPath::SEPARATOR[0]);


    std::string folder, filename, filename_wo_ext;
    size_t path_directory_index = filepath.find_last_of(PlatformPath::SEPARATOR[0]);
    if (path_directory_index == -1) {
        folder = ".";
        filename = filepath;
    }
    else {
        folder = filepath.substr(0, path_directory_index);
        filename = filepath.substr(path_directory_index + 1, filepath.size() - 1 - path_directory_index);
    }

    path_directory_index = filename.find_last_of('.');
    if (path_directory_index == -1) {
        filename_wo_ext = filename;
    }
    else {
        filename_wo_ext = filename.substr(0, path_directory_index);
    }
     */
    
    std::string folder, filename, filename_wo_ext, fileext;
    PlatformPath::splitPathString(filepath, &folder, &filename, &filename_wo_ext, &fileext);

    fprintf(stdout, "Folder: %s\n", folder.c_str());
    fprintf(stdout, "Filename: %s\n", filename.c_str());
    fprintf(stdout, "Filename W/O ext: %s\n", filename_wo_ext.c_str());

    bool tile = ends_with(filename_wo_ext, std::string("tile"));

    readPNGAndCompute(folder, filename, filename_wo_ext, tile);



    //fflush(stdout);
    //fgetc(stdin);

    return 0;
}

void computeNormals(vec3 *output, int w, int h, const float* depthmap, bool tile) {

    //normal max inclination tolerance processing
#define NORMAL_TOLETANCE (110.0f/127.0f)

    //bool toleranceTranspassing = false;
    float maxXYLength = 0.0f;

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            float xA, xB, xC;
            xB = depthmap[x + y * w];
            xA = xB;
            xC = xB;
            if (x > 0)
                xA = depthmap[x - 1 + y * w];
            if (x < w - 1)
                xC = depthmap[x + 1 + y * w];

            if (tile) {
                if (x == 0)
                    xA = depthmap[w - 1 + y * w];
                if (x == w - 1)
                    xC = depthmap[0 + y * w];
            }
            else {
                if (x == 0) {
                    xA = (xB - xC) + xB;
                    //printf("border (must be the same value: %f %f\n", (xC - xB), (xB - xA));
                }
                if (x == w - 1) {
                    xC = -(xA - xB) + xB;
                    //printf("border (must be the same value: %f %f\n", (xC - xB), (xB - xA));
                }
            }

            float ddx = ((xB - xA) + (xC - xB)) / 2.0f;


            float yA, yB, yC;
            yB = depthmap[x + y * w];
            yA = yB;
            yC = yB;
            if (y > 0)
                yA = depthmap[x + (y - 1) * w];
            if (y < h - 1)
                yC = depthmap[x + (y + 1) * w];

            if (tile) {
                if (y == 0)
                    yC = depthmap[x + (h - 1) * w];
                if (y == h - 1)
                    yC = depthmap[x + (0) * w];
            }
            else {
                if (y == 0) {
                    yA = (yB - yC) + yB;
                    //printf("border (must be the same value: %f %f\n", (yB - yC), (yA - yB));
                }
                if (y == h - 1) {
                    yC = -(yA - yB) + yB;
                    //printf("border (must be the same value: %f %f\n", (yB - yC), (yA - yB));
                }
            }

            float ddy = ((yA - yB) + (yB - yC)) / 2.0f;

            ddx *= (float)(NORMAL_STRENGTH);
            ddy *= (float)(NORMAL_STRENGTH);

            vec3 normal = vec3(ddx, ddy, sqrt(1.0 - (ddx* ddx + ddy * ddy)));
            normal = normalize(normal);

            output[x + y * w] = normal;

            float xyLength = sqrt(normal.x * normal.x + normal.y * normal.y);
            maxXYLength = maximum(maxXYLength, xyLength);
            //if (xyLength >= NORMAL_TOLETANCE)
                //toleranceTranspassing = true;

        }


    

    //fix max tolerance (avoid srqt(1-length(xy)) NAN )
    //if (toleranceTranspassing)

    if (maxXYLength >= NORMAL_TOLETANCE)
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {

            vec3 normal = output[x + y * w];

            normal.x *= NORMAL_TOLETANCE / maxXYLength;
            normal.y *= NORMAL_TOLETANCE / maxXYLength;

            float xyLength = (normal.x * normal.x + normal.y * normal.y);
            normal.z = sqrt(1.0 - xyLength);

            output[x + y * w] = normal;
        }
    }
}

void writeColorVec4(const vec4 *inputbuffer, int w, int h, const std::string &outputfilename) {
    unsigned char *buffer = new unsigned char[w*h * 4];

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            vec4 color = inputbuffer[x + y * w] * 256.0f;
            int r, g, b, a;

            r = (int)color.x;
            g = (int)color.y;
            b = (int)color.z;
            a = (int)color.w;

            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
            if (a < 0)
                a = 0;

            if (r > 255)
                r = 255;
            if (g > 255)
                g = 255;
            if (b > 255)
                b = 255;
            if (a > 255)
                a = 255;

            buffer[(x + y * w) * 4 + 0] = (unsigned char)r;
            buffer[(x + y * w) * 4 + 1] = (unsigned char)g;
            buffer[(x + y * w) * 4 + 2] = (unsigned char)b;
            buffer[(x + y * w) * 4 + 3] = (unsigned char)a;
        }

    PNGHelper::writePNG(outputfilename.c_str(), w, h, 4, (char*)buffer);

    delete[] buffer;
}

void writeNormals(const vec3 *inputbuffer, int w, int h, const std::string &outputfilename) {
    unsigned char *buffer = new unsigned char[w*h * 3];

    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            vec3 normal = (normalize(inputbuffer[x + y * w]) * 0.5f + 0.5f) * 256.0f;
            int r, g, b;

            r = (int)normal.x;
            g = (int)normal.y;
            b = (int)normal.z;

            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;

            if (r > 255)
                r = 255;
            if (g > 255)
                g = 255;
            if (b > 255)
                b = 255;

            buffer[(x + y * w) * 3 + 0] = (unsigned char)r;
            buffer[(x + y * w) * 3 + 1] = (unsigned char)g;
            buffer[(x + y * w) * 3 + 2] = (unsigned char)b;
        }

    PNGHelper::writePNG(outputfilename.c_str(), w, h, 3, (char*)buffer);

    delete[] buffer;
}


void readPNGAndCompute(const std::string &path, const std::string &filename, const std::string &filename_wo_ext, bool tile) {
    //load png
    float *bufferFloat = NULL;
    int w, h, chn, depth;
    {
        char*bufferChar;
        bufferChar = PNGHelper::readPNG((path + PlatformPath::SEPARATOR + filename).c_str(), &w, &h, &chn, &depth);
        if (bufferChar != NULL) {
            if (chn == 1 && depth == 8) {
                bufferFloat = new float[w*h];
                unsigned char*bufferUChar = (unsigned char*)bufferChar;
                for (int y = 0; y < h; y++)
                    for (int x = 0; x < w; x++)
                    {
                        bufferFloat[x + y * w] = ((float)bufferUChar[x + y * w]) / 255.0f;
                    }
            }
            else if (chn == 1 && depth == 16) {
                bufferFloat = new float[w*h];
                unsigned short*bufferUShort = (unsigned short*)bufferChar;
                for (int y = 0; y < h; y++)
                    for (int x = 0; x < w; x++)
                    {
                        bufferFloat[x + y * w] = ((float)bufferUShort[x + y * w]) / 65535.0f;
                    }
            }
            else {
                fprintf(stdout, "Error: The PNG input file must be in gray scale without alpha channel. It can be 8 BPP or 16 BPP\n");
            }
            PNGHelper::closePNG(bufferChar);
        }
        else {
            fprintf(stdout, "Error to load PNG file...\n");
        }
    }

    if (bufferFloat != NULL) {

        computeConeStepMapping(bufferFloat, w, h, path, filename_wo_ext, tile);

        delete[] bufferFloat;
        bufferFloat = NULL;
    }

}

void printUsage() {
    fprintf(stdout,
        "Usage: conestep-generator <filename>\n"
        "       if the <filename> ends with \"tile\", then it will compute the normals as a tile.\n"
        "\n"
        "  Example: conestep-generator rock.png\n"
        "           conestep-generator rocktile.png (will generate as a tile)\n"
        "           conestep-generator rock-tile.png (will generate as a tile)\n");
}

bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
