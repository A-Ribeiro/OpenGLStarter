//
// Code adapted from: http://www.lonesock.net/files/ConeStepMapping.pdf
//

void computeConeStepMappingInBuffer(vec4 *depthMapBuffer, int w, int h, float tile) {

    //basically, 99% of all pixels will fall in under 2.0(most of the time, on the heightmaps I've tested)
    //the question:Is reduced resolution worth missingthe speedup of the slow ones?

    //const float max_ratio = 1.0;
    const float max_ratio = 2.0;

    // And for the cone version, how tolerant am I?(and should it be a ratio, tolerance*r^2, or flat?)
    //const float cone_tol2 = 4.0f / (255.0f * 255.0f);

    // Do I want the textures to be computed as tileable? 
    bool x_tileable = tile;
    bool y_tileable = tile;

    unsigned char *Data = new unsigned char[w*h*4];

    // copy float buffer to uchar buffer
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Data[(x + y * w) * 4 + 0] = (unsigned char)(depthMapBuffer[(x + y * w)].x * 255.0f + 0.5f);
            Data[(x + y * w) * 4 + 1] = (unsigned char)(depthMapBuffer[(x + y * w)].y * 255.0f + 0.5f);
            Data[(x + y * w) * 4 + 2] = (unsigned char)(depthMapBuffer[(x + y * w)].z * 255.0f + 0.5f);
            Data[(x + y * w) * 4 + 3] = (unsigned char)(depthMapBuffer[(x + y * w)].w * 255.0f + 0.5f);
        }
    }
    
    int width = w;
    int height = h;
    int chans = 4;
    // forced for now (by corona)
    int ScanWidth = chans * width;
    //int TheSize = ScanWidth * height;

    // invert this (used to convert depth-map to height-map) 
    for (int px = 0; px < width*height; ++px)
        Data[px*chans] = 255 - Data[px*chans];

    float iheight = 1.0f / (float)height;
    float iwidth = 1.0f / (float)width;

    // pre-processing: compute derivatives 
    for (int y = 0; y < height; ++y) {
        // progress report: works great...if it's square! 
        for (int x = 0; x < width; ++x) {
            int der;
            // Blue is the slope in x 
            if (x == 0)
                der = (Data[y*ScanWidth + chans * (x + 1)] - Data[y*ScanWidth + chans * (x)]) / 2;
            else if (x == width - 1)
                der = (Data[y*ScanWidth + chans * (x)] - Data[y*ScanWidth + chans * (x - 1)]) / 2;
            else
                der = Data[y*ScanWidth + chans * (x + 1)] - Data[y*ScanWidth + chans * (x - 1)];
            Data[y*ScanWidth + chans * x + 2] = 127 + der / 2;
            // Alpha is the slope in y 
            if (y == 0)
                der = (Data[(y + 1)*ScanWidth + chans * x] - Data[(y)*ScanWidth + chans * x]) / 2;
            else if (y == height - 1)
                der = (Data[(y)*ScanWidth + chans * x] - Data[(y - 1)*ScanWidth + chans * x]) / 2;
            else
                der = (Data[(y + 1)*ScanWidth + chans * x] - Data[(y - 1)*ScanWidth + chans * x]);
            // And the sign of Y will be reversed in OpenGL 
            Data[y*ScanWidth + chans * x + 3] = 127 - der / 2;
        }
    }

    // OK, do the processing 
    for (int y = 0; y < height; ++y) {
        printf("progress: %f\n", (float)y / (float)(height - 1.0f));
        for (int x = 0; x < width; ++x)
        {
            float min_ratio2, actual_ratio;
            int x1, x2, y1, y2;
            float ht, dhdx, dhdy, r2, h2;
            // set up some initial values
            // (note I'm using ratio squared throughout,
            // and taking sqrt at the end...faster) 
            min_ratio2 = max_ratio * max_ratio;
            // information about this center point 
            ht = Data[y*ScanWidth + chans * x] / 255.0;
            dhdx = +(Data[y*ScanWidth + chans * x + 2] / 255.0 - 0.5) * width;
            dhdy = -(Data[y*ScanWidth + chans * x + 3] / 255.0 - 0.5) * height;
            // scan in outwardly expanding blocks
            // (so I can stop if I reach my minimum ratio) 
            for (int rad = 1; 
                (rad*rad <= 1.1 * 1.1*(1.0 - ht)*(1.0 - ht)*min_ratio2*width*height) && 
                (rad <= 1.1*(1.0 - ht)*width) && 
                (rad <= 1.1*(1.0 - ht)*height); 
                ++rad) {
                // ok, for each of these lines...
                // West 
                x1 = x - rad;
                while (x_tileable && (x1 < 0))
                    x1 += width;
                if (x1 >= 0) {
                    float delx = -rad * iwidth;
                    // y limits
                    // (+- 1 because I'll cover the corners in the X-run) 
                    y1 = y - rad + 1;
                    if (y1 < 0)
                        y1 = 0;
                    y2 = y + rad - 1;
                    if (y2 >= height)
                        y2 = height - 1;
                    // and check the line 
                    for (int dy = y1; dy <= y2; ++dy) {
                        float dely = (dy - y)*iheight;
                        r2 = delx * delx + dely * dely;
                        h2 = Data[dy*ScanWidth + chans * x1] / 255.0 - ht;
                        if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                            // this is the new (lowest) value 
                            min_ratio2 = r2 / (h2 * h2);
                        }
                    }
                }
                // East 
                x2 = x + rad;
                while (x_tileable && (x2 >= width))
                    x2 -= width;
                if (x2 < width) {
                    float delx = rad * iwidth;
                    // y limits
                    // (+- 1 because I'll cover the corners in the X-run) 
                    y1 = y - rad + 1;
                    if (y1 < 0)
                        y1 = 0;
                    y2 = y + rad - 1;
                    if (y2 >= height)
                        y2 = height - 1;
                    // and check the line
                    for (int dy = y1; dy <= y2; ++dy) {
                        float dely = (dy - y)*iheight;
                        r2 = delx * delx + dely * dely;
                        h2 = Data[dy*ScanWidth + chans * x2] / 255.0 - ht;
                        if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                            // this is the new (lowest) value 
                            min_ratio2 = r2 / (h2 * h2);
                        }
                    }
                }
                // North 
                y1 = y - rad;
                while (y_tileable && (y1 < 0))
                    y1 += height;
                if (y1 >= 0) {
                    float dely = -rad * iheight;
                    // y limits
                    // (+- 1 because I'll cover the corners in the X-run) 
                    x1 = x - rad;
                    if (x1 < 0)
                        x1 = 0;
                    x2 = x + rad;
                    if (x2 >= width)
                        x2 = width - 1;
                    // and check the line 
                    for (int dx = x1; dx <= x2; ++dx) {
                        float delx = (dx - x)*iwidth;
                        r2 = delx * delx + dely * dely;
                        h2 = Data[y1*ScanWidth + chans * dx] / 255.0 - ht;
                        if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                            // this is the new (lowest) value 
                            min_ratio2 = r2 / (h2 * h2);
                        }
                    }
                }
                // South 
                y2 = y + rad;
                while (y_tileable && (y2 >= height))y2 -= height;
                if (y2 < height) {
                    float dely = rad * iheight;
                    // y limits
                    // (+- 1 because I'll cover the corners in the X-run) 
                    x1 = x - rad;
                    if (x1 < 0)x1 = 0;
                    x2 = x + rad;
                    if (x2 >= width)
                        x2 = width - 1;
                    // and check the line 
                    for (int dx = x1; dx <= x2; ++dx) {
                        float delx = (dx - x)*iwidth;
                        r2 = delx * delx + dely * dely;
                        h2 = Data[y2*ScanWidth + chans * dx] / 255.0 - ht;
                        if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                            // this is the new (lowest) value 
                            min_ratio2 = r2 / (h2 * h2);
                        }
                    }
                }
                // done with the expanding loop
            }
            // ********** CONE VERSION **********
            // actually I have the ratio squared. Sqrt it 
            actual_ratio = sqrt(min_ratio2);
            // here I need to scale to 1.0 
            actual_ratio /= max_ratio;
            // most of the data is on the low end...sqrting again spreads it better
            // (plus multiply is a cheap operation in shaders!) 
            actual_ratio = sqrt(actual_ratio);
            // Red stays height
            // Blue remains the slope in x
            // Alpha remains the slope in y
            // but Green becomes Step-Cone-Ratio 
            Data[y*ScanWidth + chans * x + 1] = static_cast<unsigned char>(255.0 * actual_ratio + 0.5);
            // but make sure it is > 0.0, since I divide by it in the shader 
            if (Data[y*ScanWidth + chans * x + 1] < 1)
                Data[y*ScanWidth + chans * x + 1] = 1;
        }
    }

    // copy back the cone buffer
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            //depthMapBuffer[(x + y * w)].x = ((float)Data[(x + y * w) * 4 + 2]) / 255.0f;
            //depthMapBuffer[(x + y * w)].y = ((float)Data[(x + y * w) * 4 + 3]) / 255.0f;

            depthMapBuffer[(x + y * w)].z = ((float)Data[(x + y * w) * 4 + 1]) / 255.0f;
        }
    }

    delete[] Data;
}

/*
// Theirs 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <vector> 
// in-between 
using namespace std;

//basically, 99% of all pixels will fall in under 2.0(most of the time, on the heightmaps I've tested)
//the question:Is reduced resolution worth missingthe speedup of the slow ones?

const float max_ratio = 1.0;

// And for the cone version, how tolerant am I?(and should it be a ratio, tolerance*r^2, or flat?)
const float cone_tol2 = 4.0 / (255.0 * 255.0);

// Do I want the textures to be computed as tileable? 
bool x_tileable = true;
bool y_tileable = true;


int main(int argc, char *argv[]) {
    char OutName[1024];
    int FileCounter;
    long TheSize,
        ScanWidth;
    long width, height;
    float iwidth, iheight;
    long chans;
    corona::Image *image, *outimage;
    unsigned char *Data;

    long tin;
    int wProgress;
    float really_max = 1.0;
    std::cout << "********** Height Map Processor **********" << endl << endl;
    // Did I get a file name? 
    if (argc < 2) {
        // Oops, no file to convert 
        std::cout << "usage: HeightProc input_file" << endl << endl;
        system("pause");
        return (0);
    }

    for (FileCounter = 1; FileCounter < argc; ++FileCounter) {
        std::cout << "Converting file #" << FileCounter << endl;
        // OK, open the image file 
        image = corona::OpenImage(argv[FileCounter], corona::PF_R8G8B8A8);
        if (!image) {
            std::cout << "I could not open " << argv[FileCounter] << endl << endl;
            system("pause");
            return (0);
        }
        std::cout << "Loading " << argv[FileCounter] << ":" << endl;
        width = image->getWidth();

        height = image->getHeight();
        chans = 4;
        // forced for now (by corona)
        ScanWidth = chans * width;
        TheSize = ScanWidth * height;
        Data = (unsigned char *)(image->getPixels());
        // invert this (used to convert depth-map to height-map) 
        if (false) {
            for (int px = 0; px < width*height; ++px)
                Data[px*chans] = 255 - Data[px*chans];
        }
        iheight = 1.0 / height;
        iwidth = 1.0 / width;
        wProgress = width / 50;
        std::cout << endl << "The image " << argv[FileCounter] << " has been loaded." << endl;
        std::cout << " size: " << width << " x " << height << " by " << 32 << " bits" << endl;
        std::cout << endl << "Computing:" << endl;
        // warning message if the texture is not square 
        if (width != height) {
            std::cout << endl << "Warning: The image is not square! Results not guaranteed!" << endl;
            system("pause");
        } 
        // Redo the best, and save it
        // (only writing formats supported: PNG, TGA)
        // ((and PNG is compressed and has alpha!!)) 
        strcpy(OutName, argv[FileCounter]);
        strcat(OutName, "-step.png");
        tin = clock();
        // pre-processing: compute derivatives 
        std::cout << "Calculating derivatives [";
        for (int y = 0; y < height; ++y) {
            // progress report: works great...if it's square! 
            if (y % wProgress == 0)
                std::cout << ".";
            for (int x = 0; x < width; ++x) {
                int der;
                // Blue is the slope in x 
                if (x == 0)
                    der = (Data[y*ScanWidth + chans * (x + 1)] - Data[y*ScanWidth + chans * (x)]) / 2;
                else if (x == width - 1)
                    der = (Data[y*ScanWidth + chans * (x)] - Data[y*ScanWidth + chans * (x - 1)]) / 2;
                else
                    der = Data[y*ScanWidth + chans * (x + 1)] - Data[y*ScanWidth + chans * (x - 1)];
                Data[y*ScanWidth + chans * x + 2] = 127 + der / 2;
                // Alpha is the slope in y 
                if (y == 0)
                    der = (Data[(y + 1)*ScanWidth + chans * x] - Data[(y)*ScanWidth + chans * x]) / 2;
                else if (y == height - 1)
                    der = (Data[(y)*ScanWidth + chans * x] - Data[(y - 1)*ScanWidth + chans * x]) / 2;
                else
                    der = (Data[(y + 1)*ScanWidth + chans * x] - Data[(y - 1)*ScanWidth + chans * x]);
                // And the sign of Y will be reversed in OpenGL 
                Data[y*ScanWidth + chans * x + 3] = 127 - der / 2;
            }
        }
        std::cout << "]" << endl;
        // OK, do the processing 
        for (int y = 0; y < height; ++y) {
            std::cout << "img " << (argc - FileCounter) << ": line " << (height - y) << " [";
            for (int x = 0; x < width; ++x)
            {
                float min_ratio2, actual_ratio;
                int x1, x2, y1, y2;
                float ht, dhdx, dhdy, r2, h2;
                if ((x % wProgress) == 0)
                    std::cout << ".";
                // set up some initial values
                // (note I'm using ratio squared throughout,
                // and taking sqrt at the end...faster) 
                min_ratio2 = max_ratio * max_ratio;
                // information about this center point 
                ht = Data[y*ScanWidth + chans * x] / 255.0;
                dhdx = +(Data[y*ScanWidth + chans * x + 2] / 255.0 - 0.5) * width;
                dhdy = -(Data[y*ScanWidth + chans * x + 3] / 255.0 - 0.5) * height;
                // scan in outwardly expanding blocks
                // (so I can stop if I reach my minimum ratio) 
                for (int rad = 1; (rad*rad <= 1.1 * 1.1*(1.0 - ht)*(1.0 - ht)*min_ratio2*width*height) && (rad <= 1.1*(1.0 - ht)*width) && (rad <= 1.1*(1.0 - ht)*height); ++rad) {
                    // ok, for each of these lines...
                    // West 
                    x1 = x - rad;
                    while (x_tileable && (x1 < 0))
                        x1 += width;
                    if (x1 >= 0) {
                        float delx = -rad * iwidth;
                        // y limits
                        // (+- 1 because I'll cover the corners in the X-run) 
                        y1 = y - rad + 1;
                        if (y1 < 0)
                            y1 = 0;
                        y2 = y + rad - 1;
                        if (y2 >= height)
                            y2 = height - 1;
                        // and check the line 
                        for (int dy = y1; dy <= y2; ++dy) {
                            float dely = (dy - y)*iheight;
                            r2 = delx * delx + dely * dely;
                            h2 = Data[dy*ScanWidth + chans * x1] / 255.0 - ht;
                            if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                                // this is the new (lowest) value 
                                min_ratio2 = r2 / (h2 * h2);
                            }
                        }
                    }
                    // East 
                    x2 = x + rad;
                    while (x_tileable && (x2 >= width))
                        x2 -= width;
                    if (x2 < width) {
                        float delx = rad * iwidth;
                        // y limits
                        // (+- 1 because I'll cover the corners in the X-run) 
                        y1 = y - rad + 1;
                        if (y1 < 0)
                            y1 = 0;
                        y2 = y + rad - 1;
                        if (y2 >= height)
                            y2 = height - 1;
                        // and check the line
                        for (int dy = y1; dy <= y2; ++dy) {
                            float dely = (dy - y)*iheight;
                            r2 = delx * delx + dely * dely;
                            h2 = Data[dy*ScanWidth + chans * x2] / 255.0 - ht;
                            if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                                // this is the new (lowest) value 
                                min_ratio2 = r2 / (h2 * h2);
                            }
                        }
                    }
                    // North 
                    y1 = y - rad;
                    while (y_tileable && (y1 < 0))
                        y1 += height;
                    if (y1 >= 0) {
                        float dely = -rad * iheight;
                        // y limits
                        // (+- 1 because I'll cover the corners in the X-run) 
                        x1 = x - rad;
                        if (x1 < 0)
                            x1 = 0;
                        x2 = x + rad;
                        if (x2 >= width)
                            x2 = width - 1;
                        // and check the line 
                        for (int dx = x1; dx <= x2; ++dx) {
                            float delx = (dx - x)*iwidth;
                            r2 = delx * delx + dely * dely;
                            h2 = Data[y1*ScanWidth + chans * dx] / 255.0 - ht;
                            if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                                // this is the new (lowest) value 
                                min_ratio2 = r2 / (h2 * h2);
                            }
                        }
                    }
                    // South 
                    y2 = y + rad;
                    while (y_tileable && (y2 >= height))y2 -= height;
                    if (y2 < height) {
                        float dely = rad * iheight;
                        // y limits
                        // (+- 1 because I'll cover the corners in the X-run) 
                        x1 = x - rad;
                        if (x1 < 0)x1 = 0;
                        x2 = x + rad;
                        if (x2 >= width)
                            x2 = width - 1;
                        // and check the line 
                        for (int dx = x1; dx <= x2; ++dx) {
                            float delx = (dx - x)*iwidth;
                            r2 = delx * delx + dely * dely;
                            h2 = Data[y2*ScanWidth + chans * dx] / 255.0 - ht;
                            if ((h2 > 0.0) && (h2*h2 * min_ratio2 > r2)) {
                                // this is the new (lowest) value 
                                min_ratio2 = r2 / (h2 * h2);
                            }
                        }
                    }
                    // done with the expanding loop
                }
                // ********** CONE VERSION **********
                // actually I have the ratio squared. Sqrt it 
                actual_ratio = sqrt(min_ratio2);
                // here I need to scale to 1.0 
                actual_ratio /= max_ratio;
                // most of the data is on the low end...sqrting again spreads it better
                // (plus multiply is a cheap operation in shaders!) 
                actual_ratio = sqrt(actual_ratio);
                // Red stays height
                // Blue remains the slope in x
                // Alpha remains the slope in y
                // but Green becomes Step-Cone-Ratio 
                Data[y*ScanWidth + chans * x + 1] = static_cast<unsigned char>(255.0 * actual_ratio + 0.5);
                // but make sure it is > 0.0, since I divide by it in the shader 
                if (Data[y*ScanWidth + chans * x + 1] < 1)
                    Data[y*ScanWidth + chans * x + 1] = 1;
            }
            std::cout << "]" << endl;
        }
        // end my timing after the computation phase 
        tin = clock() - tin;
        std::cout << "Processed in " << tin * 0.001 << " seconds" << endl << endl;
        outimage = corona::CreateImage(width, height, corona::PF_R8G8B8A8, Data);
        if (outimage != NULL) {
            tin = clock();
            std::cout << "Saved: " << OutName << " " << corona::SaveImage(OutName, corona::FF_PNG, outimage) << endl;
            tin = clock() - tin;
            std::cout << "(That took " << tin * 0.001 << " seconds)" << endl;
        }
        else
            std::cout << "Couldn't create the new image" << endl;
        // Report 
        std::cout << endl << endl;
    }
    // and finish all file names passed in 

    //std::cout << "really_max = " << really_max << endl;
    //system ("pause");
    // And end it 
    std::cout << endl << "Done processing the image(s)." << endl;
    //system("PAUSE"); 
    return (0);
}

*/
