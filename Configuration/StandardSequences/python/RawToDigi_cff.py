import FWCore.ParameterSet.Config as cms

# This object is used to selectively make changes for different running
# scenarios. In this case it makes changes for Run 2.

from EventFilter.SiPixelRawToDigi.SiPixelRawToDigi_cfi import *

from EventFilter.SiStripRawToDigi.SiStripDigis_cfi import *

from SimCalorimetry.EcalTrigPrimProducers.ecalTriggerPrimitiveDigis_cff import *

import EventFilter.EcalRawToDigi.EcalUnpackerData_cfi
ecalDigis = EventFilter.EcalRawToDigi.EcalUnpackerData_cfi.ecalEBunpacker.clone()

import EventFilter.ESRawToDigi.esRawToDigi_cfi
ecalPreshowerDigis = EventFilter.ESRawToDigi.esRawToDigi_cfi.esRawToDigi.clone()

import EventFilter.HcalRawToDigi.HcalRawToDigi_cfi
hcalDigis = EventFilter.HcalRawToDigi.HcalRawToDigi_cfi.hcalDigis.clone()

import EventFilter.CSCRawToDigi.cscUnpacker_cfi
muonCSCDigis = EventFilter.CSCRawToDigi.cscUnpacker_cfi.muonCSCDigis.clone()

import EventFilter.DTRawToDigi.dtunpacker_cfi
muonDTDigis = EventFilter.DTRawToDigi.dtunpacker_cfi.muonDTDigis.clone()

import EventFilter.RPCRawToDigi.rpcUnpacker_cfi
muonRPCDigis = EventFilter.RPCRawToDigi.rpcUnpacker_cfi.rpcunpacker.clone()

import EventFilter.GEMRawToDigi.gemUnpacker_cfi
muonGEMDigis = EventFilter.GEMRawToDigi.gemUnpacker_cfi.muonGEMDigis.clone()

import EventFilter.GEMRawToDigi.me0Unpacker_cfi
muonME0Digis = EventFilter.GEMRawToDigi.me0Unpacker_cfi.muonME0Digis.clone()

from EventFilter.CastorRawToDigi.CastorRawToDigi_cff import *
castorDigis = EventFilter.CastorRawToDigi.CastorRawToDigi_cfi.castorDigis.clone( FEDs = cms.untracked.vint32(690,691,692, 693,722) )

from EventFilter.ScalersRawToDigi.ScalersRawToDigi_cfi import *

from EventFilter.Utilities.tcdsRawToDigi_cfi import *
tcdsDigis = EventFilter.Utilities.tcdsRawToDigi_cfi.tcdsRawToDigi.clone()

from L1Trigger.Configuration.L1TRawToDigi_cff import *

from EventFilter.CTPPSRawToDigi.ctppsRawToDigi_cff import *

RawToDigi = cms.Sequence(L1TRawToDigi
                         +siPixelDigis
                         +siStripDigis
                         +ecalDigis
                         +ecalPreshowerDigis
                         +hcalDigis
                         +muonCSCDigis
                         +muonDTDigis
                         +muonRPCDigis
                         +castorDigis
                         +scalersRawToDigi
                         +tcdsDigis
                         )

RawToDigi_noTk = cms.Sequence(L1TRawToDigi
                              +ecalDigis
                              +ecalPreshowerDigis
                              +hcalDigis
                              +muonCSCDigis
                              +muonDTDigis
                              +muonRPCDigis
                              +castorDigis
                              +scalersRawToDigi
                              +tcdsDigis
                              )
    
scalersRawToDigi.scalersInputTag = 'rawDataCollector'
siPixelDigis.InputLabel = 'rawDataCollector'
#false by default anyways ecalDigis.DoRegional = False
ecalDigis.InputLabel = 'rawDataCollector'
ecalPreshowerDigis.sourceTag = 'rawDataCollector'
hcalDigis.InputLabel = 'rawDataCollector'
muonCSCDigis.InputObjects = 'rawDataCollector'
muonDTDigis.inputLabel = 'rawDataCollector'
muonRPCDigis.InputLabel = 'rawDataCollector'
castorDigis.InputLabel = 'rawDataCollector'

from Configuration.Eras.Modifier_run3_common_cff import run3_common
run3_common.toReplaceWith(RawToDigi, RawToDigi.copyAndExclude([castorDigis]))

from Configuration.Eras.Modifier_phase2_tracker_cff import phase2_tracker
# Remove siPixelDigis until we have phase1 pixel digis
phase2_tracker.toReplaceWith(RawToDigi, RawToDigi.copyAndExclude([siPixelDigis])) # FIXME


# add CTPPS 2016 raw-to-digi modules
from Configuration.Eras.Modifier_ctpps_2016_cff import ctpps_2016

_ctpps_2016_RawToDigi = RawToDigi.copy()
_ctpps_2016_RawToDigi += ctppsRawToDigi
ctpps_2016.toReplaceWith(RawToDigi, _ctpps_2016_RawToDigi)

_ctpps_2016_RawToDigi_noTk = RawToDigi_noTk.copy()
_ctpps_2016_RawToDigi_noTk += ctppsRawToDigi
ctpps_2016.toReplaceWith(RawToDigi_noTk, _ctpps_2016_RawToDigi_noTk)

# GEM settings
_gem_RawToDigi = RawToDigi.copy()
_gem_RawToDigi.insert(-1,muonGEMDigis)

_me0_RawToDigi = RawToDigi.copy()
_me0_RawToDigi.insert(-1,muonGEMDigis*muonME0Digis)

from Configuration.Eras.Modifier_run2_GEM_2017_cff import run2_GEM_2017
run2_GEM_2017.toReplaceWith(RawToDigi, _gem_RawToDigi)

from Configuration.Eras.Modifier_run3_GEM_cff import run3_GEM
run3_GEM.toReplaceWith(RawToDigi, _gem_RawToDigi)

from Configuration.Eras.Modifier_phase2_muon_cff import phase2_muon
phase2_muon.toReplaceWith(RawToDigi, _me0_RawToDigi)
