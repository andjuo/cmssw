import FWCore.ParameterSet.Config as cms

process = cms.Process("MYGAMMAJET")

process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag=autoCond['run2_data']

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery=cms.untracked.int32(1000)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
#    input = cms.untracked.int32(-1)
)
process.source = cms.Source("PoolSource",
    fileNames = 
cms.untracked.vstring(
#   'file:/tmp/andriusj/6EC8FCC8-E2A8-E411-9506-002590596468.root'
#        '/store/relval/CMSSW_7_4_0_pre6/RelValPhotonJets_Pt_10_13/GEN-SIM-RECO/MCRUN2_74_V1-v1/00000/6EC8FCC8-E2A8-E411-9506-002590596468.root'
        'file:/tmp/andriusj/FileA_468.root',
        'file:/tmp/andriusj/FileB_0D6.root'
 )
)

process.load("Calibration.HcalAlCaRecoProducers.ALCARECOHcalCalGammaJet_cff")
process.load("Calibration.HcalAlCaRecoProducers.ALCARECOHcalCalGammaJet_Output_cff")

#process.GammaJetSelector.ApplyPhoQualityCuts = cms.bool(False)
process.p = cms.Path(process.seqALCARECOHcalCalGammaJet_noHLT)

# Two output modules are defined:
# 1) GammaJetRecos_Debug is intended to determine the needed collections.
# 2) GammaJetRecos is intended to test the outputCommands of AlCaRecoStream

process.GammaJetRecos_Debug = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('p')
    ),
    outputCommands = cms.untracked.vstring
           ('drop *',
            'keep *_gedPhotons_*_*',
            'keep recoPhotonCores_gedPhotonCore_*_*',

            ##'keep recoSuperClusters_*_*_*',
            #'keep recoSuperClusters_correctedHybridSuperClusters_*_*',
            #'keep recoSuperClusters_correctedMulti5x5SuperClustersWithPreshower_*_*',
            #'keep recoSuperClusters_correctedhfEMClusters_*_*',
            #'keep recoSuperClusters_hybridSuperClusters_*_*',
            #'keep recoSuperClusters_multi5x5SuperClustersWithPreshower_*_*',
            #'keep recoSuperClusters_hfEMClusters_*_*',
            'keep recoSuperClusters_particleFlowEGamma_*_*',
            #'keep recoSuperClusters_particleFlowSuperClusterECAL_*_*',
            #'keep recoSuperClusters_hybridSuperClusters_*_*',
            #'keep recoSuperClusters_multi5x5SuperClusters_*_*',

            #'keep recoCaloClusters_*_*_*',
            #'keep recoCaloClusters_correctedHybridSuperClusters_*_*',
            #'keep recoCaloClusters_correctedMulti5x5SuperClustersWithPreshower_*_*',
            #'keep recoCaloClusters_correctedhfEMClusters_*_*',
            #'keep recoCaloClusters_hybridSuperClusters_*_*',
            #'keep recoCaloClusters_multi5x5SuperClustersWithPreshower_*_*',
            #'keep recoCaloClusters_hfEMClusters_*_*',
            'keep recoCaloClusters_particleFlowEGamma_*_*',
            #'keep recoCaloClusters_particleFlowSuperClusterECAL_*_*',
            #'keep recoCaloClusters_hybridSuperClusters_*_*',
            #'keep recoCaloClusters_multi5x5SuperClusters_*_*',

            'keep *_particleBasedIsolation_gedPhotons_*',
            'keep *_reducedEcalRecHitsEB_*_*',
            'keep *_reducedEcalRecHitsEE_*_*',
            'keep *_reducedEcalRecHitsES_*_*',
            'keep *_PhotonIDProdGED_PhotonCutBasedIDLoose_*',
            'keep *_PhotonIDProdGED_PhotonCutBasedIDTight_*',
            'keep *_gedGsfElectrons_*_*',
            'keep *_allConversions_*_*',

            'keep recoPFJets_ak4PFJetsCHS_*_*',

            'keep *_hbhereco_*_*',
            'keep *_hfreco_*_*',
            'keep *_horeco_*_*',

            ##'keep recoTracks_*_*_*',
            #'keep recoTracks_generalTracks_*_*',
            'keep recoTracks_generalTracks__*',

            'keep *_particleFlow_*_*',
            'keep *_particleFlowPtrs_*_*',
            'keep recoPFBlocks_particleFlowBlock_*_*',

            ##'keep recoPFClusters_*_*_*',
            #'keep recoPFClusters_particleFlowClusterECAL_*_*',
            'keep recoPFClusters_particleFlowClusterHCAL_*_*',
            'keep recoPFClusters_particleFlowClusterHO_*_*',
            #'keep recoPFClusters_particleFlowClusterPS_*_*',

            'keep *_fixedGridRhoFastjetAll_*_*',
            'keep *_pfMet_*_*',
            'keep *_offlinePrimaryVertices_*_*',
            'keep *_offlineBeamSpot_*_*',

            #'keep *_TriggerResults_*_*',
            'keep *_TriggerResults_*_HLT',

            'keep *_genParticles_*_*',
            'keep *_ak4GenJets_*_*',
            'keep GenEventInfoProduct_generator_*_*'
            ),
    fileName = cms.untracked.string('gjet_debug.root')
)

process.GammaJetRecos = cms.OutputModule("PoolOutputModule",
   SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('p')
   ),
   outputCommands = process.OutALCARECOHcalCalGammaJet.outputCommands,
   fileName = cms.untracked.string('gjet.root')
)

#process.e = cms.EndPath(process.GammaJetRecos_Debug)
process.e = cms.EndPath(process.GammaJetRecos)
