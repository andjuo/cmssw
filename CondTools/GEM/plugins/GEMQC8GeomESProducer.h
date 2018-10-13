#ifndef CondTools_GEM_plugins_GEMQC8GeomESProducer_h
#define CondTools_GEM_plugins_GEMQC8GeomESProducer_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "CondTools/GEM/interface/GEMQC8GeomSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "CondFormats/DataRecord/interface/GEMQC8GeomRcd.h"

class GEMQC8GeomESProducer : public edm::ESProducer {
 public:
  GEMQC8GeomESProducer(const edm::ParameterSet&);
  ~GEMQC8GeomESProducer() override {}

  std::unique_ptr<GEMQC8Geom> produce(const GEMQC8GeomRcd&);

 private:
  popcon::GEMQC8GeomSourceHandler srcHandler;
};



#endif
