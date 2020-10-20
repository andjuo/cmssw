#ifndef RecoEgamma_EgammaTools_EGammaEnergyShifter
#define RecoEgamma_EgammaTools_EGammaEnergyShifter

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

namespace EGMSmearer {

enum UncertaintyType{ ScaleStatUp, ScaleStatDown, ScaleSystUp, ScaleSystDown, ScaleGainUp, ScaleGainDown, 
		      ResolutionRhoUp, ResolutionRhoDown, ResolutionPhiUp, ResolutionPhiDown };
enum SimplifiedUncertaintyType{ ScaleUp, ScaleDown, ResolutionUp, ResolutionDown };

};

class EGammaEnergyShifter {
  
 public:
 EGammaEnergyShifter() {};
 virtual ~EGammaEnergyShifter() {};
  
 void setConsume(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC);    
 void setEvent(const edm::Event& iEvt);
 
 protected:

 // tokens to set consume
 edm::EDGetTokenT<edm::ValueMap<float> > scaleStatUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleStatDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleSystUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleSystDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleGainUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleGainDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionRhoUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionRhoDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionPhiUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionPhiDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > scaleDownUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionUpUncertaintyToken;
 edm::EDGetTokenT<edm::ValueMap<float> > resolutionDownUncertaintyToken;

 // handles to set the event
 edm::Handle<edm::ValueMap<float> > scaleStatUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleStatDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleSystUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleSystDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleGainUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleGainDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionRhoUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionRhoDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionPhiUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionPhiDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > scaleDownUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionUpUncertaintyHandle;
 edm::Handle<edm::ValueMap<float> > resolutionDownUncertaintyHandle;
  
};

#endif

