#ifndef CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgReadDBs_h
#define CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgReadDBs_h

#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannelIVRunAvg.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <string>

/*
 *  \class GEMPVSSFWCAENChannelIVRunAvgReadDBs
 *
 *  Creates the GEMPVSSFWCAENChannelIVRunAvg record
 *  (run-averages for Imon and Vmon from CMS_GEM_PVSS_COND.FWCAENCHANNELA1515)
 *  based on RunInfo and PVSS DB data.
 *
 *  \author Andrius Juodagalvis (andjuo) - Vilnius University (Apr 15, 2018)
 *
*/

// Implementation based on CondTools/RunInfo/src/RunInfoRead.cc

class GEMPVSSFWCAENChannelIVRunAvgReadDBs {
 public:
  GEMPVSSFWCAENChannelIVRunAvgReadDBs(const std::string& connectionString,
				      const edm::ParameterSet& connectionPset);
  ~GEMPVSSFWCAENChannelIVRunAvgReadDBs();
  GEMPVSSFWCAENChannelIVRunAvg readData(const std::string& runinfo_schema, const std::string& pvss_schema, const int r_number);
 private:
  std::string m_connectionString;
  edm::ParameterSet m_connectionPset;
};

#endif
