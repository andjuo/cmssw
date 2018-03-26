#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
#include "CondFormats/DataRecord/interface/GEMPVSSFWCAENChannelRcd.h"

#include <iomanip>

//using namespace std;
//using namespace edm;

// class declaration
class GEMPVSSFWCAENChannelDBReader : public edm::EDAnalyzer {
public:
  explicit GEMPVSSFWCAENChannelDBReader( const edm::ParameterSet& );
  ~GEMPVSSFWCAENChannelDBReader();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
private:
  bool m_flag;
};

GEMPVSSFWCAENChannelDBReader::GEMPVSSFWCAENChannelDBReader( const edm::ParameterSet& iConfig )
{

}

GEMPVSSFWCAENChannelDBReader::~GEMPVSSFWCAENChannelDBReader(){}

void GEMPVSSFWCAENChannelDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMPVSSFWCAENChannelDBReader" << std::endl;

  edm::ESHandle<GEMPVSSFWCAENChannel> hFwInfo;
  iSetup.get<GEMPVSSFWCAENChannelRcd>().get(hFwInfo);
  const GEMPVSSFWCAENChannel* fwInfo=hFwInfo.product();
  if(!fwInfo) { std::cout << "fwInfo is null" << std::endl; return; }
  std::cout << fwInfo << std::endl;

  std::cout << "version: " << fwInfo->version() << " and sizes: " << fwInfo->gem_ObImon_.size() << ", " << fwInfo->gem_ObVmon_.size() << "\n";

  std::cout << (*fwInfo) << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelDBReader);
