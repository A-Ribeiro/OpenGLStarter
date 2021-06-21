#include "AssimpExporter.h"

#define DEBUG 0


void printUsage() {
    fprintf(stdout,
        "Usage: assimp2bams [-a|-gs] <filename>\n"
        "\n"
        "  Export Geometry, Scene and Animations: assimp2bams model.fbx\n"
        "  Export Only Animations: assimp2bams -a model.fbx\n"
        "  Export Geometry and Scene: assimp2bams -gs model.fbx\n");
}

int main(int argc, char* argv[]){
    aRibeiro::PlatformPath::setWorkingPath(aRibeiro::PlatformPath::getExecutablePath(argv[0]));
    printf("assimp2bams - http://alessandroribeiro.thegeneralsolution.com/ - \"make things easy\"\n");

#if DEBUG == 0
    if (argc < 2) {
        printUsage();
        return 0;
    }
#endif

    std::string filepath;
    bool export_anim = true;
    bool export_geometry_and_scene = true;

#if DEBUG == 0
        if (argc == 2)
            filepath = std::string(argv[1]);
        else if (argc == 3){
            export_geometry_and_scene = strcmp(argv[1], "-gs") == 0;
            export_anim = strcmp(argv[1], "-a") == 0;
            filepath = std::string(argv[2]);
        }
#endif

#if DEBUG == 1

#ifdef linux
    filepath = std::string("/home/alessandro/Desktop/OpenGLStarter/lib/assimp/test/models/FBX/spider.fbx");
#elif WIN32
    filepath = std::string("VirginMary.fbx");
#else
    filepath = std::string("/Users/alessandro/Desktop/GIT/OpenGLStarter/lib/assimp/test/models/FBX/spider.fbx");
#endif

#endif

    std::string folder, filename, filename_wo_ext, fileext;
    PlatformPath::splitPathString(filepath, &folder, &filename, &filename_wo_ext, &fileext);

    fprintf(stdout, "Folder: %s\n", folder.c_str());
    fprintf(stdout, "Filename: %s\n", filename.c_str());
    fprintf(stdout, "Filename W/O ext: %s\n", filename_wo_ext.c_str());

    std::string inputFile = (folder + PlatformPath::SEPARATOR + filename);
    std::string outputFile = (folder + PlatformPath::SEPARATOR + filename_wo_ext + std::string(".bams"));

    ModelContainer *container = ImportFromAssimp(inputFile.c_str());


    if (!export_anim)
        container->animations.clear();
    if (!export_geometry_and_scene){
        container->lights.clear();
        container->cameras.clear();
        container->materials.clear();
        container->geometries.clear();
        container->nodes.clear();
    }

    //
    // save the container
    //
    container->write(outputFile.c_str());
    
    delete container;


    return 0;
}

