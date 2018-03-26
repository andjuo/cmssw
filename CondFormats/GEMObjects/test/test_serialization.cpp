#include "CondFormats/Serialization/interface/Test.h"
#include "CondFormats/GEMObjects/src/headers.h"

int main()
{
  testSerialization<GEMELMap>();
  testSerialization<GEMELMap::GEMVFatMap>();
  testSerialization<std::vector<GEMELMap::GEMVFatMap>>();
  testSerialization<GEMELMap::GEMStripMap>();
  testSerialization<std::vector<GEMELMap::GEMStripMap>>();
  testSerialization<GEMDeadStrips>();
  testSerialization<std::vector<GEMDeadStrips>>();
  testSerialization<GEMMaskedStrips>();
  testSerialization<std::vector<GEMMaskedStrips>>();
  testSerialization<GEMPVSSFWCAENChannel>();
  testSerialization<std::vector<GEMPVSSFWCAENChannel>>();
}
