#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "Raytracer.h"


void ProcessRaytracer(const std::string &fileToOpen, const std::string &outputFile, int w, int h) {
    Raytracer raytracer(fileToOpen, w, h);

    raytracer.run();

    raytracer.save(outputFile.c_str());

}


int main(int argc, char* argv[]) {

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));

    //ProcessRaytracer("tests/test1.txt", "output_test1.png", 400, 300);
    int width = 800;
    int height = 600;
    ProcessRaytracer("tests/test1.txt", "output_test1.png", width, height);
    ProcessRaytracer("tests/test2.txt", "output_test2.png", width, height);
    ProcessRaytracer("tests/test3.txt", "output_test3.png", width, height);
    ProcessRaytracer("tests/test4.txt", "output_test4.png", width, height);
    ProcessRaytracer("tests/test5.txt", "output_test5.png", width, height);
    ProcessRaytracer("tests/test6.txt", "output_test6.png", width, height);
    ProcessRaytracer("tests/test7.txt", "output_test7.png", width, height);
    ProcessRaytracer("tests/test8.txt", "output_test8.png", width, height);

    return 0;
}
