# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: SingleMuPt100_cfi -s GEN,SIM,DIGI,L1,DIGI2RAW,RAW2DIGI,L1Reco,RECO --conditions auto:run2_mc --magField 38T_PostLS1 --datatier GEN-SIM --geometry GEMCosmicStand --eventcontent FEVTDEBUGHLT --era phase2_muon -n 100 --fileout out_reco.root
import datetime
print datetime.datetime.now()
import FWCore.ParameterSet.Config as cms

# options
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing('analysis')

options.register("runNum",1,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run number")
                 
options.register("eventsPerJob",2,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "The number of events (in each file)")
                 
options.register("idxJob","-1",
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "The index of this root file")

options.parseArguments()

# Insert the type 0 , S , L of the superchambers in 15 positions: frontal view, 90 deg rotated
SuperChType = ['L','L','L','L','L',\
               'L','L','L','L','L',\
               'L','L','L','L','L']

# Alignment of chambers
trueDx = [0.4,-0.1,-0.2,-0.5,0.2,\
          -0.4,0.1,0.3,-0.3,0.2,\
          0.1,-0.1,0.1,0.3,-0.4] # cm

trueRz = [-0.8,-0.2,-0.3,0.3,-0.3,\
          -0.1,-0.5,0.1,0.2,-0.2,\
          0.5,0.7,-0.1,-0.3,0.6] # degrees

# First step
shiftX = [0,0,0,0,0,\
          0,0,0,0,0,\
          0,0,0,0,0]

rotationZ = [0,0,0,0,0,\
             0,0,0,0,0,\
             0,0,0,0,0]

from Configuration.StandardSequences.Eras import eras

# eras.run2_GEM_2017 makes GEM packer/unpacker to use DB (GEMELMapRcd)
process = cms.Process('RECO',eras.phase2_muon) #,eras.run2_GEM_2017)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Geometry.GEMGeometry.GeometryGEMCosmicStand_cff')
process.load('Configuration.StandardSequences.MagneticField_0T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic50ns13TeVCollision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimIdeal_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('EventFilter.GEMRawToDigi.gemPacker_cfi')
process.load('EventFilter.RawDataCollector.rawDataCollector_cfi')
process.load('EventFilter.GEMRawToDigi.muonGEMDigis_cfi')
process.load('SimMuon.GEMCosmicMuon.muonGEMDigi_cff')
process.load('RecoLocalMuon.GEMRecHit.gemLocalReco_cff')

# For debug purposes - use what is already in GEM DB
#if hasattr(process, 'GEMQC8ConfESSource'):
#    process.GEMQC8ConfESSource.WriteDummy = cms.untracked.int32(-1)

process.GEMQC8ConfESSource.runNumber = cms.int32( options.runNum )

#process.gemPacker.useDBEMap = cms.bool(True)
process.gemPacker.name = cms.string('gemPacker--cfgFile')
#process.muonGEMDigis.useDBEMap = cms.bool(True)
process.muonGEMDigis.name = cms.string('gemUnPacker--cfgFile')

# Config importation & settings
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.eventsPerJob))
import configureRun_cfi as runConfig
nIdxJob = int(options.idxJob)
strOutput = "out_reco_MC.root" if nIdxJob >= 0 else runConfig.OutputFileName
if nIdxJob < 0: nIdxJob = 0

# Input source
process.source = cms.Source("EmptySource",
    firstRun = cms.untracked.uint32(options.runNum),
    firstEvent = cms.untracked.uint32(options.eventsPerJob * nIdxJob + 1),
    firstLuminosityBlock = cms.untracked.uint32(nIdxJob + 1),
)
process.options = cms.untracked.PSet()

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('CosmicMuonGenerator nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(10485760),
    fileName = cms.untracked.string('file:'+strOutput),
    outputCommands = cms.untracked.vstring( ('drop *')),
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# Cosmic Muon generator
process.generator = cms.EDProducer("CosmicGun",
    AddAntiParticle = cms.bool(True),
    PGunParameters = cms.PSet(
        MinPt = cms.double(99.99),
        MaxPt = cms.double(100.01),
        MinPhi = cms.double(3.141592),
        MaxPhi = cms.double(-3.141592),
        MinTheta = cms.double(1.570796),
        MaxTheta = cms.double(3.141592),
        IsThetaFlat = cms.bool(False), # If 'True': theta distribution is flat. If 'False': theta distribution is a cos^2
        PartID = cms.vint32(-13)
    ),
    Verbosity = cms.untracked.int32(0),
    firstRun = cms.untracked.uint32(1),
    psethack = cms.string('single mu pt 100')
)

process.mix = cms.EDProducer("MixingModule",
    LabelPlayback = cms.string(''),
    bunchspace = cms.int32(450),
    maxBunch = cms.int32(3),
    minBunch = cms.int32(-5),
    mixProdStep1 = cms.bool(False),
    mixProdStep2 = cms.bool(False),
    playback = cms.untracked.bool(False),
    useCurrentProcessOnly = cms.bool(False),
    digitizers = cms.PSet(),
    
    mixObjects = cms.PSet(
        mixSH = cms.PSet(
            crossingFrames = cms.untracked.vstring('MuonGEMHits'),
            input = cms.VInputTag(cms.InputTag("g4SimHits","MuonGEMHits")),
            type = cms.string('PSimHit'),
            subdets = cms.vstring('MuonGEMHits'),            
            )
        ),
    mixTracks = cms.PSet(
        input = cms.VInputTag(cms.InputTag("g4SimHits")),
        makeCrossingFrame = cms.untracked.bool(True),
        type = cms.string('SimTrack')
    ),
)

process.g4SimHits.UseMagneticField = cms.bool(False)
process.simCastorDigis = cms.EDAlias()
process.simEcalUnsuppressedDigis = cms.EDAlias()
process.simHcalUnsuppressedDigis = cms.EDAlias()
process.simSiPixelDigis = cms.EDAlias()
process.simSiStripDigis = cms.EDAlias()

process.load('RecoMuon.TrackingTools.MuonServiceProxy_cff')
process.MuonServiceProxy.ServiceParameters.Propagators.append('StraightLinePropagator')

process.AlignmentTrackRecoQC8 = cms.EDProducer("AlignmentTrackRecoQC8",
                                       process.MuonServiceProxy,
                                       gemRecHitLabel = cms.InputTag("gemRecHits"),
                                       maxClusterSize = cms.double(runConfig.maxClusterSize),
                                       minClusterSize = cms.double(runConfig.minClusterSize),
                                       trackChi2 = cms.double(runConfig.trackChi2),
                                       trackResX = cms.double(runConfig.trackResX),
                                       trackResY = cms.double(runConfig.trackResY),
                                       MulSigmaOnWindow = cms.double(runConfig.MulSigmaOnWindow),
                                       SuperChamberType = cms.vstring(SuperChType),
                                       MuonSmootherParameters = cms.PSet(
                                           PropagatorAlong = cms.string('SteppingHelixPropagatorAny'),
                                           PropagatorOpposite = cms.string('SteppingHelixPropagatorAny'),
                                           RescalingFactor = cms.double(5.0)
                                           ),
                                       isMC = cms.bool(True),
                                       genVtx = cms.InputTag("generator","unsmeared", "RECO"),
                                       shiftX = cms.vdouble(shiftX),
                                       rotationZ = cms.vdouble(rotationZ),
                                       trueDx = cms.vdouble(trueDx),
                                       trueRz = cms.vdouble(trueRz),
                                       )
process.AlignmentTrackRecoQC8.ServiceParameters.GEMLayers = cms.untracked.bool(True)
process.AlignmentTrackRecoQC8.ServiceParameters.CSCLayers = cms.untracked.bool(False)
process.AlignmentTrackRecoQC8.ServiceParameters.RPCLayers = cms.bool(False)

fScale = 1.0

process.AlignmentValidationQC8 = cms.EDProducer('AlignmentValidationQC8',
    process.MuonServiceProxy,
    verboseSimHit = cms.untracked.int32(1),
    simInputLabel = cms.InputTag('g4SimHits',"MuonGEMHits"),
    # PSimHits_g4SimHits_MuonGEMHits_RECO
    genVtx = cms.InputTag("generator","unsmeared", "RECO"),
    # edmHepMCProduct_generator_unsmeared_RECO
    recHitsInputLabel = cms.InputTag('gemRecHits'),
    # GEMDetIdGEMRecHitsOwnedRangeMap_gemRecHits__RECO
    tracksInputLabel = cms.InputTag('AlignmentTrackRecoQC8','','RECO'),
    seedInputLabel = cms.InputTag('AlignmentTrackRecoQC8','','RECO'),
    trajInputLabel = cms.InputTag('AlignmentTrackRecoQC8','','RECO'),
    chNoInputLabel = cms.InputTag('AlignmentTrackRecoQC8','','RECO'),
    seedTypeInputLabel = cms.InputTag('AlignmentTrackRecoQC8','','RECO'),
    # *_AlignmentTrackRecoQC8_*_RECO
    genParticleLabel = cms.InputTag('genParticles','','RECO'),
    # *_genParticles_*_RECO
    gemDigiLabel = cms.InputTag("muonGEMDigis","","RECO"),
    # *_*GEMDigis_*_RECO
    # st1, st2_short, st2_long of xbin, st1,st2_short,st2_long of ybin
    nBinGlobalZR = cms.untracked.vdouble(200,200,200,150,180,250),
    # st1 xmin, xmax, st2_short xmin, xmax, st2_long xmin, xmax, st1 ymin, ymax...
    RangeGlobalZR = cms.untracked.vdouble(564,572,786,794,786,802,110,260,170,350,100,350),
    maxClusterSize = cms.double(runConfig.maxClusterSize),
    minClusterSize = cms.double(runConfig.minClusterSize),
    maxResidual = cms.double(runConfig.maxResidual),
    isMC = cms.bool(True),
    trackChi2 = cms.double(runConfig.trackChi2),
    trackResX = cms.double(runConfig.trackResX),
    trackResY = cms.double(runConfig.trackResY),
    MulSigmaOnWindow = cms.double(runConfig.MulSigmaOnWindow),
    SuperChamberType = cms.vstring(SuperChType),
    MuonSmootherParameters = cms.PSet(
                      PropagatorAlong = cms.string('SteppingHelixPropagatorAny'),
                      PropagatorOpposite = cms.string('SteppingHelixPropagatorAny'),
                      RescalingFactor = cms.double(5.0)
                      ),
    shiftX = cms.vdouble(shiftX),
    rotationZ = cms.vdouble(rotationZ),
    trueDx = cms.vdouble(trueDx),
    trueRz = cms.vdouble(trueRz),
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('temp_'+strOutput)
)

process.rawDataCollector.RawCollectionList = cms.VInputTag(cms.InputTag("gemPacker"))
# Path and EndPath definitions
process.generation_step = cms.Path(process.generator+process.pgen)
process.simulation_step = cms.Path(process.psim)
process.digitisation_step = cms.Path(process.mix+process.simMuonGEMDigis)
process.reconstruction_step = cms.Path(process.gemPacker+process.rawDataCollector+process.muonGEMDigis+process.gemLocalReco+process.AlignmentTrackRecoQC8)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)
process.validation_step = cms.Path(process.AlignmentValidationQC8)
process.digitisation_step.remove(process.simEcalTriggerPrimitiveDigis)
process.digitisation_step.remove(process.simEcalDigis)
process.digitisation_step.remove(process.simEcalPreshowerDigis)
process.digitisation_step.remove(process.simHcalTriggerPrimitiveDigis)
process.digitisation_step.remove(process.simHcalDigis)
process.digitisation_step.remove(process.simHcalTTPDigis)
process.digitisation_step.remove(process.simMuonCSCDigis)
process.digitisation_step.remove(process.simMuonRPCDigis)
process.digitisation_step.remove(process.addPileupInfo)
process.digitisation_step.remove(process.simMuonDTDigis)


process.msgStart = cms.EDAnalyzer('GEMMessager',
                                  message = cms.string('start'),
                                  printMsg = cms.int32(1))
process.msgAfterGen = process.msgStart.clone( message = cms.string('afterGen') )
process.msgAfterGenFilter = process.msgStart.clone( message = cms.string('afterGenFilter') )
process.msgAfterSim = process.msgStart.clone( message = cms.string('afterSim') )
process.msgAfterDigi = process.msgStart.clone( message = cms.string('afterDigi') )
process.msgAfterReco = process.msgStart.clone( message = cms.string('afterReco') )
process.msgAfterVal = process.msgStart.clone( message = cms.string('afterVal') )

process.seq_msgStart = cms.Path( process.msgStart )
process.seq_msgAfterGen = cms.Path( process.msgAfterGen )
process.seq_msgAfterGenFilter = cms.Path( process.msgAfterGenFilter )
process.seq_msgAfterSim = cms.Path( process.msgAfterSim )
process.seq_msgAfterDigi = cms.Path( process.msgAfterDigi )
process.seq_msgAfterReco = cms.Path( process.msgAfterReco )
process.seq_msgAfterVal = cms.Path( process.msgAfterVal )
# Schedule definition
process.schedule = cms.Schedule(
    process.seq_msgStart,
    process.generation_step,
    process.seq_msgAfterGen,
    process.genfiltersummary_step,
    process.seq_msgAfterGenFilter,
    process.simulation_step,
    process.seq_msgAfterSim,
    process.digitisation_step,
    process.seq_msgAfterDigi,
    process.reconstruction_step,
    process.seq_msgAfterReco,
    process.validation_step,
    process.seq_msgAfterVal,
    process.endjob_step,
    process.FEVTDEBUGHLToutput_step
    )

process.RandomNumberGeneratorService.generator = cms.PSet(
    initialSeed = cms.untracked.uint32( ( nIdxJob + 1 ) + options.runNum*10000),
    engineName = cms.untracked.string('HepJamesRandom')
)
process.RandomNumberGeneratorService.simMuonGEMDigis = process.RandomNumberGeneratorService.generator
process.RandomNumberGeneratorService.VtxSmeared = process.RandomNumberGeneratorService.generator
process.RandomNumberGeneratorService.g4SimHits = process.RandomNumberGeneratorService.generator

process.gemSegments.maxRecHitsInCluster = cms.int32(10)
process.gemSegments.minHitsPerSegment = cms.uint32(3)
process.gemSegments.clusterOnlySameBXRecHits = cms.bool(True)
process.gemSegments.dEtaChainBoxMax = cms.double(1.05)
process.gemSegments.dPhiChainBoxMax = cms.double(1.12)
process.gemSegments.dXclusBoxMax = cms.double(10.0)
process.gemSegments.dYclusBoxMax = cms.double(50.0)
process.gemSegments.preClustering = cms.bool(False)
process.gemSegments.preClusteringUseChaining = cms.bool(False)

process.simMuonGEMDigis.averageEfficiency = cms.double(0.98)
process.simMuonGEMDigis.averageNoiseRate = cms.double(0.0)
process.simMuonGEMDigis.simulateIntrinsicNoise = cms.bool(False)
process.simMuonGEMDigis.doBkgNoise = cms.bool(False)
process.simMuonGEMDigis.doNoiseCLS = cms.bool(False)
process.simMuonGEMDigis.simulateElectronBkg = cms.bool(False)
