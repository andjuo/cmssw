#include "CondCore/ESSources/interface/registration_macros.h"
#include "CondFormats/GEMObjects/interface/GEMEMap.h"
#include "CondFormats/DataRecord/interface/GEMEMapRcd.h"
#include "CondFormats/GEMObjects/interface/ME0EMap.h"
#include "CondFormats/DataRecord/interface/ME0EMapRcd.h"
#include "CondFormats/GEMObjects/interface/GEMMapItem.h"
#include "CondFormats/DataRecord/interface/GEMMapItemRcd.h"
#include "CondFormats/GEMObjects/interface/GEMDeadStrips.h"
#include "CondFormats/DataRecord/interface/GEMDeadStripsRcd.h"
#include "CondFormats/GEMObjects/interface/GEMMaskedStrips.h"
#include "CondFormats/DataRecord/interface/GEMMaskedStripsRcd.h"


REGISTER_PLUGIN(GEMEMapRcd,GEMEMap);
REGISTER_PLUGIN(ME0EMapRcd,ME0EMap);
REGISTER_PLUGIN(GEMMapItemRcd,GEMMapItem);
REGISTER_PLUGIN(GEMDeadStripsRcd,GEMDeadStrips);
REGISTER_PLUGIN(GEMMaskedStripsRcd,GEMMaskedStrips);
