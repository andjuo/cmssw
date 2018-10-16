#include "CondTools/GEM/plugins/GEMQC8ConfESSource.h"


GEMQC8ConfESSource::GEMQC8ConfESSource(const edm::ParameterSet &p) :
  srcHandler(p)
{
  setWhatProduced(this,&GEMQC8ConfESSource::produce);
  findingRecord<GEMQC8ConfRcd>();

  edm::LogInfo("GEMQC8ConfESProducer") << "constructor" << std::endl;
}


std::unique_ptr<GEMQC8Conf> GEMQC8ConfESSource::produce(const GEMQC8ConfRcd&)
{
  std::cout << "GEMQC8ConfESSource: calling getNewObjects" << std::endl;
  srcHandler.getNewObjects();
  std::cout << "GEMQC8ConfESSource: calling getNewObjects done" << std::endl;
  if (!srcHandler.getQC8conf()) {
    std::cout << "GEMQC8ConfESProducer::produce failed to get a ptr" << std::endl;
  }

  std::unique_ptr<GEMQC8Conf> qc8conf(new GEMQC8Conf(srcHandler.getQC8conf()));
  //(*qc8conf).assign(*srcHandler.getQC8conf());

  if (!qc8conf) std::cout << "qc8conf is null" << std::endl;
  else { std::cout << "constructed qc8conf "; qc8conf->print(std::cout,1); }
  std::cout << "quitting the produce !!!" << std::endl;
  return qc8conf;
}


void GEMQC8ConfESSource::setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
					const edm::IOVSyncValue & iosv,
					edm::ValidityInterval & oValidity)
{
  edm::ValidityInterval infinity(iosv.beginOfTime(), iosv.endOfTime());
  oValidity = infinity;
}


#include "FWCore/Framework/interface/SourceFactory.h"
DEFINE_FWK_EVENTSETUP_SOURCE(GEMQC8ConfESSource);
