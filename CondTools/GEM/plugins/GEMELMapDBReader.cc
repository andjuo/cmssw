#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMELMap.h"
#include "CondFormats/DataRecord/interface/GEMELMapRcd.h"

#include <iomanip>
#include <fstream>

//using namespace std;
//using namespace edm;

// class declaration
class GEMELMapDBReader : public edm::EDAnalyzer {
public:
  explicit GEMELMapDBReader( const edm::ParameterSet& );
  ~GEMELMapDBReader();
  void beginJob();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  void endJob();

private:
  std::string dumpFileName_;
  std::ofstream dumpFout_;
  //bool m_flag;
};

GEMELMapDBReader::GEMELMapDBReader( const edm::ParameterSet& iConfig ) :
  dumpFileName_(iConfig.getUntrackedParameter<std::string>("dumpFileName","")),
  dumpFout_()
{
}

GEMELMapDBReader::~GEMELMapDBReader(){}


void GEMELMapDBReader::beginJob()
{
  if (dumpFileName_.size()) {
    dumpFout_.open(dumpFileName_.c_str());
  }
}


void GEMELMapDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMELMapDBReader" << std::endl;

  edm::ESHandle<GEMELMap> hInfo;
  iSetup.get<GEMELMapRcd>().get(hInfo);
  if (!hInfo.isValid()) std::cout << "invalid handle hInfo" << std::endl;
  const GEMELMap* info=hInfo.product();
  if(!info) { std::cout << "info is null" << std::endl; return; }

  info->print(std::cout);
  std::cout << std::endl;

  if (dumpFout_.is_open()) info->print(dumpFout_,1);
}

void GEMELMapDBReader::endJob()
{
  if (dumpFout_.is_open()) {
    dumpFout_ << "GEMELMapDBReader: dump done\n";
    dumpFout_.close();
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(GEMELMapDBReader);
