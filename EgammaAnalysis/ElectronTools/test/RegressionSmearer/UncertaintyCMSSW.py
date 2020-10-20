import FWCore.ParameterSet.Config as cms

process = cms.Process('Test')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

inputFiles = cms.untracked.vstring('file:step4.root')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
process.source = cms.Source ('PoolSource', fileNames = inputFiles )                             

from EgammaAnalysis.ElectronTools.egammaEnergyShifter_cff import configElectronEnergyShifter

process.ntupler = cms.EDAnalyzer(
    'ElectronTestUncertainty',
    electronsMiniAOD    = cms.InputTag('calibratedPatElectrons'),
    egmUncertaintyConfig = configElectronEnergyShifter
    )

process.p = cms.Path(process.ntupler)
