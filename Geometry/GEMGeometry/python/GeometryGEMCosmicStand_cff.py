import FWCore.ParameterSet.Config as cms

#from Geometry.MuonCommonData.gembox_cfi import *
#from Geometry.MuonCommonData.gemboxCosmicStand_cfi import *

from CondTools.GEM.gemQC8ConfESSource_cfi import *

#XMLIdealGeometryESSource.DBSource = cms.untracked.PSet(
#    connect = GEMQC8ConfESSource.connect,
#    DBParameters = GEMQC8ConfESSource.DBParameters,
#    DebugMode = cms.untracked.int32(0),
#    WriteDummy = cms.untracked.int32(0),#fakeData for testing
#    NoDBOutput = cms.untracked.int32(1), # whether PoolDBOutputService is established
#    OnlyConfDef = cms.untracked.int32(1), # whether to not load ELMap
#    printValues = cms.untracked.bool( False ), # whether to print obtained values
#    runNumber = cms.int32(1)
#)

from Geometry.GEMGeometryBuilder.gemGeometry_cfi import *
from RecoMuon.DetLayers.muonDetLayerGeometry_cfi import *
from Geometry.CommonDetUnit.bareGlobalTrackingGeometry_cfi import *
from Geometry.MuonNumbering.muonNumberingInitialization_cfi import *
