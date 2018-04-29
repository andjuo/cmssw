#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgPlotter.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<GEMPVSSFWCAENChannelIVRunAvgPlotter> GEMPVSSFWCAENChannelIVRunAvgPopConPlotter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelIVRunAvgPopConPlotter);
