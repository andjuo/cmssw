#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelReadDPIDs.h"
//#include "CondTools/RunInfo/interface/RunSummaryRead.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/TimeStamp.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>
#include <cmath>

GEMPVSSFWCAENChannelReadDPIDs::GEMPVSSFWCAENChannelReadDPIDs( const std::string& connectionString,
	  const edm::ParameterSet& connectionPset ):
  m_connectionString( connectionString ),
  m_connectionPset( connectionPset )
{}

GEMPVSSFWCAENChannelReadDPIDs::~GEMPVSSFWCAENChannelReadDPIDs() {}

GEMPVSSFWCAENChannelReadDPIDs::TDPIDAliasMap
GEMPVSSFWCAENChannelReadDPIDs::readData(const std::string & dpid_schema )
{
  GEMPVSSFWCAENChannelReadDPIDs::TDPIDAliasMap temp_aliasMap;

  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: Initialising Connection Pool" << std::endl;
  cond::persistency::ConnectionPool connection;
  connection.setParameters( m_connectionPset );
  connection.configure();
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: Initialising read-only session to " << m_connectionString << std::endl;
  std::shared_ptr<coral::ISessionProxy> session = connection.createCoralSession( m_connectionString, false );

  try{
    session->transaction().start( true );
    coral::ISchema& schema = session->schema( dpid_schema );
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: Accessing schema " << dpid_schema << std::endl;
    //new query to obtain the start_time
    std::unique_ptr<coral::IQuery> query( schema.newQuery() );
    query->addToTableList( "DP_NAME2ID" );
    query->addToTableList( "TEST2" );
    query->addToOutputList( "DP_NAME2ID.ID", "DPID" );
    query->addToOutputList( "TEST2.ALIAS" , "ALIAS" );

    std::string condition = "TEST2.DPE_NAME=(DP_NAME2ID.DPNAME||'.')";
    coral::AttributeList conditionData;
    query->setCondition( condition, conditionData );
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader") << " calling query->execute()" << std::endl;
    coral::ICursor& cursor = query->execute();
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader") << " calling query->execute() done" << std::endl;

    std::ostringstream oss;
    while ( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      float dpid_f= row["DPID"].data<float>();
      int dpid= static_cast<int>(dpid_f);
      std::string alias=row["ALIAS"].data<std::string>();
      temp_aliasMap[dpid]= alias;

      //oss << "  " << temp_aliasMap.size() << " dpid=" << dpid << "(" << dpid_f << ") alias=" << alias << "\n";
    }
    if (oss.str().size())
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgRead" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: Loaded aliases:\n" << oss.str() << std::endl;

    session->transaction().commit();
  }
  catch (const std::exception& e) {
    throw cms::Exception( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: "
			 << "Unable to read data. Original Exception:\n"
			 << e.what() << std::endl;
  }

  if (0) {
    std::ostringstream oss;
    oss << "\n Test temp_aliasMap:\n";
    for ( auto it : temp_aliasMap ) {
      oss << "  " << it.first << "  " << it.second << "\n";
    }
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelReadDPIDs::" << __func__ << "]: " << oss.str() << std::endl;
  }

  return temp_aliasMap;
}
