#ifndef BCM_HOST_INIT_H
#define BCM_HOST_INIT_H

class BMCHost {
    bool initialized;
    BMCHost();
public:
    static BMCHost* Instance();
    void init();
};

#endif