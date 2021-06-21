#ifndef model_texture_h_
#define model_texture_h_

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

namespace model {

    enum TextureType
    {
        TextureType_NONE = 0x0,
        TextureType_DIFFUSE = 0x1,
        TextureType_SPECULAR = 0x2,
        TextureType_AMBIENT = 0x3,
        TextureType_EMISSIVE = 0x4,
        TextureType_HEIGHT = 0x5,
        TextureType_NORMALS = 0x6,
        TextureType_SHININESS = 0x7,
        TextureType_OPACITY = 0x8,
        TextureType_DISPLACEMENT = 0x9,
        TextureType_LIGHTMAP = 0xa,
        TextureType_REFLECTION = 0xb,
        TextureType_UNKNOWN = 0xc
    };

    static const char* TextureTypeToStr(TextureType type) {
        switch(type){
            case TextureType_NONE: return "none";
            case TextureType_DIFFUSE: return "diffuse";
            case TextureType_SPECULAR: return "specular";
            case TextureType_AMBIENT: return "ambient";
            case TextureType_EMISSIVE: return "emissive";
            case TextureType_HEIGHT: return "height";
            case TextureType_NORMALS: return "normals";
            case TextureType_SHININESS: return "shininess";
            case TextureType_OPACITY: return "opacity";
            case TextureType_DISPLACEMENT: return "displacement";
            case TextureType_LIGHTMAP: return "lightmap";
            case TextureType_REFLECTION: return "reflection";
            case TextureType_UNKNOWN: return "unknown";
        }
        return "error to parse";
    }

    enum TextureOp
    {
        // T = T1 * T2
        TextureOp_Multiply = 0x0,
        // T = T1 + T2
        TextureOp_Add = 0x1,
        // T = T1 - T2
        TextureOp_Subtract = 0x2,
        // T = T1 / T2
        TextureOp_Divide = 0x3,
        // T = (T1 + T2) - (T1 * T2)
        TextureOp_SmoothAdd = 0x4,
        // T = T1 + (T2-0.5)
        TextureOp_SignedAdd = 0x5,

        TextureOp_Default = -1 //32766
    };

    static const char* TextureOpToStr(TextureOp op) {
        switch(op){
            case TextureOp_Multiply: return "multiply";
            case TextureOp_Add: return "add";
            case TextureOp_Subtract: return "subtract";
            case TextureOp_Divide: return "divide";
            case TextureOp_SmoothAdd: return "smooth add";
            case TextureOp_SignedAdd: return "signed add";
            case TextureOp_Default: return "default";
        }
        return "error to parse";
    }

    enum TextureMapMode
    {
        // A texture coordinate u|v is translated to u%1|v%1
        TextureMapMode_Wrap = 0x0,
        
        // Texture coordinates outside [0...1]
        //  are clamped to the nearest valid value.
        TextureMapMode_Clamp = 0x1,
        
        // If the texture coordinates for a pixel are outside [0...1]
        //  the texture is not applied to that pixel
        TextureMapMode_Decal = 0x3,
        
        // A texture coordinate u|v becomes u%1|v%1 if (u-(u%1))%2 is zero and
        //  1-(u%1)|1-(v%1) otherwise
        TextureMapMode_Mirror = 0x2,
    };

    static const char* TextureMapModeToStr(TextureMapMode map) {
        switch(map){
            case TextureMapMode_Wrap: return "wrap";
            case TextureMapMode_Clamp: return "clamp";
            case TextureMapMode_Decal: return "decal";
            case TextureMapMode_Mirror: return "mirror";
        }
        return "error to parse";
    }

    class _SSE2_ALIGN_PRE Texture {
    public:
        std::string filename;
        std::string fileext;
        TextureType type;
        TextureOp op;
        TextureMapMode mapMode;
        
        int uvIndex;

        void write(aRibeiro::BinaryWriter* writer)const {
            writer->writeString(filename);
            writer->writeString(fileext);
            writer->writeUInt8(type);
            writer->writeUInt8(op);
            writer->writeUInt8(mapMode);
            writer->writeUInt32(uvIndex);
        }

        void read(aRibeiro::BinaryReader* reader) {
            filename = reader->readString();
            fileext = reader->readString();
            type = (TextureType)reader->readUInt8();
            op = (TextureOp)reader->readUInt8();
            mapMode = (TextureMapMode)reader->readUInt8();
            uvIndex = reader->readUInt32();
        }

        Texture() {
            type = TextureType_NONE;
            op = TextureOp_Multiply;
            mapMode = TextureMapMode_Wrap;
            uvIndex = 0;
        }
        
        Texture(const Texture& v) {
            (*this) = v;
        }
        
        void operator=(const Texture& v) {
            filename = v.filename;
            fileext = v.fileext;
            type = v.type;
            uvIndex = v.uvIndex;
            op = v.op;
            mapMode = v.mapMode;
        }
        
        SSE2_CLASS_NEW_OPERATOR
        
    }_SSE2_ALIGN_POS;

}

#endif