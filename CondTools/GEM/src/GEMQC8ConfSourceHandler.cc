#include "CondTools/GEM/interface/GEMQC8ConfSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
//#include "CoralBase/AttributeSpecification.h"

//#include <fstream>
#include <vector>
#include <sstream>

//inline void HERE(std::string msg)
//{ std::cout << msg << std::endl; }


popcon::GEMQC8ConfSourceHandler::GEMQC8ConfSourceHandler( const edm::ParameterSet& ps ):
  m_name( ps.getUntrackedParameter<std::string>( "name", "GEMQC8ConfSourceHandler" ) ),
  m_dummy( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  m_debugMode( ps.getUntrackedParameter<int>( "DebugMode", 0 ) ),
  m_connect( ps.getParameter<std::string>( "connect" ) ),
  m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) ),
  m_runNumber( ps.getParameter<int>("runNumber") ),
  m_allowRollBack( ps.getUntrackedParameter<int>( "AllowRollBack", 1 ) ),
  m_noDBOutput( ps.getUntrackedParameter<int>( "NoDBOutput", 0 ) ),
  m_onlyConfDef( ps.getUntrackedParameter<int>( "OnlyConfDef", 1 ) ),
  m_printValues( ps.getUntrackedParameter<bool>( "printValues", false ) )
{
  if (m_printValues) {
    std::cout << "GEMQC8ConfSourceHandler constructor\n";
    std::cout << "  * m_name = " << m_name << "\n";
    std::cout << "  * m_dummy= " << m_dummy << "\n";
    std::cout << "  * m_debugMode= " << m_debugMode << "\n";
    std::cout << "  * m_connect= " << m_connect << "\n";
    std::cout << "  * DBParameters= " << m_connectionPset << "\n";
    std::cout << "  * m_runNumber=" << m_runNumber << "\n";
    std::cout << "  * m_allowRollBack=" << m_allowRollBack << "\n";
    std::cout << "  * m_noDBOutput=" << m_noDBOutput << "\n";
    std::cout << "  * m_onlyConfDef=" << m_onlyConfDef << "\n";
    std::cout << "  * m_printValues=" << m_printValues << "\n";
  }
}

popcon::GEMQC8ConfSourceHandler::~GEMQC8ConfSourceHandler()
{
}


void popcon::GEMQC8ConfSourceHandler::getNewObjects()
{
  //std::cout << "GEMQC8ConfSourceHandler getNewObjects" << std::endl;

  edm::LogInfo( "GEMQC8ConfSourceHandler" ) << "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
					    << "BEGIN m_onlyConfDef=" << m_onlyConfDef << std::endl;
  //std::cout << "m_noDBOutput=" << m_noDBOutput << std::endl;

  // first check what is already there in offline DB
  /* // needed if we want to avoid duplicated records
  if (!m_noDBOutput) {
    Ref payload;
    if(tagInfo().size>0) {
      edm::LogInfo( "GEMQC8ConfSourceHandler" )
	<< "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
	<< "Validation was requested, so will check present contents\n"
	<< "Destination Tag Info: name " << tagInfo().name
	<< ", size " << tagInfo().size
	<< ", last object valid since " << tagInfo().lastInterval.first
	<< ", hash " << tagInfo().lastPayloadToken << std::endl;
      payload = lastPayload();
    }
  }
  */

  qc8conf =  new GEMQC8Conf();
  qc8elMap = new GEMELMap();

  if (m_dummy==0) {
    std::cout << "\n\tcalling ConnectOnlineDB" << std::endl;
    ConnectOnlineDB( m_connect, m_connectionPset );
    readGEMQC8Conf();
    if (!m_onlyConfDef) readGEMQC8EMap();
    DisconnectOnlineDB();
  }
  else if (m_dummy==-1) {
    std::cout << "\n putting dummy data to match GEM INT2R\n\n";
    float flow=111. + 1e-3*m_runNumber;
    qc8conf->run_number_ = m_runNumber;
    qc8conf->chSerialNums_.push_back("GE1/1-VII-L-CERN-0002");
    qc8conf->chPositions_.push_back("1/1/T");
    qc8conf->chGasFlow_.push_back(flow);
    qc8conf->chSerialNums_.push_back("GE1/1-VII-L-CERN-0001");
    qc8conf->chPositions_.push_back("1/1/B");
    qc8conf->chGasFlow_.push_back(flow);
    qc8conf->chSerialNums_.push_back("GE1/1-VII-S-CERN-0002");
    qc8conf->chPositions_.push_back("1/2/T");
    qc8conf->chGasFlow_.push_back(flow);
    qc8conf->chSerialNums_.push_back("GE1/1-VII-S-CERN-0001");
    qc8conf->chPositions_.push_back("1/2/B");
    qc8conf->chGasFlow_.push_back(flow);
    ConnectOnlineDB( m_connect, m_connectionPset );
    std::cout << "m_onlyConfDef=" << m_onlyConfDef << std::endl;
    if (!m_onlyConfDef) {
      readGEMQC8EMap();
      qc8conf->hasELMap_=1;
    }
    DisconnectOnlineDB();
  }
  else {
    std::cout << "\n putting dummy data\n\n";
    qc8conf->run_number_ = m_runNumber;
    for (unsigned int i=0; i<15; i++) {
      qc8conf->chSerialNums_.push_back("L");
      std::stringstream ss;
      ss << "c" << (i/5+1) << "_r" << (i%5+1);
      qc8conf->chPositions_.push_back(ss.str());
      qc8conf->chGasFlow_.push_back(999.+1e-3*m_runNumber);
    }
  }

  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  if (mydbservice.isAvailable()) {
    cond::Time_t snc = mydbservice->currentTime();
    // don't look for recent changes
    int difference=1;
    if (difference==1) {
      std::cout << "GEMQC8ConfSourceHandler getNewObjects difference=1\n";
      m_to_transfer.push_back(std::make_pair((GEMQC8Conf*)qc8conf,snc));
      edm::LogInfo( "GEMQC8ConfSourceHandler" )
	<< "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
	<< "QC8Conf runNumber=" << (*qc8conf).run_number_
	<< ", sizes: " << (*qc8conf).chSerialNums_.size() << ", "
	<< (*qc8conf).chPositions_.size() << ", " << (*qc8conf).chGasFlow_.size()
	<< ", payloads to transfer: " << m_to_transfer.size() << std::endl;
    }
  }
  edm::LogInfo( "GEMQC8ConfSourceHandler" )
    << "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
    << "END." << std::endl;
}


// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMQC8ConfSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  //std::cout << "GEMQC8ConfSourceHandler ConnectOnlineDB\n";
  cond::persistency::ConnectionPool connection;
  edm::LogInfo( "GEMQC8ConfSourceHandler" ) << "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEMEMapConfigSourceHandler: connecting to " << connect << "..." << std::endl;
  connection.setParameters( connectionPset );
  //std::cout << "connectionPset = " << connectionPset << "\n";
  connection.configure();
  session = connection.createSession( connect,true );
  //session = connection.createSession( connect,false ); // not writeCapable
  edm::LogInfo( "GEMQC8ConfSourceHandler" ) << "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "Done." << std::endl;
  //std::cout << "GEMQC8ConfSourceHandler ConnectOnlineDB leaving" << std::endl;
}

void popcon::GEMQC8ConfSourceHandler::DisconnectOnlineDB()
{
  session.close();
}

void popcon::GEMQC8ConfSourceHandler::readGEMQC8Conf()
{
  //std::cout << "\nGEMQC8ConfSourceHandler readGEMQC8Conf" << std::endl;

  session.transaction().start( true );
  //std::cout << "transaction started" << std::endl;
  coral::ISchema& schema = session.nominalSchema();
  //std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;


  // if m_allowRollBack=1, we will decrease the run number, until we find a record
  int searchRun= m_runNumber;

  do {
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
      ssCond << "RUN_NUMBER = " << searchRun;
      condition = ssCond.str();
    }

    // get values
    query1->setCondition( condition, conditionData );
    coral::ICursor& cursor = query1->execute();
    std::cout<<"cursor OK"<<std::endl;
    qc8conf->run_number_ = searchRun;
    while ( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      try {
	int db_runnumber = row["RUN_NUMBER"].data<long long>();
	std::string db_chSerNum = row["CH_SERIAL_NUMBER"].data<std::string>();
	std::string db_chPos = row["POSITION"].data<std::string>();
	float db_flow = row["FLOW_METER"].data<float>();

	if (m_debugMode) db_flow+= 1e-3*searchRun;

	qc8conf->chSerialNums_.push_back(db_chSerNum);
	qc8conf->chPositions_.push_back(db_chPos);
	qc8conf->chGasFlow_.push_back(db_flow);

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

    // check if we have to loop
    searchRun--;
    if (m_allowRollBack && (qc8conf->chSerialNums_.size()==0)) {
      edm::LogInfo( "GEMQC8ConfSourceHandler" )
	<< "[" << "GEMQC8ConfSourceHandler::" << __func__ << "]:" << m_name << ": "
	<< "failed to find runNumber=" << (searchRun+1) << std::endl;
    }
  } while (!qc8conf->chSerialNums_.size() && m_allowRollBack && (searchRun>0));

  //std::cout << "GEMQC8ConfSourceHandler readGEMQC8Conf done" << std::endl;
}



void popcon::GEMQC8ConfSourceHandler::readGEMQC8EMap()
{
  std::cout << "\nGEMQC8ConfSourceHandler readGEMQC8EMap" << std::endl;

  if (!qc8conf || !qc8conf->chSerNums().size()) {
    std::cout << "readGEMQC8EMap is called for incorrect qc8conf\n";
    return;
  }
  if (!qc8elMap) { qc8elMap= new GEMELMap(); }

  session.transaction().start( true );
  //std::cout << "transaction started" << std::endl;
  coral::ISchema& schema = session.nominalSchema();
  //std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;

  for (unsigned int ich=0; ich<qc8conf->chSerNums().size(); ich++) {
    std::cout << " readGEMQC8EMap chamber " << qc8conf->chSerNum(ich) << "\n";

    coral::IQuery* query1 = schema.newQuery();
    query1->addToTableList( "GEM_SPRCHMBR_OPTHYB_VFATS_VIEW");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.SPER_CHMBR_SER_NUM", "SPER_CHMBR_SER_NUM");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.CHMBR_SER_NUM", "CHMBR_SER_NUM");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.DEPTH", "DEPTH");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.VFAT_ADDRESS", "VFAT_ADDRESS");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.VFAT_NAME", "VFAT_NAME");
    query1->addToOutputList("GEM_SPRCHMBR_OPTHYB_VFATS_VIEW.VFAT_POSN", "VFAT_POSN");

    coral::AttributeList conditionData; // empty
    std::string condition;
    {
      std::stringstream ssCond;
      ssCond << "CHMBR_SER_NUM = '" << qc8conf->chSerNum(ich) << "'";
      condition = ssCond.str();
    }

    // get values
    query1->setCondition( condition, conditionData );
    coral::ICursor& cursor = query1->execute();
    std::cout<<"cursor OK"<<std::endl;
    GEMELMap elmap;
    GEMELMap::GEMVFatMap vfats;
    vfats.VFATmapTypeId=-1;
    while ( cursor.next() ) {
      const coral::AttributeList& row = cursor.currentRow();
      try {
	std::string db_spChSerNum = row["SPER_CHMBR_SER_NUM"].data<std::string>();
	std::string db_chSerNum = row["CHMBR_SER_NUM"].data<std::string>();
	std::string db_depth = row["DEPTH"].data<std::string>();
	std::string db_vfatAddr = row["VFAT_ADDRESS"].data<std::string>();
	std::string db_vfatName = row["VFAT_NAME"].data<std::string>();
	std::string db_vfatPos = row["VFAT_POSN"].data<std::string>();

	int vfatPos=-1, dep=0, vfatType=-1;
	uint16_t vfatId=0;
	{
	  std::stringstream ssd(db_depth); ssd>>dep;
	  std::stringstream sspos(db_vfatPos); sspos>>vfatPos;
	  vfatType = (db_vfatName.find("VFAT2")!=std::string::npos) ? 2 : -1;
	  std::stringstream ssid(db_vfatAddr); ssid>>std::hex>>vfatId;
	}
	vfats.vfat_position.push_back(vfatPos);
	vfats.depth.push_back(dep);
	vfats.vfatType.push_back(vfatType);
	vfats.vfatId.push_back(vfatId);

	if (m_printValues) {
	  std::cout << "db: " << db_spChSerNum << ", " << db_chSerNum
		    << ", " << db_depth << ", vfatAddr=" << db_vfatAddr
		    << ", vfatName=" << db_vfatName
		    << ", vfatPos=" << db_vfatPos << "\n";
	  std::cout <<"    - converted " << dep << " 0x"
		    << std::hex << vfatId << std::dec << " "
		    << vfatType << " " << vfatPos << "\n";
	}


      }
      catch ( const std::exception & e ) {
	std::cout << "exception " << e.what() << " caught\n";
	//continue;
      }
    } // cursor.next

    // store the entry
    elmap.theVFatMap_.push_back(vfats);
    qc8conf->elMap_.push_back(elmap);
    if (vfats.size()) qc8elMap->theVFatMap_.push_back(vfats);

    // cleanup
    delete query1;
  }

  std::cout << "\nGEMQC8ConfSourceHandler readGEMQC8EMap done" << std::endl;
}
