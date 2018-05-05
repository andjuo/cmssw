#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CondTools/GEM/interface/GEMListTablesSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMListTablesSourceHandler> GEMListTablesDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMListTablesDBWriter);
