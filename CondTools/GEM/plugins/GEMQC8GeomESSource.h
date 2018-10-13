#ifndef CondTools_GEM_plugins_GEMQC8GeomESProducer_h
#define CondTools_GEM_plugins_GEMQC8GeomESProducer_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "CondTools/GEM/interface/GEMQC8GeomSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "CondFormats/DataRecord/interface/GEMQC8GeomRcd.h"

class GEMQC8GeomESSource : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder {
 public:
  GEMQC8GeomESSource(const edm::ParameterSet&);
  ~GEMQC8GeomESSource() override {}

  std::unique_ptr<GEMQC8Geom> produce(const GEMQC8GeomRcd&);

 protected:
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
		      const edm::IOVSyncValue&, edm::ValidityInterval &) override;

 private:
  popcon::GEMQC8GeomSourceHandler srcHandler;
};



#endif
