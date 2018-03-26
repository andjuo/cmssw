#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMPVSSFWCAENChannelSourceHandler> GEMPVSSFWCAENChannelDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMPVSSFWCAENChannelDBWriter);
