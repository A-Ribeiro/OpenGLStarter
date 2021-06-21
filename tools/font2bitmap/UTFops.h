#ifndef utfOps___
#define utfOps___

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <convertutf/ConvertUTF.h>
#include "assert.h"
#include "memory.h"



class UTF8data{

    UTF8 *mData;
    int mCount;

    void release(){
        if (mData){
            delete[] mData;
            mData = NULL;
        }
        mCount = 0;
    }
public:
    void operator = (const UTF8data* v){
        release();
        mData = new UTF8[v->mCount];
        memcpy(mData,v->mData,v->mCount*sizeof(UTF8));
        mCount = v->mCount;
    }
    UTF8data(const UTF8data* v){
        (*this)=v;
    }

    UTF8data(){
        mData = 0;
        mCount = 0;
    }

    virtual ~UTF8data(){
        release();
    }

    //bytes - for utf8
    void setBufferCount(int s){
        release();
        mData = new UTF8[s];
        mCount = s;
    }

    int count()const {
        return mCount;
    }

    UTF8* begin()const {
        return mData;
    }

    UTF8* end()const {
        return &mData[mCount-1];
    }

    static UTF8data* readFromFile(const char* filename) {
        UTF8data* result = new UTF8data();
        FILE* input = fopen(filename,"rb");
        if (input){
            fseek(input,0,SEEK_END);
            result->setBufferCount(ftell(input)+2);
            fseek(input,0,SEEK_SET);
            assert(sizeof(UTF8) == 1);
            memset(result->begin(),0,result->count()*sizeof(UTF8));
            fread(result->begin(),sizeof(UTF8),result->count()-2,input);
            fclose(input);
        }
        return result;
    }
};


class UTF32data{
    
    UTF32 *mData;
    int mCount;

    void release(){
        if (mData){
            delete[] mData;
            mData = NULL;
        }
        mCount = 0;
    }

public:
    void operator = (const UTF32data* v){
        release();
        mData = new UTF32[v->mCount];
        memcpy(mData,v->mData,v->mCount*sizeof(UTF32));
        mCount = v->mCount;
    }
    UTF32data(const UTF32data* v){
        (*this)=v;
    }

    UTF32data(){
        mData = 0;
        mCount = 0;
    }

    virtual ~UTF32data(){
        release();
    }

    void setBufferCount(int s){
        release();
        mData = new UTF32[s];
        mCount = s;
    }

    int count()const {
        return mCount-1;//remove the '\0' entry from the count
    }

    UTF32* begin()const {
        return mData;
    }

    UTF32* end()const {
        return &mData[mCount-1];
    }

    UTF32data(const UTF8data* utf8){
        mData = 0;
        mCount = 0;
        setBufferCount(utf8->count());
        const UTF8* begin = utf8->begin();
        UTF32* tbegin = this->begin();
        UTF32* tend = this->end();
        ConvertUTF8toUTF32(&begin,utf8->end(),&tbegin,tend, lenientConversion );
        mCount = tbegin - mData;
    }

};

#endif
