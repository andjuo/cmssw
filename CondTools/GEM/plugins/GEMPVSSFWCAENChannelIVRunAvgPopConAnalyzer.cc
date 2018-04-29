#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<GEMPVSSFWCAENChannelIVRunAvgSourceHandler> GEMPVSSFWCAENChannelIVRunAvgPopConAnalyzer;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelIVRunAvgPopConAnalyzer);
