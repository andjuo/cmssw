#include "EgammaAnalysis/ElectronTools/interface/ElectronEnergyShifter.h"

math::XYZTLorentzVector ElectronEnergyShifter::getShiftedCalibratedMomentum(edm::RefToBase<reco::GsfElectron> calibratedObject, EGMSmearer::UncertaintyType uncertainty) {

  math::XYZTLorentzVector fourMomentum = calibratedObject->p4();

  float shiftedEnergy;
  switch ( uncertainty ) {
  case EGMSmearer::ScaleStatUp:
    shiftedEnergy = (*scaleStatUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleStatDown:
    shiftedEnergy = (*scaleStatDownUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleSystUp:
    shiftedEnergy = (*scaleSystUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleSystDown:
    shiftedEnergy = (*scaleSystDownUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleGainUp:
    shiftedEnergy = (*scaleGainUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleGainDown:
    shiftedEnergy = (*scaleGainDownUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionRhoUp:
    shiftedEnergy = (*resolutionRhoUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionRhoDown:
    shiftedEnergy = (*resolutionRhoDownUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionPhiUp:
    shiftedEnergy = (*resolutionPhiUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionPhiDown:
    shiftedEnergy = (*resolutionPhiDownUncertaintyHandle)[calibratedObject];
    break;
  default:
    edm::LogError("I do not know this source of uncertainty");
    shiftedEnergy = fourMomentum.energy();
    break;
  }

  fourMomentum *= shiftedEnergy/fourMomentum.energy();
  return fourMomentum;
}


math::XYZTLorentzVector ElectronEnergyShifter::getSimpleShiftedCalibratedMomentum(edm::RefToBase<reco::GsfElectron> calibratedObject, EGMSmearer::SimplifiedUncertaintyType uncertainty) {

  math::XYZTLorentzVector fourMomentum = calibratedObject->p4();

  float shiftedEnergy;
  switch ( uncertainty ) {
  case EGMSmearer::ScaleUp:
    shiftedEnergy = (*scaleUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ScaleDown:
    shiftedEnergy = (*scaleDownUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionUp:
    shiftedEnergy = (*resolutionUpUncertaintyHandle)[calibratedObject];
    break;
  case EGMSmearer::ResolutionDown:
    shiftedEnergy = (*resolutionDownUncertaintyHandle)[calibratedObject];
    break;
  default:
    edm::LogError("I do not know this source of uncertainty");
    shiftedEnergy = fourMomentum.energy();
    break;
  }

  fourMomentum *= shiftedEnergy/fourMomentum.energy();
  return fourMomentum;
}


reco::GsfElectron ElectronEnergyShifter::getSimpleShiftedObject(edm::RefToBase<reco::GsfElectron> electron, EGMSmearer::SimplifiedUncertaintyType uncertainty) {  

  reco::GsfElectron shiftedElectron(*electron);
  float energyResolution = electron->p4Error(reco::GsfElectron::P4_COMBINATION);
  float trackResolution = electron->trackMomentumError();
  math::XYZTLorentzVector newFourMomentum = getSimpleShiftedCalibratedMomentum(electron, uncertainty);

  shiftedElectron.correctMomentum(newFourMomentum, trackResolution, energyResolution);
  return shiftedElectron;
}

reco::GsfElectron ElectronEnergyShifter::getShiftedObject(edm::RefToBase<reco::GsfElectron> electron, EGMSmearer::UncertaintyType uncertainty) {  

  reco::GsfElectron shiftedElectron(*electron);
  float energyResolution = electron->p4Error(reco::GsfElectron::P4_COMBINATION);
  float trackResolution = electron->trackMomentumError();
  math::XYZTLorentzVector newFourMomentum = getShiftedCalibratedMomentum(electron, uncertainty);

  shiftedElectron.correctMomentum(newFourMomentum, trackResolution, energyResolution);
  return shiftedElectron;
}
