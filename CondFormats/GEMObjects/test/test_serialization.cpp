
#include "CondFormats/Serialization/interface/Test.h"
#include "CondFormats/GEMObjects/src/headers.h"

int main()
{
  testSerialization<GEMEMap>();
  testSerialization<GEMEMap::GEMEMapItem>();
  testSerialization<std::vector<GEMEMap::GEMEMapItem>>();
  testSerialization<GEMEMap::GEMVFatMaptype>();
  testSerialization<std::vector<GEMEMap::GEMVFatMaptype>>();
  testSerialization<GEMEMap::GEMVFatMapInPos>();
  testSerialization<std::vector<GEMEMap::GEMVFatMapInPos>>();

  testSerialization<ME0EMap>();
  testSerialization<ME0EMap::ME0EMapItem>();
  testSerialization<std::vector<ME0EMap::ME0EMapItem>>();
  testSerialization<ME0EMap::ME0VFatMaptype>();
  testSerialization<std::vector<ME0EMap::ME0VFatMaptype>>();
  testSerialization<ME0EMap::ME0VFatMapInPos>();
  testSerialization<std::vector<ME0EMap::ME0VFatMapInPos>>();

  testSerialization<GEMMapItem>();
  testSerialization<GEMMapItem::MapItem>();

  testSerialization<GEMDeadStrips>();
  testSerialization<GEMDeadStrips::DeadItem>();

  testSerialization<GEMMaskedStrips>();
  testSerialization<GEMMaskedStrips::MaskItem>();

}
