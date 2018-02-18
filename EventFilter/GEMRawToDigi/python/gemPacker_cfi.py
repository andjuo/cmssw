import FWCore.ParameterSet.Config as cms 

set_useDBEMap=False

from Configuration.Eras.Modifier_run2_GEM_2017_cff import run2_GEM_2017
from Configuration.Eras.Modifier_run2_GEM_2017_MCTest_cff import run2_GEM_2017_MCTest
from Configuration.Eras.Modifier_run3_GEM_cff import run3_GEM

if not run2_GEM_2017_MCTest.isChosen():
    if run2_GEM_2017.isChosen() or run3_GEM.isChosen() :
        from EventFilter.GEMRawToDigi.GEMSQLiteCabling_cfi import GEMCabling
        set_useDBEMap=True


gemPacker = cms.EDProducer("GEMDigiToRawModule",
    eventType = cms.int32(0),
    gemDigi = cms.InputTag("simMuonGEMDigis"),
    useDBEMap = cms.bool(set_useDBEMap)
)

