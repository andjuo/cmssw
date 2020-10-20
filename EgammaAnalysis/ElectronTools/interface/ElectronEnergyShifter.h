#ifndef RecoEgamma_EgammaTools_ElectronEnergyShifter
#define RecoEgamma_EgammaTools_ElectronEnergyShifter

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

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

#include "EgammaAnalysis/ElectronTools/interface/EGammaEnergyShifter.h"

class ElectronEnergyShifter : public EGammaEnergyShifter {
  
 public:
 ElectronEnergyShifter() {};
   
 math::XYZTLorentzVector getShiftedCalibratedMomentum(edm::RefToBase<reco::GsfElectron>, EGMSmearer::UncertaintyType);
 math::XYZTLorentzVector getSimpleShiftedCalibratedMomentum(edm::RefToBase<reco::GsfElectron>, EGMSmearer::SimplifiedUncertaintyType); 

 reco::GsfElectron       getShiftedObject(edm::RefToBase<reco::GsfElectron>, EGMSmearer::UncertaintyType);
 reco::GsfElectron       getSimpleShiftedObject(edm::RefToBase<reco::GsfElectron>, EGMSmearer::SimplifiedUncertaintyType);
  
};

#endif

