#include <aribeiro/common.h>
#include "configFile.h"

/*
#define EXCEPT(message)                     \
{                                           \
  fprintf(stderr, "Error in file: [%s line %d] ->> %s\n",  \
          __FILE__, __LINE__, message);     \
  exit(-1); \
}
*/

#define EXCEPT(message)\
    ARIBEIRO_ABORT(true,"%s\n",message);

#define DEBUG_PRINT(msg)



ConfigFile::ConfigFile(const char* file) :doc(file) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: Filename \"%s\"", file);
    DEBUG_PRINT(tmp);
#endif
    applicationConfig = NULL;
    if (doc.LoadFile() != 0) {
        if (doc.Error() != 0)
            EXCEPT(doc.ErrorDesc());
        applicationConfig = doc.FirstChildElement("ApplicationConfig");
        DEBUG_PRINT("[ConfigFile]: config document opened");
    }
#ifdef USING_DEBUG
    else {
        DEBUG_PRINT("[ConfigFile]: config file not found or not can be readed");
    }
#endif

    if (applicationConfig == NULL) {
        DEBUG_PRINT("[ConfigFile]: Application info cannot be found in file, creating a new");
        doc.Clear();
        applicationConfig = (TiXmlElement*)doc.InsertEndChild(TiXmlElement("ApplicationConfig"));
        if (applicationConfig == NULL) {
            EXCEPT("Error to create new application info");
        }
    }
    changed = false;
}
ConfigFile::~ConfigFile() {
    save();
}



TiXmlElement* ConfigFile::getBranch(const char* branch) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    TiXmlElement* retorno = applicationConfig->FirstChildElement(branch);
    if (retorno == NULL) {
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: Branch NOT FOUND \"%s\"!! Try to create one", branch);
        DEBUG_PRINT(tmp);
#endif
        retorno = (TiXmlElement*)applicationConfig->InsertEndChild(TiXmlElement(branch));
        if (retorno == NULL) {
            EXCEPT("Error to create new branch info");
        }
#ifdef USING_DEBUG
        else {
            sprintf(tmp, "[ConfigFile]: Branch Created!!! (\"%s\")", branch);
            DEBUG_PRINT(tmp);
        }
#endif
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: Branch FOUND \"%s\"", branch);
        DEBUG_PRINT(tmp);
    }
#endif
    return retorno;
}

float ConfigFile::getFloat(const char* element, const float defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    float r;
    if (applicationConfig->QueryFloatAttribute(element, &r) != TIXML_SUCCESS) {
        applicationConfig->SetDoubleAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  float \"%s\" NOT FOUND! (seting to default = %f)", element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute float \"%s\" FOUND! (v = %f)", element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
int ConfigFile::getInt(const char* element, const int defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    int r;
    if (applicationConfig->QueryIntAttribute(element, &r) != TIXML_SUCCESS) {
        applicationConfig->SetAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  int \"%s\" NOT FOUND! (seting to default = %d)", element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute int \"%s\" FOUND! (v = %d)", element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
const char* ConfigFile::getString(const char* element, const char* defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    const char* r = applicationConfig->Attribute(element);
    if (r == NULL) {
        applicationConfig->SetAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  string \"%s\" NOT FOUND! (seting to default = \"%s\")", element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute string \"%s\" FOUND! (v = \"%s\")", element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
void ConfigFile::setFloat(const char* element, const float v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute float \"%s\" to \"%f\"", element, v);
    DEBUG_PRINT(tmp);
#endif
    applicationConfig->SetDoubleAttribute(element, v);
    changed = true;
}
void ConfigFile::setInt(const char* element, const int v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute int \"%s\" to \"%d\"", element, v);
    DEBUG_PRINT(tmp);
#endif
    applicationConfig->SetAttribute(element, v);
    changed = true;
}
void ConfigFile::setString(const char* element, const char* v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute string \"%s\" to \"%s\"", element, v);
    DEBUG_PRINT(tmp);
#endif
    applicationConfig->SetAttribute(element, v);
    changed = true;
}




float ConfigFile::getFloat(const char* branch, const char* element, const float defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    float r;
    TiXmlElement* brnch = getBranch(branch);
    if (brnch->QueryFloatAttribute(element, &r) != TIXML_SUCCESS) {
        brnch->SetDoubleAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  float \"%s:%s\" NOT FOUND! (seting to default = %f)", branch, element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute float \"%s:%s\" FOUND! (v = %f)", branch, element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
int ConfigFile::getInt(const char* branch, const char* element, const int defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    int r;
    TiXmlElement* brnch = getBranch(branch);
    if (brnch->QueryIntAttribute(element, &r) != TIXML_SUCCESS) {
        brnch->SetAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  int \"%s:%s\" NOT FOUND! (seting to default = %d)", branch, element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute int \"%s:%s\" FOUND! (v = %d)", branch, element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
const char* ConfigFile::getString(const char* branch, const char* element, const char* defaultValue) {
#ifdef USING_DEBUG
    char tmp[1024];
#endif
    TiXmlElement* brnch = getBranch(branch);
    const char* r = brnch->Attribute(element);
    if (r == NULL) {
        brnch->SetAttribute(element, defaultValue);
        r = defaultValue;
#ifdef USING_DEBUG
        sprintf(tmp, "[ConfigFile]: attribute  string \"%s:%s\" NOT FOUND! (seting to default = \"%s\")", branch, element, r);
        DEBUG_PRINT(tmp);
#endif
        changed = true;
    }
#ifdef USING_DEBUG
    else {
        sprintf(tmp, "[ConfigFile]: attribute string \"%s:%s\" FOUND! (v = \"%s\")", branch, element, r);
        DEBUG_PRINT(tmp);
    }
#endif
    return r;
}
void ConfigFile::setFloat(const char* branch, const char* element, const float v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute float \"%s:%s\" to \"%f\"", branch, element, v);
    DEBUG_PRINT(tmp);
#endif
    getBranch(branch)->SetDoubleAttribute(element, v);
    changed = true;
}
void ConfigFile::setInt(const char* branch, const char* element, const int v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute int \"%s:%s\" to \"%d\"", branch, element, v);
    DEBUG_PRINT(tmp);
#endif
    getBranch(branch)->SetAttribute(element, v);
    changed = true;
}
void ConfigFile::setString(const char* branch, const char* element, const char* v) {
#ifdef USING_DEBUG
    char tmp[1024];
    sprintf(tmp, "[ConfigFile]: seting attribute string \"%s:%s\" to \"%s\"", branch, element, v);
    DEBUG_PRINT(tmp);
#endif
    getBranch(branch)->SetAttribute(element, v);
    changed = true;
}

void ConfigFile::save() {
    if (changed) {
        DEBUG_PRINT("[ConfigFile]: Save file");
        doc.SaveFile();
        changed = false;
    }
#ifdef USING_DEBUG
    else {
        DEBUG_PRINT("[ConfigFile]: file dont need to be saved");
    }
#endif
}
