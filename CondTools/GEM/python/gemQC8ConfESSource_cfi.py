import FWCore.ParameterSet.Config as cms

from CondCore.CondDB.CondDB_cfi import *
SourceDBConnection = CondDB.clone( connect = cms.string('oracle://INT2R/CMS_GEM_MUON_VIEW') )
SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('.')
SourceDBConnection.DBParameters.authenticationSystem = cms.untracked.int32(2)

GEMQC8ConfESSource = cms.ESSource("GEMQC8ConfESSource",
    SourceDBConnection,
    runNumber = cms.int32(1),
    DebugMode = cms.untracked.int32(1),
    WriteDummy = cms.untracked.int32(0),#fakeData for testing
    NoDBOutput = cms.untracked.int32(1), # whether PoolDBOutputService is established
    OnlyConfDef = cms.untracked.int32(1), # whether EMAP should be obtained
    printValues = cms.untracked.bool( True ), # whether to print obtained values
)
