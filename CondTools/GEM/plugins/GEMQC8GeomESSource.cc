#include "CondTools/GEM/plugins/GEMQC8GeomESSource.h"


GEMQC8GeomESSource::GEMQC8GeomESSource(const edm::ParameterSet &p) :
  srcHandler(p)
{
  setWhatProduced(this,&GEMQC8GeomESSource::produce);
  findingRecord<GEMQC8GeomRcd>();

  edm::LogInfo("GEMQC8GeomESProducer") << "constructor" << std::endl;
}


std::unique_ptr<GEMQC8Geom> GEMQC8GeomESSource::produce(const GEMQC8GeomRcd&)
{
  std::cout << "GEMQC8GeomESSource: calling getNewObjects" << std::endl;
  srcHandler.getNewObjects();
  std::cout << "GEMQC8GeomESSource: calling getNewObjects done" << std::endl;
  if (!srcHandler.getQC8geom()) {
    std::cout << "GEMQC8GeomESProducer::produce failed to get a ptr" << std::endl;
  }

  std::unique_ptr<GEMQC8Geom> qc8geom(new GEMQC8Geom(srcHandler.getQC8geom()));
  //(*qc8geom).assign(*srcHandler.getQC8geom());

  if (!qc8geom) std::cout << "qc8geom is null" << std::endl;
  else { std::cout << "constructed qc8geom "; qc8geom->print(std::cout); }
  std::cout << "quitting the produce !!!" << std::endl;
  return qc8geom;
}


void GEMQC8GeomESSource::setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
					const edm::IOVSyncValue & iosv,
					edm::ValidityInterval & oValidity)
{
  edm::ValidityInterval infinity(iosv.beginOfTime(), iosv.endOfTime());
  oValidity = infinity;
}


#include "FWCore/Framework/interface/SourceFactory.h"
DEFINE_FWK_EVENTSETUP_SOURCE(GEMQC8GeomESSource);
