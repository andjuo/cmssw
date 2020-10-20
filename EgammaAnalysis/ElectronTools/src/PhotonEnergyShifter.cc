#include "EgammaAnalysis/ElectronTools/interface/PhotonEnergyShifter.h"

math::XYZTLorentzVector PhotonEnergyShifter::getShiftedCalibratedMomentum(edm::RefToBase<reco::Photon> calibratedObject, EGMSmearer::UncertaintyType uncertainty) {

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


math::XYZTLorentzVector PhotonEnergyShifter::getSimpleShiftedCalibratedMomentum(edm::RefToBase<reco::Photon> calibratedObject, EGMSmearer::SimplifiedUncertaintyType uncertainty) {

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


reco::Photon PhotonEnergyShifter::getSimpleShiftedObject(edm::RefToBase<reco::Photon> photon, EGMSmearer::SimplifiedUncertaintyType uncertainty) {  

  reco::Photon shiftedPhoton(*photon);
  float energyResolution = photon->getCorrectedEnergyError(reco::Photon::P4type::regression2);
  math::XYZTLorentzVector newFourMomentum = getSimpleShiftedCalibratedMomentum(photon, uncertainty);
  
  shiftedPhoton.setCorrectedEnergy(reco::Photon::P4type::regression2, newFourMomentum.energy(), energyResolution, true);
  return shiftedPhoton;

}

reco::Photon PhotonEnergyShifter::getShiftedObject(edm::RefToBase<reco::Photon> photon, EGMSmearer::UncertaintyType uncertainty) {  

  reco::Photon shiftedPhoton(*photon);
  float energyResolution = photon->getCorrectedEnergyError(reco::Photon::P4type::regression2);
  math::XYZTLorentzVector newFourMomentum = getShiftedCalibratedMomentum(photon, uncertainty);
  
  shiftedPhoton.setCorrectedEnergy(reco::Photon::P4type::regression2, newFourMomentum.energy(), energyResolution, true);
  return shiftedPhoton;

}
