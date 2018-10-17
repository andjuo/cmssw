#ifndef CondTools_GEM_plugins_GEMQC8ConfESProducer_h
#define CondTools_GEM_plugins_GEMQC8ConfESProducer_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESProducts.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "CondTools/GEM/interface/GEMQC8ConfSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Conf.h"
#include "CondFormats/DataRecord/interface/GEMQC8ConfRcd.h"
#include "CondFormats/GEMObjects/interface/GEMELMap.h"
#include "CondFormats/DataRecord/interface/GEMELMapRcd.h"

class GEMQC8ConfESSource : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder {
 public:
  GEMQC8ConfESSource(const edm::ParameterSet&);
  ~GEMQC8ConfESSource() override {}

  edm::ESProducts<std::shared_ptr<GEMQC8Conf>,std::shared_ptr<GEMELMap> > produce(const GEMQC8ConfRcd&);

 protected:
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
		      const edm::IOVSyncValue&, edm::ValidityInterval &) override;

 private:
  popcon::GEMQC8ConfSourceHandler srcHandler;
};



#endif
