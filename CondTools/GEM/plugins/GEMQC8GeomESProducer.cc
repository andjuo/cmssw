#include "CondTools/GEM/plugins/GEMQC8GeomESProducer.h"


GEMQC8GeomESProducer::GEMQC8GeomESProducer(const edm::ParameterSet &p) : srcHandler(p)
{
  setWhatProduced(this);

  edm::LogInfo("GEMQC8GeomESProducer") << "constructor" << std::endl;
}


std::unique_ptr<GEMQC8Geom> GEMQC8GeomESProducer::produce(const GEMQC8GeomRcd&)
{
  srcHandler.getNewObjects();
  if (!srcHandler.getQC8geom()) {
    std::cout << "GEMQC8GeomESProducer::produce failed to get a ptr" << std::endl;
  }

  std::unique_ptr<GEMQC8Geom> qc8geom(new GEMQC8Geom(srcHandler.getQC8geom()));
  (*qc8geom).assign(*srcHandler.getQC8geom());

  if (!qc8geom) std::cout << "qc8geom is null" << std::endl;
  else { std::cout << "constructed qc8geom "; qc8geom->print(std::cout); }
  return qc8geom;
}

//#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
DEFINE_FWK_EVENTSETUP_MODULE(GEMQC8GeomESProducer);
