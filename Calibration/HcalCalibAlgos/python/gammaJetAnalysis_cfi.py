import FWCore.ParameterSet.Config as cms
from RecoJets.Configuration.RecoJets_cff import *
from RecoJets.Configuration.RecoPFJets_cff import *
from CommonTools.ParticleFlow.pfNoPileUp_cff import *

GammaJetAnalysis = cms.EDAnalyzer('GammaJetAnalysis',
                                  rhoColl             = cms.InputTag("fixedGridRhoFastjetAll"),
                                  PFMETColl           = cms.InputTag("pfMet"),
                                  PFMETTYPE1Coll      = cms.InputTag("pfType1CorrectedMet"),
                                  photonCollName      = cms.string('gedPhotons'),
                                  pfJetCollName       = cms.string('ak4PFJetsCHS'),
                                  pfJetCorrName       = cms.string('ak4PFCHSL2L3'),
                                  genJetCollName      = cms.string('ak4GenJets'),
                                  genParticleCollName = cms.string('genParticles'),
                                  genEventInfoName    = cms.string('generator'),
                                  hbheRecHitName      = cms.string('hbhereco'),
                                  hfRecHitName        = cms.string('hfreco'),
                                  hoRecHitName        = cms.string('horeco'),
                                  rootHistFilename    = cms.string('PhotonPlusJet_tree.root'),
                                  pvCollName = cms.string('offlinePrimaryVertices'),
                                  beamSpotName= cms.string('offlineBeamSpot'),
                                  conversionsName= cms.string('allConversions'),
                                  electronCollName= cms.string('gedGsfElectrons'),
                                  photonIdTightName= cms.InputTag('PhotonIDProdGED','PhotonCutBasedIDTight'),
                                  photonIdLooseName= cms.InputTag('PhotonIDProdGED','PhotonCutBasedIDLoose'),
                                  photonRun2IdLooseMap = cms.InputTag("egmPhotonIDs","cutBasedPhotonID-PHYS14-PU20bx25-V2-standalone-loose"),
                                  photonRun2IdMediumMap = cms.InputTag("egmPhotonIDs","cutBasedPhotonID-PHYS14-PU20bx25-V2-standalone-medium"),
                                  photonRun2IdTightMap = cms.InputTag("egmPhotonIDs","cutBasedPhotonID-PHYS14-PU20bx25-V2-standalone-tight"),
                                  egmPhoChIsoMap = cms.InputTag("photonIDValueMapProducer","phoChargedIsolation"),
                                  egmPhoNhIsoMap = cms.InputTag("photonIDValueMapProducer","phoNeutralHadronIsolation"),
                                  egmPhoPhIsoMap = cms.InputTag("photonIDValueMapProducer","phoPhotonIsolation"),
                                  egmEffAreasChConfigFile = cms.FileInPath('RecoEgamma/PhotonIdentification/data/PHYS14/effAreaPhotons_cone03_pfChargedHadrons_V2.txt'),
                                  egmEffAreasNhConfigFile = cms.FileInPath('RecoEgamma/PhotonIdentification/data/PHYS14/effAreaPhotons_cone03_pfNeutralHadrons_V2.txt'),
                                  egmEffAreasPhConfigFile = cms.FileInPath('RecoEgamma/PhotonIdentification/data/PHYS14/effAreaPhotons_cone03_pfPhotons_V2.txt'),
                                  allowNoPhoton       = cms.bool(False),
                                  photonJetDPhiMin    = cms.double(2.0),  # 0.75 pi= 2.356, 0.7 pi=2.2
                                  photonPtMin         = cms.double(15.),
                                  jetEtMin            = cms.double(15.),
                                  jet2EtMax            = cms.double(100.),
                                  jet3EtMax            = cms.double(50.),
                                  photonTriggers      = cms.vstring(''), #HLT_Photon20_*, HLT_Photon135*'),
                                  jetTriggers         = cms.vstring(''), #HLT_Jet30*'),
                                  writeTriggerPrescale= cms.bool(False),
                                  doPFJets            = cms.bool(True),
                                  doGenJets           = cms.bool(True),
                                  debug               = cms.untracked.int32(0),
    # debugHLTTrigNames:
    # 0 - no info,
    # 1 - print HLT triggers containing '_Photon',
    # 2 - print all HLT triggers and mark '_Photon' triggers
                                  debugHLTTrigNames   = cms.untracked.int32(0),
                                  workOnAOD           = cms.int32(0)
                                  )
