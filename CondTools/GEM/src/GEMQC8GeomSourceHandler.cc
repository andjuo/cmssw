#include "CondTools/GEM/interface/GEMQC8GeomSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

//#include "RelationalAccess/ISessionProxy.h"
//#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
//#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
//#include "CoralBase/AttributeSpecification.h"

//#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>
//#include <cstdlib>
#include <vector>

#include <sstream>

//#include <DataFormats/MuonDetId/interface/GEMDetId.h>

inline void HERE(std::string msg)
{ std::cout << msg << std::endl; }


popcon::GEMQC8GeomSourceHandler::GEMQC8GeomSourceHandler( const edm::ParameterSet& ps ):
  m_name( ps.getUntrackedParameter<std::string>( "name", "GEMQC8GeomSourceHandler" ) ),
  m_dummy( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  m_connect( ps.getParameter<std::string>( "connect" ) ),
  m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) ),
  m_runNumber( ps.getParameter<int>("runNumber") ),
  m_printValues( ps.getUntrackedParameter<bool>( "printValues", false ) )
{
  std::cout << "GEMQC8GeomSourceHandler constructor\n";
  std::cout << "  * m_name = " << m_name << "\n";
  std::cout << "  * m_dummy= " << m_dummy << "\n";
  std::cout << "  * m_connect= " << m_connect << "\n";
  std::cout << "  * DBParameters= " << m_connectionPset << "\n";
  std::cout << "  * m_runNumber=" << m_runNumber << "\n";
  std::cout << "  * m_printValues=" << m_printValues << "\n";
}

popcon::GEMQC8GeomSourceHandler::~GEMQC8GeomSourceHandler()
{
}


void popcon::GEMQC8GeomSourceHandler::getNewObjects()
{
  std::cout << "GEMQC8GeomSourceHandler getNewObjects\n";

  edm::LogInfo( "GEMQC8GeomSourceHandler" ) << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "BEGIN" << std::endl;

  // first check what is already there in offline DB
  Ref payload;
  if(tagInfo().size>0) {
    edm::LogInfo( "GEMQC8GeomSourceHandler" )
      << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
      << "Validation was requested, so will check present contents\n"
      << "Destination Tag Info: name " << tagInfo().name
      << ", size " << tagInfo().size
      << ", last object valid since " << tagInfo().lastInterval.first
      << ", hash " << tagInfo().lastPayloadToken << std::endl;
    payload = lastPayload();
  }

  qc8geom =  new GEMQC8Geom();

  if (m_dummy==0) {
    std::cout << "\n\tcalling ConnectOnlineDB" << std::endl;
    ConnectOnlineDB( m_connect, m_connectionPset );
    readGEMQC8Geom();
    DisconnectOnlineDB();
  }
  else {
    std::cout << "\n putting dummy data\n\n";
    qc8geom->run_number_ = 0;
    for (unsigned int i=0; i<15; i++) {
      qc8geom->chSerialNums_.push_back("L");
      std::stringstream ss;
      ss << "c" << (i/5+1) << "_r" << (i%5+1);
      qc8geom->chPositions_.push_back(ss.str());
      qc8geom->chGasFlow_.push_back(1.23);
    }
  }

  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  cond::Time_t snc = mydbservice->currentTime();
  // don't look for recent changes
  int difference=1;
  if (difference==1) {
    std::cout << "GEMQC8GeomSourceHandler getNewObjects difference=1\n";
    m_to_transfer.push_back(std::make_pair((GEMQC8Geom*)qc8geom,snc));
    edm::LogInfo( "GEMQC8GeomSourceHandler" )
      << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
      << "QC8Geom runNumber=" << (*qc8geom).run_number_
      << ", sizes: " << (*qc8geom).chSerialNums_.size() << ", "
      << (*qc8geom).chPositions_.size() << ", " << (*qc8geom).chGasFlow_.size()
      << ", payloads to transfer: " << m_to_transfer.size() << std::endl;
  }
  edm::LogInfo( "GEMQC8GeomSourceHandler" )
    << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
    << "END." << std::endl;
}


// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMQC8GeomSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  //std::cout << "GEMQC8GeomSourceHandler ConnectOnlineDB\n";
  cond::persistency::ConnectionPool connection;
  edm::LogInfo( "GEMQC8GeomSourceHandler" ) << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEMEMapConfigSourceHandler: connecting to " << connect << "..." << std::endl;
  connection.setParameters( connectionPset );
  //std::cout << "connectionPset = " << connectionPset << "\n";
  connection.configure();
  session = connection.createSession( connect,true );
  //session = connection.createSession( connect,false ); // not writeCapable
  edm::LogInfo( "GEMQC8GeomSourceHandler" ) << "[" << "GEMQC8GeomSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "Done." << std::endl;
  //std::cout << "GEMQC8GeomSourceHandler ConnectOnlineDB leaving" << std::endl;
}

void popcon::GEMQC8GeomSourceHandler::DisconnectOnlineDB()
{
  session.close();
}

void popcon::GEMQC8GeomSourceHandler::readGEMQC8Geom()
{
  std::cout << "\nGEMQC8GeomSourceHandler readGEMQC8Geom" << std::endl;

  session.transaction().start( true );
  std::cout << "transaction started" << std::endl;
  coral::ISchema& schema = session.nominalSchema();
  std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;

  std::cout << std::endl <<"GEMQC8GeomSourceHandler: start to build GEMQC8Geom..." << std::flush << std::endl << std::endl;

  coral::IQuery* query1 = schema.newQuery();
  query1->addToTableList( "QC8_GEM_STAND_GEOMETRY_VIEW_RH");
  query1->addToOutputList("QC8_GEM_STAND_GEOMETRY_VIEW_RH.CH_SERIAL_NUMBER", "CH_SERIAL_NUMBER");
  query1->addToOutputList("QC8_GEM_STAND_GEOMETRY_VIEW_RH.POSITION", "POSITION");
  query1->addToOutputList("QC8_GEM_STAND_GEOMETRY_VIEW_RH.FLOW_METER", "FLOW_METER");
  query1->addToOutputList("QC8_GEM_STAND_GEOMETRY_VIEW_RH.RUN_NUMBER", "RUN_NUMBER");

  coral::AttributeList conditionData; // empty
  std::string condition;
  {
    std::stringstream ssCond;
    ssCond << "RUN_NUMBER = " << m_runNumber;
    condition = ssCond.str();
  }

  // IMon values
  query1->setCondition( condition, conditionData );
  coral::ICursor& cursor = query1->execute();
  std::cout<<"cursor OK"<<std::endl;
  qc8geom->run_number_ = m_runNumber;
  while ( cursor.next() ) {
    const coral::AttributeList& row = cursor.currentRow();
    try {
      int db_runnumber = row["RUN_NUMBER"].data<int>();
      std::string db_chSerNum = row["CH_SERIAL_NUMBER"].data<std::string>();
      std::string db_chPos = row["POSITION"].data<std::string>();
      float db_flow = row["FLOW_METER"].data<float>();

      qc8geom->chSerialNums_.push_back(db_chSerNum);
      qc8geom->chPositions_.push_back(db_chPos);
      qc8geom->chGasFlow_.push_back(db_flow);

      if (m_printValues) {
	std::cout << "db: " << db_runnumber << ", " << db_chSerNum
		  << ", " << db_chPos << ", " << db_flow << std::endl;
      }
    }
    catch ( const std::exception & e ) {
      std::cout << "exception " << e.what() << " caught\n";
      //continue;
    }
  }

  delete query1;

  std::cout << "GEMQC8GeomSourceHandler readGEMQC8Geom done" << std::endl;

}
