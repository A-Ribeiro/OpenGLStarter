#include "Atlas.h"

namespace DataModel {
    
    //private copy constructores, to avoid copy...
    Atlas::Atlas(const Atlas& v) {
    }
    
    void Atlas::operator=(const Atlas& v){
    }
    
    void Atlas::resetPositions(){
        for(size_t i=0;i<elements.size();i++){
            elements[i]->rect.x = xspacing;
            elements[i]->rect.y = yspacing;
        }
    }

    bool Atlas::colideWithAnyObject(AtlasElement* base_element, int maxIterate) {
        for (size_t i = 0; i < maxIterate; i++) {
            AtlasElement* element = elements[i];
            if (base_element->rect.overlaps(element->rect, xspacing, yspacing))
                return true;
        }
        return false;
    }

    std::vector<AtlasRect> Atlas::computeInsertPositionArray(const AtlasRect &screen, int currY, int maxIterate) {
        std::vector<AtlasRect> result;

        result.push_back(AtlasRect(xspacing / 2, currY,0,0));

        int xmax = screen.w;

        AtlasRect overlapTest(0, currY, screen.w, 1);

        for (size_t i = 0; i < maxIterate; i++) {
            AtlasElement* element = elements[i];
            if (!overlapTest.overlaps(element->rect, xspacing, yspacing))
                continue;

            result.push_back(AtlasRect(element->rect.x + element->rect.w + xspacing, currY, 0, 0));
        }

        return result;
    }
    
    AtlasElement* Atlas::findMinY(const AtlasRect &screen, int currY, int maxIterate){
        
        if (currY + yspacing >= screen.h)
            return NULL;
        
        AtlasElement* result = NULL;
        
        int ymax = screen.h;
        
        AtlasRect overlapTest(0,currY,screen.w,1);
        
        for(size_t i=0;i<maxIterate;i++){
            AtlasElement* element = elements[i];
            if (!overlapTest.overlaps(element->rect,xspacing,yspacing))
                continue;
            
            int ymax_aux = element->rect.y + element->rect.h + yspacing;
            if (ymax_aux < ymax){
                ymax = ymax_aux;
                result = element;
            }
        }
        
        return result;
    }
    
    bool Atlas::repositionAllElements(const AtlasRect &screen, bool fastMode) {
        
        if (elements.size()==0)
            return true;

        std::vector<AtlasRect> possibleInsertPositions;

        int currY = yspacing / 2;

        for(size_t i=0;i<elements.size();i++){

            if (!fastMode)
                currY = yspacing / 2;

            AtlasElement *element = elements[i];
            possibleInsertPositions = computeInsertPositionArray(screen, currY, i);

            bool foundPlace = false;
            while (!foundPlace) {
                //each position test overlap with other objects
                for (size_t j = 0; j < possibleInsertPositions.size(); j++) {
                    AtlasRect &insertPos = possibleInsertPositions[j];
                    element->rect.setXY(insertPos.x, insertPos.y);
                    if (!colideWithAnyObject(element, i) && element->rect.inside(screen, xspacing / 2, yspacing / 2)) {
                        foundPlace = true;
                        break;
                    }
                }

                if (!foundPlace) {
                    AtlasElement* minY = findMinY(screen, currY, i);
                    if (minY == NULL)
                        return false;
                    currY = minY->rect.maxYInclusive() + 1 + yspacing;

                    possibleInsertPositions = computeInsertPositionArray(screen, currY, i);
                }
            }
        }
        return true;
    }
    
    void Atlas::clearElements() {
        for(size_t i=0;i<elements.size();i++){
            delete elements[i];
        }
        elements.clear();
    }
    
    Atlas::Atlas(int _xspacing, int _yspacing){
        xspacing = _xspacing;
        yspacing = _yspacing;
        
        if (xspacing % 2 == 1)
            xspacing++;
        if (yspacing % 2 == 1)
            yspacing++;
        
    }
    
    Atlas::~Atlas(){
        clearElements();
    }
    
    AtlasElement* Atlas::addElement(const std::string &name, int w, int h) {
        AtlasElement* result = new AtlasElement(w,h);
        result->name = name;
        elements.push_back(result);
        return result;
    }
    
    void Atlas::organizePositions(bool fastMode){
        
        AtlasRect res(128, 128);
        
        int sideToIncrease = 0;
        while (!repositionAllElements(res, fastMode)){
            if (sideToIncrease == 0)
                res.w = res.w << 1;
            else
                res.h = res.h << 1;
            sideToIncrease = (sideToIncrease+1)%2;
        }
        
        textureResolution = res;
        
        //change the texture increase pattern
        res = AtlasRect(128, 128);
        sideToIncrease = 1;
        
        while (!repositionAllElements(res, fastMode)){
            if (sideToIncrease == 0)
                res.w = res.w << 1;
            else
                res.h = res.h << 1;
            sideToIncrease = (sideToIncrease+1)%2;
        }
        
        if (res.w*res.h < textureResolution.w*textureResolution.h) {
            textureResolution = res;
        }

        repositionAllElements(textureResolution, fastMode);
    }
    
    uint8_t * Atlas::createRGBA()const{

        ARIBEIRO_ABORT((textureResolution.w == 0 || textureResolution.h == 0), "Error to create texture from atlas.\n");
        //if (textureResolution.w == 0 || textureResolution.h == 0){
        //    fprintf(stderr,"Error to create texture from atlas.\n");
        //    exit(-1);
        //}
        
        uint8_t *result = new uint8_t[textureResolution.w*textureResolution.h*4];

        //set all color to 0
        memset(result, 0, sizeof(uint8_t)*textureResolution.w*textureResolution.h * 4);

        /*
        for (int y = 0; y < textureResolution.h; y++) {
            for (int x = 0; x < textureResolution.w; x++) {
                result[(x + y * textureResolution.w) * 4 + 0] = 0;
                result[(x + y * textureResolution.w) * 4 + 1] = 0;
                result[(x + y * textureResolution.w) * 4 + 2] = 0;
                result[(x + y * textureResolution.w) * 4 + 3] = 0;
            }
        }
        */
        
        for(size_t i=0;i<elements.size();i++){
            AtlasElement *element = elements[i];
            element->copyToRGBABuffer(result, textureResolution.w * 4, xspacing / 2, yspacing / 2);
        }
        
        return result;
    }
    
    void Atlas::releaseRGBA(uint8_t **data)const {
        delete[] *data;
        *data = NULL;
    }

    uint8_t* Atlas::createA()const {

        ARIBEIRO_ABORT((textureResolution.w == 0 || textureResolution.h == 0), "Error to create texture from atlas.\n");
        //if (textureResolution.w == 0 || textureResolution.h == 0) {
        //    fprintf(stderr, "Error to create texture from atlas.\n");
        //    exit(-1);
        //}

        uint8_t *result = new uint8_t[textureResolution.w*textureResolution.h];

        //set all color to 0
        memset(result, 0, sizeof(uint8_t)*textureResolution.w*textureResolution.h);

        for (size_t i = 0; i < elements.size(); i++) {
            AtlasElement *element = elements[i];
            element->copyToABuffer(result, textureResolution.w, xspacing / 2, yspacing / 2);
        }

        return result;
    }
    void Atlas::releaseA(uint8_t **data)const {
        delete[] * data;
        *data = NULL;
    }
    
    void Atlas::write(aRibeiro::BinaryWriter *writer)const{
        writer->writeUInt32(elements.size());
        for(size_t i=0;i<elements.size();i++){
            writer->writeString(elements[i]->name);
            elements[i]->rect.write(writer);
        }
    }
    
    void Atlas::read(aRibeiro::BinaryReader *reader){
        clearElements();
        elements.resize(reader->readUInt32());
        for(size_t i=0;i<elements.size();i++){
            AtlasElement *element = new AtlasElement();
            element->read(reader);
            elements[i] = element;
        }
    }
    
    void Atlas::savePNG(const std::string &filename) const {
        uint8_t * image = createRGBA();
        aRibeiro::PNGHelper::writePNG(filename.c_str(), textureResolution.w, textureResolution.h, 4, (char*)image);
        releaseRGBA(&image);
    }
    
    void Atlas::savePNG_Alpha(const std::string &filename)const {
        uint8_t * image = createA();
        aRibeiro::PNGHelper::writePNG(filename.c_str(), textureResolution.w, textureResolution.h, 1, (char*)image);
        releaseRGBA(&image);
    }
    
    void Atlas::writeTable(const std::string &filename)const {
        aRibeiro::BinaryWriter writer;
        writer.writeToFile(filename.c_str());
        write(&writer);
        writer.close();
    }
}
