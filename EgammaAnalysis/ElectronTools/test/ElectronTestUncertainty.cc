// system include files
#include <memory>
#include <vector>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEnergyShifter.h"

//
// class declaration
//

class ElectronTestUncertainty : public edm::stream::EDAnalyzer<> {
public:
  explicit ElectronTestUncertainty(const edm::ParameterSet&);
  ~ElectronTestUncertainty(); 
   
private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    
  edm::EDGetToken electronsMiniAODToken_;
  ElectronEnergyShifter electronEnergyUncertainty_;    
  
};

ElectronTestUncertainty::ElectronTestUncertainty(const edm::ParameterSet& iConfig) 
{

  const edm::ParameterSet& uncertaintyConfig = iConfig.getParameter<edm::ParameterSet>("egmUncertaintyConfig");
  electronEnergyUncertainty_.setConsume(uncertaintyConfig, consumesCollector());

  electronsMiniAODToken_ = mayConsume<edm::View<reco::GsfElectron> >
    (iConfig.getParameter<edm::InputTag>
     ("electronsMiniAOD"));
  
}


ElectronTestUncertainty::~ElectronTestUncertainty()
{}


//
// member functions
//

// ------------ method called for each event  ------------
void
ElectronTestUncertainty::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace std;
  using namespace edm;
  using namespace reco;

  electronEnergyUncertainty_.setEvent(iEvent);
  
  edm::Handle<edm::View<reco::GsfElectron> > electrons;
  iEvent.getByToken(electronsMiniAODToken_,electrons);

  // Loop over electrons
  for (size_t i = 0; i < electrons->size(); ++i){
    const auto elRef = electrons->refAt(i);
    std::cout << elRef->energy() << " " << electronEnergyUncertainty_.getSimpleShiftedObject(elRef, EGMSmearer::ResolutionUp) << std::endl;
  }
  
}

//define this as a plug-in
DEFINE_FWK_MODULE(ElectronTestUncertainty);
