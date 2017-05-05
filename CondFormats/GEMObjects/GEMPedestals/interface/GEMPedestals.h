#ifndef GEMPedestals_h
#define GEMPedestals_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include "CondFormats/GEMPedestals/interface/GEMPedestal.h"
#include <memory>


/*
 *I'm sending ParameterSet to constructor. Because I need it in implementation.

 **/
class GEMPedestals : public edm::ESProducer{
    public:
        GEMPedestals(const edm::ParameterSet&);
        ~GEMPedestals();
    
        void produce(){};
   protected:
    /*
     * 
     *  unique_ptr help us to create new GEMPedestal object.
     **/
    std::unique_ptr<GEMPedestal> producePedestals(const GEMPedestalsRcd& rcd);

};
#endif

