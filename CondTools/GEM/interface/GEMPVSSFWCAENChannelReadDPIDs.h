#ifndef CondTools_GEM_GEMPVSSFWCAENChannelReadDPIDs_h
#define CondTools_GEM_GEMPVSSFWCAENChannelReadDPIDs_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <string>

/*
 *  \class GEMPVSSFWCAENChannelReadDPIDs
 *
 *  Reads DPID aliases
 *
 *
 *
 *  \author Andrius Juodagalvis (andjuo) - Vilnius University (May 8, 2018)
 *
*/

class GEMPVSSFWCAENChannelReadDPIDs {
 public:
  typedef std::map<int,std::string> TDPIDAliasMap;
 public:
  GEMPVSSFWCAENChannelReadDPIDs(const std::string& connectionString,
				const edm::ParameterSet& connectionPset);
  ~GEMPVSSFWCAENChannelReadDPIDs();
  GEMPVSSFWCAENChannelReadDPIDs::TDPIDAliasMap readData(const std::string& dpid_schema);
 private:
  std::string m_connectionString;
  edm::ParameterSet m_connectionPset;
};

#endif
