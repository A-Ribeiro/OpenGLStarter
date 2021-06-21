#include "Bams.h"


BamsData_POS_COL* Bams::Geometry::getBamsData_POS_COL(){
	if (mHeader.vertexFormat == (BAMS_POS | BAMS_COL))
		return (BamsData_POS_COL*)mData;
	return 0;
}

BamsData_POS* Bams::Geometry::getBamsData_POS(){
	if (mHeader.vertexFormat == (BAMS_POS))
		return (BamsData_POS*)mData;
	return 0;
}

void Bams::Geometry::setHeader(const BamsChunkGeometry& header){
	if (mData)
		delete[] mData;
	mHeader = header;
	mData = new char[mHeader.nextChunkOffset];
	memset(mData,0,mHeader.nextChunkOffset);//pode ser ignorado se tudo funfar certinho
}
		
Bams::Geometry::Geometry(const BamsChunkGeometry& header){
	mData = 0;
	setHeader(header);
}

Bams::Geometry::Geometry(){
	memset(&mHeader,0,sizeof(BamsChunkGeometry));
	mData = 0;
}

Bams::Geometry& Bams::Geometry::operator=(const Geometry& v){
	if (mData)
		delete[] mData;
	mHeader = v.mHeader;
	mData = new char[mHeader.nextChunkOffset];
	memcpy(mData,v.mData,mHeader.nextChunkOffset);

	return *this;
}
Bams::Geometry::Geometry(const Geometry& v){
	mData = 0;
	*this = v;
}
Bams::Geometry::~Geometry(){
	if (mData)
		delete[] mData;
	mData = 0;
	memset(&mHeader,0,sizeof(BamsChunkGeometry));
}




Bams::Geometry* Bams::getGeometry(const char* name){
	if (mGeometry.find(name) != mGeometry.end())
		return &mGeometry[name];
	return 0;
}
	
void Bams::addGeometry(const Geometry& geometry){
	if (mGeometry.find(geometry.mHeader.name)!=mGeometry.end()){
		printf("[Bams]ERROR -- inserting geometry with the same name __ only the last inserted will be valid...\n");
	}
	mGeometry[geometry.mHeader.name] = geometry;
}

void Bams::loadFromFile(const char* filename){
	FILE* in = fopen(filename,"rb");
	if (in){
		//clear all
		mGeometry.clear();

		BamsFileHeader bamsFileHeader;
		fread(&bamsFileHeader,sizeof(BamsFileHeader),1,in);
		//load all animations
		for (unsigned int i=0;i<bamsFileHeader.animationCount;i++){
			BamsChunkAnimation bamsChunkAnimation;
			fread(&bamsChunkAnimation,sizeof(BamsChunkAnimation),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkAnimation.nextChunkOffset,SEEK_CUR);
		}
		//load all geometries
		for (unsigned int i=0;i<bamsFileHeader.geometryCount;i++){
			BamsChunkGeometry bamsChunkGeometry;
			fread(&bamsChunkGeometry,sizeof(BamsChunkGeometry),1,in);
			if (mGeometry.find(bamsChunkGeometry.name)!=mGeometry.end())
				printf("[Bams]ERROR -- inserting geometry with the same name __ only the last inserted will be valid...\n");
			mGeometry[bamsChunkGeometry.name].setHeader(bamsChunkGeometry);
			fread(mGeometry[bamsChunkGeometry.name].mData,1,bamsChunkGeometry.nextChunkOffset,in);
		}
		//load all materials
		for (unsigned int i=0;i<bamsFileHeader.materialCount;i++){
			BamsChunkMaterial bamsChunkMaterial;
			fread(&bamsChunkMaterial,sizeof(BamsChunkMaterial),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkMaterial.nextChunkOffset,SEEK_CUR);
		}
		//load all scenes
		for (unsigned int i=0;i<bamsFileHeader.sceneCount;i++){
			BamsChunkScene bamsChunkScene;
			fread(&bamsChunkScene,sizeof(BamsChunkScene),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkScene.nextChunkOffset,SEEK_CUR);
		}
		fclose(in);
	}
}

void Bams::saveToFile(const char* filename){
	FILE* out = fopen(filename,"wb");
	if (out){
		BamsFileHeader bamsFileHeader;
		memcpy(bamsFileHeader.fileID,".asilva...bams..",16);
		bamsFileHeader.version = 1;

		bamsFileHeader.animationCount = 0;
		bamsFileHeader.geometryCount = mGeometry.size();
		bamsFileHeader.materialCount = 0;
		bamsFileHeader.sceneCount = 0;

		fwrite(&bamsFileHeader,sizeof(BamsFileHeader),1,out);

		//write all animations
		//write all geometries
		{
			std::map<std::string,Geometry>::iterator it = mGeometry.begin();
			while (it != mGeometry.end()){
				const Geometry& geometry = it->second;
				fwrite(&geometry.mHeader,sizeof(BamsChunkGeometry),1,out);
				fwrite(geometry.mData,sizeof(char),geometry.mHeader.nextChunkOffset,out);
				it++;
			}
		}
		//write all materials
		//write all scenes
		fclose(out);
	}
}


Bams::Geometry* Bams::findGeometry(const char* filename,const char* geometryName){
	FILE* in = fopen(filename,"rb");
	if (in){
		BamsFileHeader bamsFileHeader;
		fread(&bamsFileHeader,sizeof(BamsFileHeader),1,in);
		//load all animations
		for (unsigned int i=0;i<bamsFileHeader.animationCount;i++){
			BamsChunkAnimation bamsChunkAnimation;
			fread(&bamsChunkAnimation,sizeof(BamsChunkAnimation),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkAnimation.nextChunkOffset,SEEK_CUR);
		}
		//load all geometries
		for (unsigned int i=0;i<bamsFileHeader.geometryCount;i++){
			BamsChunkGeometry bamsChunkGeometry;
			fread(&bamsChunkGeometry,sizeof(BamsChunkGeometry),1,in);
			if (strcmp(geometryName,bamsChunkGeometry.name) == 0){
				Bams::Geometry* result = new Bams::Geometry(bamsChunkGeometry);
				fread(result->mData,1,bamsChunkGeometry.nextChunkOffset,in);
				fclose(in);
				return  result;
			}
			fseek(in,bamsChunkGeometry.nextChunkOffset,SEEK_CUR);
		}
		/*
		//load all materials
		for (unsigned int i=0;i<bamsFileHeader.materialCount;i++){
			BamsChunkMaterial bamsChunkMaterial;
			fread(&bamsChunkMaterial,sizeof(BamsChunkMaterial),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkMaterial.nextChunkOffset,SEEK_CUR);
		}
		//load all scenes
		for (unsigned int i=0;i<bamsFileHeader.sceneCount;i++){
			BamsChunkScene bamsChunkScene;
			fread(&bamsChunkScene,sizeof(BamsChunkScene),1,in);
			//ignoring chunk content
			fseek(in,bamsChunkScene.nextChunkOffset,SEEK_CUR);
		}
		*/
		fclose(in);
	}
	return 0;
}
