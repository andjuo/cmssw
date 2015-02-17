import FWCore.ParameterSet.Config as cms

# output block for alcastream HCAL Dijets
# output module 
#  module alcastreamHcalGammaJetOutput = PoolOutputModule
alcastreamHcalGammaJetOutput = cms.PSet(
    outputCommands = cms.untracked.vstring('drop *',
                 'keep recoPhotonCores_*_*_*',
                 'keep recoSuperClusters_*_*_*',
                 'keep recoTracks_generalTracks_*_*',
                 'keep *_particleFlow_*_*',
                 'keep recoPFBlocks_particleFlowBlock_*_*',
                 'keep recoPFClusters_*_*_*',
                 'keep *_GammaJetProd_*_*')
)
