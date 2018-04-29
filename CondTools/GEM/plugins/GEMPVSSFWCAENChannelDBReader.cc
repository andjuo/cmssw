#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
#include "CondFormats/DataRecord/interface/GEMPVSSFWCAENChannelRcd.h"

#include <iomanip>
#include <fstream>

//using namespace std;
//using namespace edm;

// class declaration
class GEMPVSSFWCAENChannelDBReader : public edm::EDAnalyzer {
public:
  explicit GEMPVSSFWCAENChannelDBReader( const edm::ParameterSet& );
  ~GEMPVSSFWCAENChannelDBReader();
  void beginJob();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  void endJob();

private:
  std::string dumpFileName_;
  std::ofstream dumpFout_;
  //bool m_flag;
};

GEMPVSSFWCAENChannelDBReader::GEMPVSSFWCAENChannelDBReader( const edm::ParameterSet& iConfig ) :
  dumpFileName_(iConfig.getUntrackedParameter<std::string>("dumpFileName","")),
  dumpFout_()
{
}

GEMPVSSFWCAENChannelDBReader::~GEMPVSSFWCAENChannelDBReader(){}


void GEMPVSSFWCAENChannelDBReader::beginJob()
{
  if (dumpFileName_.size()) {
    dumpFout_.open(dumpFileName_.c_str());
  }
}


void GEMPVSSFWCAENChannelDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMPVSSFWCAENChannelDBReader" << std::endl;

  edm::ESHandle<GEMPVSSFWCAENChannel> hFwInfo;
  iSetup.get<GEMPVSSFWCAENChannelRcd>().get(hFwInfo);
  const GEMPVSSFWCAENChannel* fwInfo=hFwInfo.product();
  if(!fwInfo) { std::cout << "fwInfo is null" << std::endl; return; }
  std::cout << fwInfo << std::endl;

  std::cout << "version: " << fwInfo->version() << " and sizes: " << fwInfo->gem_ObImon_.size() << ", " << fwInfo->gem_ObVmon_.size() << "\n";

  std::cout << (*fwInfo) << std::endl;

  if (dumpFout_.is_open()) dumpFout_ << (*fwInfo) << std::endl;

}

void GEMPVSSFWCAENChannelDBReader::endJob()
{
  if (dumpFout_.is_open()) {
    dumpFout_ << "GEMPVSSFWCAENChannelDBReader: dump done\n";
    dumpFout_.close();
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelDBReader);
