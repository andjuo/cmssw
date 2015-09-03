import FWCore.ParameterSet.Config as cms

#------------------------------------------------
#AlCaReco filtering for HCAL gammajet:
#------------------------------------------------
from Calibration.HcalAlCaRecoProducers.alcagammajet_cfi import *
from Calibration.HcalAlCaRecoProducers.gammajetHLT_cfi import *
from RecoEgamma.PhotonIdentification.PhotonIDValueMapProducer_cfi import *

seqALCARECOHcalCalGammaJet_noHLT = cms.Sequence(photonIDValueMapProducer *
                                                GammaJetSelector)
seqALCARECOHcalCalGammaJet = cms.Sequence(gammajetHLT *
                                          photonIDValueMapProducer *
                                          GammaJetSelector)
