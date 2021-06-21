#ifndef BAMS_H
#define BAMS_H

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <vector>

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

#include "Color.h"


//enums
enum NodeType{
	BAMS_ANIMATION,
	BAMS_GEOMETRY,
	BAMS_MATERIAL,
	BAMS_SCENE
};

enum GeometryType{
	BAMS_LINES,
	BAMS_LINE_STRIP,
	BAMS_LINE_LOOP,
	BAMS_POINTS,
	
	BAMS_TRIANGLES,
	BAMS_TRIANGLE_STRIP,
	BAMS_TRIANGLE_FAN,
	
	BAMS_QUADS,
	BAMS_QUAD_STRIP,
};

enum GeometryVertexFormat{
	BAMS_POS = 0x00000001, // float3 position
	BAMS_NOR = 0x00000002, // float3 normal
	BAMS_BIN = 0x00000004, // float3 binormal
	BAMS_TAN = 0x00000008, // float3 tangent
	BAMS_COL = 0x00000010, // unsigned byte 4 color
	BAMS_UV0 = 0x00000020, // float2 
	BAMS_UV1 = 0x00000040, // float2
	BAMS_UV2 = 0x00000080, // float2
	BAMS_UV3 = 0x00000100, // float2
	BAMS_UV4 = 0x00000200, // float2
	BAMS_UV5 = 0x00000400, // float2
	BAMS_UV6 = 0x00000800, // float2
	BAMS_UV7 = 0x00001000, // float2
	
};

#pragma pack(push)  //push current alignment to stack
#pragma pack(1)     //set alignment to 1 byte boundary

struct BamsFileHeader{
	//.asilva...bams..
	unsigned char fileID[16];
	unsigned int version;//1
	unsigned int animationCount;
	unsigned int geometryCount;
	unsigned int materialCount;
	unsigned int sceneCount;
	
};

struct BamsChunkAnimation{
	unsigned int nextChunkOffset;
};

//definicao de varios data pointers
struct BamsData_POS_COL{
    vec3 mPos;
	Color mColor;
};

struct BamsData_POS{
    vec3 mPos;
};

struct BamsChunkGeometry{
	unsigned char geometryType;
	unsigned int indexCount;//if indexCount == 0, then it has the geometry all in the vertex directly
	unsigned int vertexCount;
	char name[32];
	//BAMS_POS | BAMS_COL
	unsigned int vertexFormat;
	unsigned int nextChunkOffset;
};

struct BamsChunkMaterial{
	unsigned int nextChunkOffset;
};

struct BamsChunkScene{
	unsigned int nextChunkOffset;
};

#pragma pack(pop)   //restore original alignment from stack



class Bams{
public:
	class Geometry{
	public:
		BamsChunkGeometry mHeader;
		char* mData;
	public:
		Geometry();
		~Geometry();
		Geometry(const Geometry& v);
		Geometry(const BamsChunkGeometry& header);
		Geometry& operator=(const Geometry& v);
		BamsData_POS_COL* getBamsData_POS_COL();
		BamsData_POS* getBamsData_POS();
		void setHeader(const BamsChunkGeometry& header);
	};

private:
	std::map<std::string,Geometry> mGeometry;
public:
	void addGeometry(const Geometry& geometry);
	Geometry* getGeometry(const char* name);
	void loadFromFile(const char* filename);
	void saveToFile(const char* filename);

	//tries to find the geometry at this file, if it encounters is, create an instace of it.
	static Geometry* findGeometry(const char* filename,const char* geometryName);
};


#endif
