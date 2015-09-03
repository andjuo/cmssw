import FWCore.ParameterSet.Config as cms
process = cms.Process('ANALYSIS')

# The purpose of this macro is to check gammaJetAnalysis_cfg.py output
# reading the input files directly, not through GammaJetSelector.
# It is for convenience of use.

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.categories+=cms.untracked.vstring('GammaJetAnalysis')
#process.MessageLogger.cerr.FwkReport.reportEvery=cms.untracked.int32(1000)

# Specify IdealMagneticField ESSource (needed for CMSSW 730)
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load("MagneticField.Engine.autoMagneticFieldProducer_cfi")
from Configuration.AlCa.autoCond import autoCond
#process.GlobalTag.globaltag=autoCond['startup']
process.GlobalTag.globaltag = 'PHYS14_25_V1::All'

# get the official photonID producer
#process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/PhotonIdentification/egmPhotonIDs_cff")

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate
#if useAOD == True :
dataFormat = DataFormat.AOD
#else :
#dataFormat = DataFormat.MiniAOD

switchOnVIDPhotonIdProducer(process, dataFormat)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_PHYS14_PU20bx25_V2_cff']

#add them to the VID producer
for idmod in my_id_modules:
          setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)


#load the response corrections calculator
process.load('Calibration.HcalCalibAlgos.gammaJetAnalysis_cfi')
#  needed for nonCHS
#process.load('JetMETCorrections.Configuration.JetCorrectors_cff')
process.load('JetMETCorrections.Configuration.JetCorrectionProducers_cff')

# run over files
process.GammaJetAnalysis.rootHistFilename = cms.string('PhoJet_tree_CHS_252126_noGJetSel.root')
process.GammaJetAnalysis.doPFJets = cms.bool(True)
process.GammaJetAnalysis.doGenJets = cms.bool(False)

# trigger names should not end with '_'
process.GammaJetAnalysis.photonTriggers = cms.vstring(
    'HLT_Photon20_CaloIdVL_IsoL','HLT_Photon30_CaloIdVL_IsoL',
    'HLT_Photon50_CaloIdVL_IsoL','HLT_Photon75_CaloIdVL_IsoL',
    'HLT_Photon90_CaloIdVL_IsoL','HLT_Photon135',
    'HLT_Photon150','HLT_Photon160')
# triggers for CMSSW 730
process.GammaJetAnalysis.photonTriggers += cms.vstring(
    'HLT_Photon22', 'HLT_Photon30', 'HLT_Photon36',
    'HLT_Photon50', 'HLT_Photon75',
    'HLT_Photon90', 'HLT_Photon120', 'HLT_Photon175',
    'HLT_Photon250_NoHE', 'HLT_Photon300_NoHE'
)
# to disable photonTriggers assign an empty vstring
#process.GammaJetAnalysis.photonTriggers = cms.vstring()

# a clone without CHS
process.GammaJetAnalysis_noCHS= process.GammaJetAnalysis.clone()
process.GammaJetAnalysis_noCHS.rootHistFilename = cms.string('PhoJet_tree_nonCHS_252126_noGJetSel.root')
# for 7XY use ak4* instead of ak5
process.GammaJetAnalysis_noCHS.pfJetCollName = cms.string('ak4PFJets')
process.GammaJetAnalysis_noCHS.pfJetCorrName = cms.string('ak4PFL2L3')

import FWCore.Utilities.FileUtils as FileUtils
listFileName='../../HcalAlCaRecoProducers/test/run252126.txt'
mylist = FileUtils.loadListFromFile(listFileName)

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring( *mylist )
)

#To have the same number of histograms, do not run over GenJets
#process.GammaJetAnalysis.doGenJets = cms.bool(False)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.MessageLogger.cerr.FwkReport.reportEvery=cms.untracked.int32(1000)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# specify 'workOnAOD=1' if the input file is AOD, otherwise put 0
process.GammaJetAnalysis.workOnAOD = cms.int32(0)
process.GammaJetAnalysis.doGenJets = cms.bool(False)
process.GammaJetAnalysis.debug     = cms.untracked.int32(0)

process.egmPhotonIDSequence.remove('photonMVAValueMapProducer')

process.p = cms.Path(
    process.photonIDValueMapProducer *
    process.egmPhotonIDs *
    process.GammaJetAnalysis
)

# To check what collections are present, define the output module
process.Output = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('gjetOut.root')
)

#process.e = cms.EndPath(process.Output)
