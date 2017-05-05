#ifndef GEMPedestal_h
#define GEMPedestal_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include <string>


/*
 *  As I understand - here I describe condition object.
 *  I will describe objects' structure. In this example there are two variables - var and var2.
 *  Also there are constructor and destructor.
 *
 * */

class GEMPedestal{
    public:
        GEMPedestal();
        ~GEMPedestal();

    struct Item{
        std::string var;
        std::string var2;
        COND_SERIALIZABLE;
    };
};
#endif

