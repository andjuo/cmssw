import FWCore.ParameterSet.Config as cms

process = cms.Process("MYGAMMAJET")

process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag=autoCond['startup']

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.maxEvents = cms.untracked.PSet(
#    input = cms.untracked.int32(100)
    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
    fileNames = 
cms.untracked.vstring(
#   'file:/tmp/andriusj/6EC8FCC8-E2A8-E411-9506-002590596468.root'
#        '/store/relval/CMSSW_7_4_0_pre6/RelValPhotonJets_Pt_10_13/GEN-SIM-RECO/MCRUN2_74_V1-v1/00000/6EC8FCC8-E2A8-E411-9506-002590596468.root'
    'file:/tmp/andriusj/Run2012A_Photon_22Jan2013-002618943913.root'
 )
)

process.load("Calibration.HcalAlCaRecoProducers.alcagammajet_cfi")
process.load("Calibration.HcalAlCaRecoProducers.ALCARECOHcalCalGammaJet_Output_cff")

process.GammaJetProd.PhoInput= cms.InputTag("photons")
#process.GammaJetProd.PFjetInput= cms.InputTag("ak5PFJetsCHS")
process.GammaJetProd.gsfeleInput= cms.InputTag("gsfElectrons")
process.GammaJetProd.PhoLoose= cms.InputTag("PhotonIDProd","PhotonCutBasedIDLoose")
process.GammaJetProd.PhoTight= cms.InputTag("PhotonIDProd","PhotonCutBasedIDTight")

#process.GammaJetRecos = cms.OutputModule("PoolOutputModule",
#    outputCommands = cms.untracked.vstring('drop *',
##                 'keep recoPhotonCores_*_*_*',
#                 'keep recoSuperClusters_*_*_*',
#                 #'keep recoTracks_*_*_*',
#                 'keep recoTracks_generalTracks_*_*',
#                 #'keep *_PhotonIDProd_*_*',
#               'keep *_particleFlow_*_*',
#              'keep recoPFBlocks_particleFlowBlock_*_*',
#              'keep recoPFClusters_*_*_*',
##                         'keep *_particleFlowPtrs_*_*',
#        'keep *_GammaJetProd_*_*'),
#    fileName = cms.untracked.string('gjet.root')
#)

process.GammaJetRecos = cms.OutputModule("PoolOutputModule",
   outputCommands = process.OutALCARECOHcalCalGammaJet.outputCommands,
   fileName = cms.untracked.string('/tmp/andriusj/gjet_Run2012A.root')
)


process.ak5PFCHSJets = process.ak5PFJets.clone( src = 'pfNoPileUp' )
process.seq_ak5PFCHS= cms.Sequence( process.pfNoPileUpSequence * process.ak5PFCHSJets )

process.p = cms.Path( process.seq_ak5PFCHS * process.GammaJetProd )
process.e = cms.EndPath(process.GammaJetRecos)
