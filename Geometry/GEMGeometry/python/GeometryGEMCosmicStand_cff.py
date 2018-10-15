import FWCore.ParameterSet.Config as cms

#from Geometry.MuonCommonData.gembox_cfi import *
from Geometry.MuonCommonData.gemboxCosmicStand_cfi import *

from CondTools.GEM.gemQC8ConfESSource_cfi import *
XMLIdealGeometryESSource.DBSource = GEMQC8ConfESSource

from Geometry.GEMGeometryBuilder.gemGeometry_cfi import *
from RecoMuon.DetLayers.muonDetLayerGeometry_cfi import *
from Geometry.CommonDetUnit.bareGlobalTrackingGeometry_cfi import *
from Geometry.MuonNumbering.muonNumberingInitialization_cfi import *
