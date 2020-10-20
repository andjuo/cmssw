#ifndef RecoEgamma_EgammaTools_PhotonEnergyShifter
#define RecoEgamma_EgammaTools_PhotonEnergyShifter

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

#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"

#include "EgammaAnalysis/ElectronTools/interface/EGammaEnergyShifter.h"

class PhotonEnergyShifter : public EGammaEnergyShifter {
  
 public:
 PhotonEnergyShifter() {};
   
 math::XYZTLorentzVector getShiftedCalibratedMomentum(edm::RefToBase<reco::Photon>, EGMSmearer::UncertaintyType);
 math::XYZTLorentzVector getSimpleShiftedCalibratedMomentum(edm::RefToBase<reco::Photon>, EGMSmearer::SimplifiedUncertaintyType); 

 reco::Photon       getShiftedObject(edm::RefToBase<reco::Photon>, EGMSmearer::UncertaintyType);
 reco::Photon       getSimpleShiftedObject(edm::RefToBase<reco::Photon>, EGMSmearer::SimplifiedUncertaintyType);
  
};

#endif

