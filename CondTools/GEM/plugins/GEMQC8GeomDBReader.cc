#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "CondFormats/DataRecord/interface/GEMQC8GeomRcd.h"

#include <iomanip>
#include <fstream>

//using namespace std;
//using namespace edm;

// class declaration
class GEMQC8GeomDBReader : public edm::EDAnalyzer {
public:
  explicit GEMQC8GeomDBReader( const edm::ParameterSet& );
  ~GEMQC8GeomDBReader();
  void beginJob();
  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  void endJob();

private:
  std::string dumpFileName_;
  std::ofstream dumpFout_;
  //bool m_flag;
};

GEMQC8GeomDBReader::GEMQC8GeomDBReader( const edm::ParameterSet& iConfig ) :
  dumpFileName_(iConfig.getUntrackedParameter<std::string>("dumpFileName","")),
  dumpFout_()
{
}

GEMQC8GeomDBReader::~GEMQC8GeomDBReader(){}


void GEMQC8GeomDBReader::beginJob()
{
  if (dumpFileName_.size()) {
    dumpFout_.open(dumpFileName_.c_str());
  }
}


void GEMQC8GeomDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMQC8GeomDBReader" << std::endl;

  edm::ESHandle<GEMQC8Geom> hInfo;
  iSetup.get<GEMQC8GeomRcd>().get(hInfo);
  if (!hInfo.isValid()) std::cout << "invalid handle hInfo" << std::endl;
  const GEMQC8Geom* info=hInfo.product();
  if(!info) { std::cout << "info is null" << std::endl; return; }

  info->print(std::cout);
  std::cout << std::endl;

  if (dumpFout_.is_open()) info->print(dumpFout_);
}

void GEMQC8GeomDBReader::endJob()
{
  if (dumpFout_.is_open()) {
    dumpFout_ << "GEMQC8GeomDBReader: dump done\n";
    dumpFout_.close();
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(GEMQC8GeomDBReader);
