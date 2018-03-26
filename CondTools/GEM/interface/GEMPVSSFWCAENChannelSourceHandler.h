#ifndef CondTools_GEM_GEMPVSSFWCAENChannelSourceHandler_h
#define CondTools_GEM_GEMPVSSFWCAENChannelSourceHandler_h

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CoralBase/TimeStamp.h"

#include <string>

namespace popcon
{
  class GEMPVSSFWCAENChannelSourceHandler : public popcon::PopConSourceHandler<GEMPVSSFWCAENChannel>
  {

  public:

    GEMPVSSFWCAENChannelSourceHandler( const edm::ParameterSet& ps );
    ~GEMPVSSFWCAENChannelSourceHandler();
    void getNewObjects();
    void ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset ); // additional work
    void DisconnectOnlineDB(); // additional work
    void readGEMPVSSFWCAENChannel();
    void listDBTablesInSchema(); // only list tables in the given schema
    std::string id() const { return m_name; }
    static coral::TimeStamp convertTimeStamp(long long dateInt, bool isStartTime);

  private:
    GEMPVSSFWCAENChannel * fwCh;
    cond::persistency::Session session; // additional work
    std::string m_name;
    int m_dummy;
    int m_validate;
    int m_sliceTest;
    int m_listTables;
    std::string m_connect;
    std::string m_authpath;
    edm::ParameterSet m_connectionPset;
    long long m_sinceTime, m_untilTime;
    bool m_printValues;
  };
}
#endif
