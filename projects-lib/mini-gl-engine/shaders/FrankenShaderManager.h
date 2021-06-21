#ifndef _FrankenShaderManager_h_
#define _FrankenShaderManager_h_

#include "FrankenShader.h"

namespace GLEngine {
    
    class FrankenShaderManager : public DefaultEngineShader {
        
        std::map<uint32_t, FrankenShader*> shaderMap;
        
    public:
        virtual ~FrankenShaderManager(){
            std::map<uint32_t, FrankenShader*>::iterator it;
            
            for ( it = shaderMap.begin(); it != shaderMap.end(); it++ )
            {
                FrankenShader* shader = it->second;
                delete shader;
            }
            
            shaderMap.clear();
        }
        
        FrankenShader* getShader(uint32_t frankenFormat){
            std::map<uint32_t, FrankenShader*>::iterator it = shaderMap.find(frankenFormat);
            if(it == shaderMap.end()){
                FrankenShader *result = new FrankenShader(frankenFormat);
                shaderMap[frankenFormat] = result;
                return result;
            }
            return it->second;
        }

    };
    
    
}

#endif
