#ifndef configFile_h
#define configFile_h

#include "../SysHelper/TinyXML/all.h"

class ConfigFile {
private:
    TiXmlElement* applicationConfig;
    TiXmlDocument doc;
    TiXmlElement* getBranch(const char* branch);
    bool changed;
public:
    ConfigFile(const char* file);
    ~ConfigFile();
    float getFloat(const char* element, const float defaultValue);
    int getInt(const char* element, const int defaultValue);
    const char* getString(const char* element, const char* defaultValue);
    void setFloat(const char* element, const float v);
    void setInt(const char* element, const int v);
    void setString(const char* element, const char* v);

    //one level parameter
    float getFloat(const char* branch, const char* element, const float defaultValue);
    int getInt(const char* branch, const char* element, const int defaultValue);
    const char* getString(const char* branch, const char* element, const char* defaultValue);
    void setFloat(const char* branch, const char* element, const float v);
    void setInt(const char* branch, const char* element, const int v);
    void setString(const char* branch, const char* element, const char* v);

    void save();
};

#endif
