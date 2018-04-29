#ifndef CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgSourceHandler_H
#define CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgSourceHandler_H

//#include <string>

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannelIVRunAvg.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class GEMPVSSFWCAENChannelIVRunAvgSourceHandler : public popcon::PopConSourceHandler<GEMPVSSFWCAENChannelIVRunAvg> {
 public:
  GEMPVSSFWCAENChannelIVRunAvgSourceHandler(const edm::ParameterSet& pset);
  ~GEMPVSSFWCAENChannelIVRunAvgSourceHandler() override;
  void getNewObjects() override;
  std::string id() const override { return m_name; }

 private:
  unsigned long long m_since;
  std::string m_name;

  // for reading from omds
  std::string m_runinfo_schema;
  std::string m_pvss_schema;
  std::string m_connectionString;
  edm::ParameterSet m_connectionPset;
};

#endif
