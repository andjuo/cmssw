#ifndef CondTools_GEM_GEMListTablesSourceHandler_h
#define CondTools_GEM_GEMListTablesSourceHandler_h

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CoralBase/TimeStamp.h"

#include <string>

namespace popcon
{
  class GEMListTablesSourceHandler : public popcon::PopConSourceHandler<GEMPVSSFWCAENChannel>
  {

  public:

    GEMListTablesSourceHandler( const edm::ParameterSet& ps );
    ~GEMListTablesSourceHandler();
    std::string id() const { return std::string("GEMListTablesSourceHandler"); }
    void getNewObjects(); // no work will be done
    void listDBTablesInSchema(); // only list tables in the given schema

  private:
    //cond::persistency::Session session; // additional work
    std::string m_connect;
    //std::string m_authpath;
    edm::ParameterSet m_connectionPset;
  };
}
#endif
