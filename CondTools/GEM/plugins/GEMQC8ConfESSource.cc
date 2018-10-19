#include "CondTools/GEM/plugins/GEMQC8ConfESSource.h"


GEMQC8ConfESSource::GEMQC8ConfESSource(const edm::ParameterSet &p) :
  srcHandler(p)
{
  setWhatProduced(this,&GEMQC8ConfESSource::produce_QC8Conf);
  findingRecord<GEMQC8ConfRcd>();
  setWhatProduced(this,&GEMQC8ConfESSource::produce_ELMap);
  findingRecord<GEMELMapRcd>();

  edm::LogInfo("GEMQC8ConfESProducer") << "constructor" << std::endl;
}


std::unique_ptr<GEMQC8Conf> GEMQC8ConfESSource::produce_QC8Conf(const GEMQC8ConfRcd&)
{
  std::cout << "GEMQC8ConfESSource::produce_QC8Conf calling getNewObjects" << std::endl;
  srcHandler.setOnlyConfDef(1);
  srcHandler.getNewObjects();
  std::cout << "GEMQC8ConfESSource::produce_QC8Conf calling getNewObjects done" << std::endl;
  if (!srcHandler.getQC8conf()) {
    std::cout << "GEMQC8ConfESProducer::produce_QC8Conf failed to get a ptr" << std::endl;
  }

  std::unique_ptr<GEMQC8Conf> qc8conf(new GEMQC8Conf(srcHandler.getQC8conf()));
  //(*qc8conf).assign(*srcHandler.getQC8conf());
  if (!qc8conf) std::cout << "qc8conf is null" << std::endl;
  else { std::cout << "constructed qc8conf "; qc8conf->print(std::cout); }

  std::cout << "quitting the produce_QC8Conf !!!" << std::endl;
  return qc8conf;
}


std::unique_ptr<GEMELMap> GEMQC8ConfESSource::produce_ELMap(const GEMELMapRcd&)
{
  std::cout << "GEMQC8ConfESSource::produce_ELMap calling getNewObjects" << std::endl;
  srcHandler.setOnlyConfDef(0);
  srcHandler.getNewObjects();
  std::cout << "GEMQC8ConfESSource::produce_ELMap calling getNewObjects done" << std::endl;
  if (!srcHandler.getQC8elMap()) {
    std::cout << "GEMQC8ConfESProducer::produce_ELMap failed to get a ptr" << std::endl;
  }

  std::unique_ptr<GEMELMap> qc8elMap(new GEMELMap(srcHandler.getQC8elMap()));
  if (!qc8elMap) std::cout << "qc8elMap is null" << std::endl;
  else { std::cout << "constructed qc8elmap "; qc8elMap->print(std::cout); }

  std::cout << "quitting the produce_ELMap !!!" << std::endl;
  return qc8elMap;
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
