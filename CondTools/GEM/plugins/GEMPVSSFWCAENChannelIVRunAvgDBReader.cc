#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannelIVRunAvg.h"
#include "CondFormats/DataRecord/interface/GEMPVSSFWCAENChannelIVRunAvgRcd.h"

#include <iomanip>
#include <fstream>

//using namespace std;
//using namespace edm;

// class declaration
class GEMPVSSFWCAENChannelIVRunAvgDBReader : public edm::EDAnalyzer {
public:
  explicit GEMPVSSFWCAENChannelIVRunAvgDBReader( const edm::ParameterSet& );
  ~GEMPVSSFWCAENChannelIVRunAvgDBReader();
  void beginJob();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  void endJob();
private:
  std::string dumpFileName_;
  std::ofstream dumpFout_;
  //bool m_flag;
};

GEMPVSSFWCAENChannelIVRunAvgDBReader::GEMPVSSFWCAENChannelIVRunAvgDBReader( const edm::ParameterSet& iConfig ) :
  dumpFileName_(iConfig.getUntrackedParameter<std::string>("dumpFileName","")),
  dumpFout_()
{

}

GEMPVSSFWCAENChannelIVRunAvgDBReader::~GEMPVSSFWCAENChannelIVRunAvgDBReader(){}


void GEMPVSSFWCAENChannelIVRunAvgDBReader::beginJob()
{
  if (dumpFileName_.size()) dumpFout_.open(dumpFileName_.c_str());
}

void GEMPVSSFWCAENChannelIVRunAvgDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMPVSSFWCAENChannelIVRunAvgDBReader" << std::endl;

  edm::ESHandle<GEMPVSSFWCAENChannelIVRunAvg> hFwInfo;
  iSetup.get<GEMPVSSFWCAENChannelIVRunAvgRcd>().get(hFwInfo);
  const GEMPVSSFWCAENChannelIVRunAvg* fwInfo=hFwInfo.product();
  if(!fwInfo) { std::cout << "fwInfo is null" << std::endl; return; }
  std::cout << fwInfo << std::endl;

  __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg=0;
  std::cout << "\nrunNumber=" << fwInfo->runTime_.run << ", version <" << fwInfo->version() << "> and sizes: avgImon[" << fwInfo->avgImon_.size() << "], avgVmon[" << fwInfo->avgVmon_.size() << "]\n\n";

  std::cout << (*fwInfo) << std::endl;

  if (dumpFout_.is_open()) dumpFout_ << (*fwInfo) << std::endl;
}


void GEMPVSSFWCAENChannelIVRunAvgDBReader::endJob()
{
  if (dumpFout_.is_open()) {
    dumpFout_ << "GEMPVSSFWCAENChannelIVRunAvgDBReader: dump done\n";
    dumpFout_.close();
  }
}


//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelIVRunAvgDBReader);
