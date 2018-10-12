#ifndef CondTools_GEM_GEMQC8GeomSourceHandler_h
#define CondTools_GEM_GEMQC8GeomSourceHandler_h

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "CoralBase/TimeStamp.h"

#include <string>

namespace popcon
{
  class GEMQC8GeomSourceHandler : public popcon::PopConSourceHandler<GEMQC8Geom>
  {

  public:

    GEMQC8GeomSourceHandler( const edm::ParameterSet& ps );
    ~GEMQC8GeomSourceHandler();
    void getNewObjects();
    void ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset );
    void DisconnectOnlineDB();
    void readGEMQC8Geom();
    std::string id() const { return m_name; }

  private:
    GEMQC8Geom * qc8geom;
    cond::persistency::Session session;
    std::string m_name;
    int m_dummy;
    std::string m_connect;
    std::string m_authpath;
    edm::ParameterSet m_connectionPset;
    int m_runNumber;
    bool m_printValues;
  };
}
#endif
