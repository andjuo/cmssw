#include "EgammaAnalysis/ElectronTools/interface/EGammaEnergyShifter.h"

void EGammaEnergyShifter::setConsume(edm::ParameterSet const& iPS, edm::ConsumesCollector && iC) {
  
  scaleStatUpUncertaintyToken       = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleStatUpUncertainty"));
  scaleStatDownUncertaintyToken     = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleStatDownUncertainty"));
  scaleSystUpUncertaintyToken       = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleSystUpUncertainty"));
  scaleSystDownUncertaintyToken     = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleSystDownUncertainty"));
  scaleGainUpUncertaintyToken       = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleGainUpUncertainty"));
  scaleGainDownUncertaintyToken     = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleGainDownUncertainty"));
  resolutionRhoUpUncertaintyToken   = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionRhoUpUncertainty"));
  resolutionRhoDownUncertaintyToken = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionRhoDownUncertainty"));
  resolutionPhiUpUncertaintyToken   = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionPhiUpUncertainty"));
  resolutionPhiDownUncertaintyToken = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionPhiDownUncertainty"));

  scaleUpUncertaintyToken           = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleUpUncertainty"));
  scaleDownUncertaintyToken         = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("scaleDownUncertainty"));
  resolutionUpUncertaintyToken      = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionUpUncertainty"));
  resolutionDownUncertaintyToken    = iC.consumes<edm::ValueMap<float> >(iPS.getParameter<edm::InputTag>("resolutionDownUncertainty"));
  
}

void EGammaEnergyShifter::setEvent(const edm::Event& iEvt) {

  iEvt.getByToken(scaleStatUpUncertaintyToken, scaleStatUpUncertaintyHandle);
  iEvt.getByToken(scaleStatDownUncertaintyToken, scaleStatDownUncertaintyHandle);
  iEvt.getByToken(scaleSystUpUncertaintyToken, scaleSystUpUncertaintyHandle);
  iEvt.getByToken(scaleSystDownUncertaintyToken, scaleSystDownUncertaintyHandle);
  iEvt.getByToken(scaleGainUpUncertaintyToken, scaleGainUpUncertaintyHandle);
  iEvt.getByToken(scaleGainDownUncertaintyToken, scaleGainDownUncertaintyHandle);
  iEvt.getByToken(resolutionRhoUpUncertaintyToken, resolutionRhoUpUncertaintyHandle);
  iEvt.getByToken(resolutionRhoDownUncertaintyToken, resolutionRhoDownUncertaintyHandle);
  iEvt.getByToken(resolutionPhiUpUncertaintyToken, resolutionPhiUpUncertaintyHandle);
  iEvt.getByToken(resolutionPhiDownUncertaintyToken, resolutionPhiDownUncertaintyHandle);

  iEvt.getByToken(scaleUpUncertaintyToken, scaleUpUncertaintyHandle);
  iEvt.getByToken(scaleDownUncertaintyToken, scaleDownUncertaintyHandle);
  iEvt.getByToken(resolutionUpUncertaintyToken, resolutionUpUncertaintyHandle);
  iEvt.getByToken(resolutionDownUncertaintyToken, resolutionDownUncertaintyHandle);

  
}
