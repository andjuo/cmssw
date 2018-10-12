#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CondTools/GEM/interface/GEMQC8GeomSourceHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::GEMQC8GeomSourceHandler> GEMQC8GeomDBWriter;
//define this as a plug-in
DEFINE_FWK_MODULE(GEMQC8GeomDBWriter);
