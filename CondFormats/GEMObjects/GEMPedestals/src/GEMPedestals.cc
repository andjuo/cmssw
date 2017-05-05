#include <memory>
#include <iostream>
#include <fstream>

#include "FWCore/Framework/interface/ESProducer.h"

#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Framework/interface/ValidityInterval.h"

#include "CondFormats/DataRecord/interface/GEMPedestalsRcd.h"

#include "CondFormats/GEMPedestals/interface/GEMPedestals.h"
#include "CondFormats/GEMPedestals/interface/GEMPedestal.h"

#include "DataFormats/FWLite/interface/IOVSyncValue.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

/*
 * In constructor method I invoke setWhatProduced, but before that i need to create unique object.
 * I'm not sure why this not working... still trying to figure it out.
 **/
GEMPedestals::GEMPedestals(const edm::ParameterSet& iConfig){
    setWhatProduced (this, &GEMPedestals::producePedestals);
}

GEMPedestals::~GEMPedestals(){}


std::unique_ptr<GEMPedestal> GEMPedestals::producePedestals (const GEMPedestalsRcd& rcd) {

    std::unique_ptr<GEMPedestal> obj = std::unique_ptr<GEMPedestal> (new GEMPedestal() );

    return obj;
};

