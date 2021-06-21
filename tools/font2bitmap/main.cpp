#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <aribeiro/aribeiro.h>
#include <data-model/data-model.h>
#include <ft2-wrapper/ft2-wrapper.h>
using namespace aRibeiro;
using namespace DataModel;
using namespace ft2Wrapper;

#include "UTFops.h"
#include "ft2_font_generator.h"

#ifdef _WIN32
    #include "win32/getopt.h"
#else
    #include <getopt.h>
#endif

std::string UInt32toString(uint32_t i){
    char result[16];
    sprintf(result,"%u",i);
    return result;
}

std::string UInt32toStringHEX(uint32_t i) {
    char result[16];
    sprintf(result, "%x", i);
    return result;
}

struct ParametersSet{

    bool i,o,s,c,p,t;

    std::string inputFile;
    std::string outputFile;
    int characterSize;
    std::string characterUtf8Charset;
    int spaceBetweenChar;
    float outlineThickness;
    int faceToSelect;
};

int main(int argc, char *argv[]){
    fprintf (stdout, "font2bitmap - alessandroribeiro.thegeneralsolution.com\n");

    ParametersSet parameters={0,0,0,0,0,0,"","",0,"",0,0.0f,0};
    int c;
    opterr = 0;

    char validOp[] = {"i:o:s:c:p:t:f:"};
    while ((c = getopt (argc, argv, validOp)) != -1){
        switch (c){
            case 'i':
                parameters.i = true;
                parameters.inputFile = optarg;
                break;
            case 'o':
                parameters.o = true;
                parameters.outputFile = optarg;
                break;
            case 's':
                parameters.s = true;
                parameters.characterSize = atoi(optarg);
                break;
            case 'c':
                parameters.c = true;
                parameters.characterUtf8Charset = optarg;
                break;
            case 'p':
                parameters.p = true;
                parameters.spaceBetweenChar = atoi(optarg);
                break;
            case 't':
                parameters.t = true;
                parameters.outlineThickness = atof(optarg);
                break;
            case 'f':
                parameters.faceToSelect = atoi(optarg);
                break;
            case '?':
                if (optopt == 'i')
                    fprintf (stderr, "Option -i requires an argument <input file>.\n");
                else if (optopt == 'o')
                    fprintf (stderr, "Option -o requires an argument <output file>.\n");
                else if (optopt == 's')
                    fprintf (stderr, "Option -s requires an argument <character size in pixels>.\n");
                else if (optopt == 'c')
                    fprintf (stderr, "Option -c requires an argument <input charset utf8>.\n");
                else if (optopt == 'p')
                    fprintf (stderr, "Option -p requires an argument <pixel between char>.\n");
                else if (optopt == 't')
                    fprintf (stderr, "Option -t requires an argument <outline thickness>.\n");
                else if (optopt == 'f')
                    fprintf (stderr, "Option -f requires an argument <face to select>.\n");
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
                break;
            default:
                //error -- unknown parameter
                break;
        }
    }

    //debug
#define DEBUG 0

#if DEBUG == 1
    aRibeiro::PlatformPath::setWorkingPath(aRibeiro::PlatformPath::getExecutablePath(argv[0]));

    parameters.i = parameters.o = parameters.s = parameters.c = parameters.p = parameters.t = true;

    parameters.inputFile = "Roboto-Regular.ttf";
    parameters.outputFile = "Roboto-Regular";
    parameters.characterUtf8Charset = "charset.utf8";
    parameters.characterSize = 80;
    parameters.spaceBetweenChar = 6;
    parameters.faceToSelect = 0;
    parameters.outlineThickness = 1.0f;

#endif

    if (!parameters.i&&!parameters.o&&!parameters.s&&!parameters.c&&!parameters.p&&!parameters.t){
        fprintf (stdout,
            "Usage: font2bitmap -i ... -f ... -o ... -s ... -c ... -p ... -t ...\n"
            "-i : input file(.ttf|.otf|freetype compatible font)\n"
            "-f : face in font to be selected (default: 0)\n"
            "-o : output file without extension\n"
            "-s : character size in pixels to be exported (minimum:14).\n"
            "-c : text file with all characters to be generated in utf8.\n"
            "-p : size in pixels of the space between the characters exported.\n"
            "-t : outline thickness real number.\n"
        );
    }else if (!parameters.i){
        fprintf (stderr, "Missing option -i <input file>.\n");
    }else if (!parameters.o){
        fprintf (stderr, "Missing option -o <output file>.\n");
    }else if (!parameters.s){
        fprintf (stderr, "Missing option -s <character size in pixels>.\n");
    }else if (!parameters.c){
        fprintf (stderr, "Missing option -c <input charset utf8 file>.\n");
    }else if (!parameters.p){
        fprintf (stderr, "Missing option -p <pixel between char>.\n");
    }else if (!parameters.t){
        fprintf (stderr, "Missing option -t <outline thickness>.\n");
    }else{

        FontWriter fontWriter;

        //load utf8 data
        UTF8data * fileContent = UTF8data::readFromFile(parameters.characterUtf8Charset.c_str());
        UTF32data *utf32data = new UTF32data(fileContent);
        delete fileContent;
        UTF32* char_array = utf32data->begin();


        //configura FT2 and Atlas
        FT2 ft2;
        ft2.readFromFile_PX(parameters.inputFile,
                            parameters.faceToSelect,
                            parameters.outlineThickness,
                            parameters.characterSize,
                            parameters.characterSize);

        float newLineHeight = (float)parameters.characterSize;

        float spaceWidth = (float)parameters.characterSize;
        FT2Glyph *glyph_white_space = ft2.generateGlyph(L' ');
        if (glyph_white_space != NULL) {
            spaceWidth = glyph_white_space->advancex;
            ft2.releaseGlyph(&glyph_white_space);
        }

        Atlas atlas(parameters.spaceBetweenChar,parameters.spaceBetweenChar);

        fontWriter.initFromAtlas(&atlas, parameters.characterSize, spaceWidth, newLineHeight);

        //generate all characters
        for (int i=0;i<utf32data->count();i++){
            if (char_array[i] == L' ')
                continue;

            FT2Glyph *glyph = ft2.generateGlyph(char_array[i]);
            if (glyph == NULL){
                printf("Glyph not found: %u\n", char_array[i]);
                continue;
            }

            AtlasElement* atlasElementFace = atlas.addElement(UInt32toStringHEX(char_array[i]),
                                                            glyph->normalRect.w,
                                                            glyph->normalRect.h);

            atlasElementFace->copyFromRGBABuffer(glyph->bitmapRGBANormal, glyph->normalRect.w * 4);

            AtlasElement* atlasElementStroke = atlas.addElement(
                UInt32toStringHEX(char_array[i]) + std::string("s"),
                glyph->strokeRect.w,
                glyph->strokeRect.h
            );

            atlasElementStroke->copyFromRGBABuffer(glyph->bitmapRGBAStroke, glyph->strokeRect.w * 4);

            fontWriter.setCharacter(
                char_array[i],
                glyph->advancex,
                glyph->normalRect.top,
                glyph->normalRect.left,
                atlasElementFace,
                glyph->strokeRect.top,
                glyph->strokeRect.left,
                atlasElementStroke);

            ft2.releaseGlyph(&glyph);
        }

        atlas.organizePositions(false);


        atlas.savePNG((parameters.outputFile +
            std::string("-") +
            UInt32toString(parameters.characterSize) +
            std::string("-atlas.png")).c_str());
        atlas.savePNG_Alpha((parameters.outputFile +
            std::string("-") +
            UInt32toString(parameters.characterSize) +
            std::string("-atlas-gray.png")).c_str());

        fontWriter.saveGlyphTable(
            (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string(".asbgt2")).c_str());

        fontWriter.save(
            (parameters.outputFile +
            std::string("-") +
            UInt32toString(parameters.characterSize) +
            std::string(".basof2")).c_str());


        //clean up
        //delete utf32data;

        //
        // save old basof file format
        //
        {
            FT2Configuration configuration;
            //config the generator
            configuration.m_char_max_height = parameters.characterSize;
            configuration.m_char_max_width = parameters.characterSize;
            configuration.m_face_to_select = parameters.faceToSelect;

            PixelFont* generatedFont = PixelFontFactory::generatePixelFont(parameters.inputFile.c_str(),
                configuration, utf32data->begin(), utf32data->count(), false);

            delete utf32data;

            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportGLPack(
                (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string(".basof")).c_str()
            );

            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportSFont(
                (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string(".meta")).c_str(),
                (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string("-meta-atlas.png")).c_str(),
                (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string("-meta-atlas-gray.png")).c_str()
            );

            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportASilvaBinTable(
                (parameters.outputFile +
                std::string("-") +
                UInt32toString(parameters.characterSize) +
                std::string(".asbgt")).c_str(),
                NULL,
                NULL
            );


            /*
            //generate the files
            if (parameters.mode == "s") {
                generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportSFont(
                    (parameters.outputFile + ".meta").c_str(),
                    (parameters.outputFile + "-color.png").c_str(),
                    (parameters.outputFile + "-gray.png").c_str()
                );
            }
            else if (parameters.mode == "opengl") {
                generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportGLPack(
                    (parameters.outputFile + ".basof").c_str()
                );
            }
            else if (parameters.mode == "mixed") {
                generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportASilvaBinTable(
                    (parameters.outputFile + ".asbgt").c_str(),
                    (parameters.outputFile + "-color.png").c_str(),
                    (parameters.outputFile + "-gray.png").c_str()
                );
            }
            */

            delete generatedFont;
        }


        /*
        FT2Configuration configuration;
        //config the generator
        configuration.m_char_max_height = parameters.characterSize;
        configuration.m_char_max_width = parameters.characterSize;
        configuration.m_face_to_select = parameters.faceToSelect;

        PixelFont* generatedFont = PixelFontFactory::generatePixelFont(parameters.inputFile.c_str(),
            configuration,utf32data->begin(),utf32data->count(),parameters.v);
        delete utf32data;

        //generate the files
        if (parameters.mode == "s"){
            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportSFont(
                (parameters.outputFile + ".meta").c_str(),
                (parameters.outputFile + "-color.png").c_str(),
                (parameters.outputFile + "-gray.png").c_str()
            );
        }else if (parameters.mode == "opengl"){
            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportGLPack(
                (parameters.outputFile+".basof").c_str()
            );
        }else if (parameters.mode == "mixed"){
            generatedFont->getTextureFontRenderer(parameters.spaceBetweenChar).exportASilvaBinTable(
                (parameters.outputFile + ".asbgt").c_str(),
                (parameters.outputFile + "-color.png").c_str(),
                (parameters.outputFile + "-gray.png").c_str()
            );
        }
        delete generatedFont;
        */
    }

    //getc(stdin);
    return 0;
}
