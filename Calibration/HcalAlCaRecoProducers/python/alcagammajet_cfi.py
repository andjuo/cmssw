import FWCore.ParameterSet.Config as cms

# selector for HcalAlCaGammaJet
GammaJetSelector = cms.EDFilter("AlCaGammaJetSelector",
   PhoInput = cms.InputTag("gedPhotons"),
   PFjetInput = cms.InputTag("ak4PFJetsCHS"),
   VertexCollection = cms.InputTag("offlinePrimaryVertices"),
   MinPtPhoton = cms.double(10.0),
   MinPtJet = cms.double(10.0),
   MinDPhiPhoJet = cms.double(1.8),

   ApplyPhoQualityCuts = cms.bool(True),
   RhoCollection = cms.InputTag("fixedGridRhoFastjetAll"),
   PhoSieieCollection = cms.InputTag("photonIDValueMapProducer","phoFull5x5SigmaIEtaIEta"),
   PhoChIsoCollection = cms.InputTag("photonIDValueMapProducer","phoChargedIsolation"),
   PhoNhIsoCollection = cms.InputTag("photonIDValueMapProducer","phoNeutralHadronIsolation"),
   PhoPhIsoCollection = cms.InputTag("photonIDValueMapProducer","phoPhotonIsolation"),
   # based on information in the twiki
   # https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2
   # apply looser pre-selection cuts on the leading photon
   # pT dependence on isolation is ignored
   PhoMaxHE_barrel = cms.double(0.6),
   PhoMaxSieie_barrel = cms.double(0.015),
   PhoMaxChIso_barrel = cms.double(3.),
   PhoMaxNhIso_barrel = cms.double(16.),
   PhoMaxPhIso_barrel = cms.double(10.),

   PhoMaxHE_endcap = cms.double(0.1),
   PhoMaxSieie_endcap = cms.double(0.035),
   PhoMaxChIso_endcap = cms.double(2.),
   PhoMaxNhIso_endcap = cms.double(20.),
   PhoMaxPhIso_endcap = cms.double(12.),

   # The approximate effective areas for photon isolation
   # Charged hadron isolation, neutral hadron isolation, photon isolation
   PhoEffAreas_barrel = cms.vdouble( 0.03, 0.03, 0.1 ),
   PhoEffAreas_endcap = cms.vdouble( 0.005, 0.2, 0.2 )

   )
