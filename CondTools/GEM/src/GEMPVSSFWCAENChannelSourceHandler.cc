#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ITableDescription.h"
#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/IPrimaryKey.h"
#include "RelationalAccess/IIndex.h"
#include "RelationalAccess/IView.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include <TString.h>

#include <fstream>
#include <cstdlib>
#include <vector>

#include <sstream>

//#include <DataFormats/MuonDetId/interface/GEMDetId.h>

inline void HERE(std::string msg)
{ std::cout << msg << std::endl; }


popcon::GEMPVSSFWCAENChannelSourceHandler::GEMPVSSFWCAENChannelSourceHandler( const edm::ParameterSet& ps ):
  m_name( ps.getUntrackedParameter<std::string>( "name", "GEMPVSSFWCAENChannelSourceHandler" ) ),
  m_dummy( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  m_validate( ps.getUntrackedParameter<int>( "Validate", 1 ) ),
  m_sliceTest( ps.getUntrackedParameter<int>( "sliceTest", 0 ) ),
  m_listTables( ps.getUntrackedParameter<int>( "listTables", 0 ) ),
  m_connect( ps.getParameter<std::string>( "connect" ) ),
  m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) ),
  m_sinceTime( ps.getUntrackedParameter<long long>( "sinceTime", 20180101 ) ),
  m_untilTime( ps.getUntrackedParameter<long long>( "untilTime", -1 ) ),
  m_printValues( ps.getUntrackedParameter<bool>( "printValues", false ) )
{
  std::cout << "GEMPVSSFWCAENChannelSourceHandler constructor\n";
  std::cout << "  * m_name = " << m_name << "\n";
  std::cout << "  * m_dummy= " << m_dummy << "\n";
  std::cout << "  * m_validate= " << m_validate << "\n";
  std::cout << "  * m_sliceTest= " << m_sliceTest << "\n";
  std::cout << "  * m_listTables= " << m_listTables << "\n";
  std::cout << "  * m_connect= " << m_connect << "\n";
  std::cout << "  * DBParameters= " << m_connectionPset << "\n";
  std::cout << "  * m_sinceTime= " << m_sinceTime << "\n";
  std::cout << "  * m_untilTime= " << m_untilTime << "\n";
}

popcon::GEMPVSSFWCAENChannelSourceHandler::~GEMPVSSFWCAENChannelSourceHandler()
{
}


// Note: UTTtoT converter could be copied from CondTools/RPC/src/RPCFW.cc
coral::TimeStamp popcon::GEMPVSSFWCAENChannelSourceHandler::convertTimeStamp (long long dateInt, bool isStartTime)
{
  coral::TimeStamp tStamp;
  if (dateInt>20160101*10) {
    // this is probably a timestamp
    long nanosec= (isStartTime) ? 0 : 999999999;
    if (dateInt>static_cast<long long>(99999999999)) {
      // either microsecs or nanosecs included
      const std::string dateStr=std::to_string(dateInt);
      const long long factor=(dateStr.size()>16) ? 1000000000 : 1000000;
      nanosec=dateInt%factor * ((dateStr.size()>16) ? 1:1000);
      dateInt/=factor;
    }
    time_t rawTime=static_cast<time_t>(dateInt);
    tm * ptm = gmtime(&rawTime);
    tStamp= coral::TimeStamp(ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,
			     ptm->tm_hour,ptm->tm_min,ptm->tm_sec, nanosec,
			     false);
    //std::cout << "convertTimeStamp " << dateInt << ", " << tStamp.toString() << std::endl;
    //std::cout << "  year=" << tStamp.year() << ", month=" << tStamp.month() << ", day=" << tStamp.day() << "\n";
    //std::cout << "  - isStartTime=" << ((isStartTime) ? "yes":"no") << ", h=" << tStamp.hour() << ", m=" << tStamp.minute() << ", sec=" << tStamp.second() << ", nanosec=" << tStamp.nanosecond() << std::endl;
  }
  else {
    int year=dateInt/10000;
    int month=(dateInt%10000)/100;
    int day= dateInt%100;
    int hour= (isStartTime) ? 0 : 23;
    int min= (isStartTime) ? 0 : 59;
    int sec= (isStartTime) ? 0 :59;
    long nanosec= (isStartTime) ? 0 : 999999999;
    if (dateInt==-1) {
      time_t rawtime;
      time(&rawtime); //time since January 1, 1970
      tm * ptm = gmtime(&rawtime);//GMT time
      year = ptm->tm_year + 1900;
      month = ptm->tm_mon + 1;
      day = ptm->tm_mday;
    }
    //std::cout << "convertTimeStamp " << dateInt << " , year=" << year << ", month=" << month << ", day=" << day << "\n";
    //std::cout << "  - isStartTime=" << ((isStartTime) ? "yes":"no") << ", h=" << hour << ", m=" << min << ", sec=" << sec << ", nanosec=" << nanosec << std::endl;
    tStamp= coral::TimeStamp(year,month,day, hour,min,sec,nanosec, false);
  }
  std::cout << "convertTimeStamp " << dateInt << ", " << tStamp.toString() << std::endl;
  return tStamp;
}


void popcon::GEMPVSSFWCAENChannelSourceHandler::getNewObjects()
{
  std::cout << "GEMPVSSFWCAENChannelSourceHandler getNewObjects\n";

  edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" ) << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "BEGIN" << std::endl;

  edm::Service<cond::service::PoolDBOutputService> mydbservice;

  // first check what is already there in offline DB
  Ref payload;
  if(m_validate==1 && tagInfo().size>0) {
    edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" ) << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
                                           << "Validation was requested, so will check present contents\n"
                                           << "Destination Tag Info: name " << tagInfo().name
                                           << ", size " << tagInfo().size
                                           << ", last object valid since " << tagInfo().lastInterval.first
                                           << ", hash " << tagInfo().lastPayloadToken << std::endl;
    payload = lastPayload();
  }

  // now construct new cabling map from online DB
  // FIXME: use boost::ptime
  time_t rawtime;
  time(&rawtime); //time since January 1, 1970
  tm * ptm = gmtime(&rawtime);//GMT time
  char buffer[20];
  strftime(buffer,20,"%d/%m/%Y_%H:%M:%S",ptm);
  std::string pvssInfo_version( buffer );
  edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" ) << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEM PVSSInfo version: " << pvssInfo_version << std::endl;
  fwCh =  new GEMPVSSFWCAENChannel(pvssInfo_version);

  if (m_dummy==0) {
    std::cout << "\n\tcalling ConnectOnlineDB" << std::endl;
    ConnectOnlineDB( m_connect, m_connectionPset );
    if (m_listTables) {
      listDBTablesInSchema();
      std::cout << "listTables=1. code operates to list tables and views. exciting..." << std::endl;
      return;
    }
    else {
      readGEMPVSSFWCAENChannel();
    }
    DisconnectOnlineDB();
  }
  else {
    std::cout << "\n putting dummy data\n\n";
    for (int i=0; i<5; i++)
      fwCh->gem_ObImon_.push_back(GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_IMonParam>(i+10, 20180324, 1500, 0.2*(i+1)));
    for (int v=0; v<6; v++)
      fwCh->gem_ObVmon_.push_back(GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_VMonParam>(v+20, 20180324, 1501, 0.2*(v+2)));
  }


  cond::Time_t snc = mydbservice->currentTime();
  // look for recent changes
  int difference=1;
  if (difference==1) {
    std::cout << "GEMPVSSFWCAENChannelSourceHandler getNewObjects difference=1\n";
    m_to_transfer.push_back(std::make_pair((GEMPVSSFWCAENChannel*)fwCh,snc));
    edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" )
      << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
      << "FWCAENChannel size: " << (*fwCh).gem_ObImon_.size() << " and " << (*fwCh).gem_ObVmon_.size()
      << ", payloads to transfer: " << m_to_transfer.size() << std::endl;
  }
  edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" )
    << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
    << "END." << std::endl;
}


// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMPVSSFWCAENChannelSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  //std::cout << "GEMPVSSFWCAENChannelSourceHandler ConnectOnlineDB\n";
  cond::persistency::ConnectionPool connection;
  edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" ) << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEMEMapConfigSourceHandler: connecting to " << connect << "..." << std::endl;
  connection.setParameters( connectionPset );
  //std::cout << "connectionPset = " << connectionPset << "\n";
  connection.configure();
  session = connection.createSession( connect,true );
  //session = connection.createSession( connect,false ); // not writeCapable
  edm::LogInfo( "GEMPVSSFWCAENChannelSourceHandler" ) << "[" << "GEMPVSSFWCAENChannelSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "Done." << std::endl;
  //std::cout << "GEMPVSSFWCAENChannelSourceHandler ConnectOnlineDB leaving" << std::endl;
}

void popcon::GEMPVSSFWCAENChannelSourceHandler::DisconnectOnlineDB()
{
  session.close();
}

void popcon::GEMPVSSFWCAENChannelSourceHandler::listDBTablesInSchema()
{
  std::cout << "\nGEMPVSSFWCAENChannelSourceHandler listDBTablesInSchema"<< std::endl;

  session.transaction().start( true );
  std::cout << "transaction started" << std::endl;
  coral::ISchema& schema = session.nominalSchema();
  std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;

  if (schema.schemaName().size()) {
    std::set<std::string> tables= schema.listTables();
    std::set<std::string> views= schema.listViews();
    std::cout << "\n\ttables.size=" << tables.size() << ", views.size=" << views.size() << "\n" << std::endl;

    if (tables.size()) {
      std::cout << "\ntables:\n";
      int idx=0;
      for (std::set<std::string>::const_iterator it= tables.begin();
	   it!=tables.end(); it++, idx++) {
	std::cout << "\ntable idx=" << idx << ": " << *it << "\n";
	//HERE("get table handle");
	const coral::ITable* tableHandlePtr= & schema.tableHandle(*it);
	if (!tableHandlePtr) {
	  std::cout << "failed to get tableHandlePtr\n";
	  return;
	}
	//HERE("get table description");
	const coral::ITableDescription* d = & tableHandlePtr->description();
	if (!d) {
	  std::cout << "failed to get tableDescriptionPtr\n";
	  return;
	}
	//const coral::ITableDescription* d= tableDescrPtr;
	//HERE("list contents");
	try {
	  std::cout << "name=" << d->name() << ", type=" << d->type() << ", numberOfColumns=" << d->numberOfColumns() << ", hasPrimaryKey=" << d->hasPrimaryKey() << ", numberOfIndices=" << d->numberOfIndices() << ", numberOfForeignKeys=" << d->numberOfForeignKeys() << ", numberOfUniqueConstraints=" << d->numberOfUniqueConstraints() << "\n";
	  if (d->hasPrimaryKey()) {
	    const coral::IPrimaryKey *pkey= & d->primaryKey();
	    if (!pkey) {
	      std::cout << "failed to get primary key\n";
	      return;
	    }
	    const std::vector<std::string> & colNames= pkey->columnNames();
	    std::cout << " ** PrimaryKey" << " tableSpaceName=" << pkey->tableSpaceName() << " has " << colNames.size() << " columnNames: ";
	    for (unsigned int icol=0; icol<colNames.size(); icol++) {
		std::cout << " " << colNames[icol];
	    }
	    std::cout << "\n";
	  }
	  if (d->numberOfIndices()) {
	    if (d->numberOfIndices()>1) std::cout << " ** Several indices! (listing)\n";
	    for (int ii=0; ii<d->numberOfIndices(); ii++) {
	      const  coral::IIndex *iidx= & d->index(ii);
	      if (!iidx) {
		std::cout << "failed to get index at ii=" << ii << "\n";
		return;
	      }
	      std::cout << " ** Index @" << ii << ". name=" << iidx->name() << ", isUnique=" << iidx->isUnique() << ", tableSpaceName=" << iidx->tableSpaceName() << "\n";
	      const std::vector<std::string> & colNames=iidx->columnNames();
	      std::cout << "  has " << colNames.size() << " columns: ";
	      for (unsigned int icol=0; icol<colNames.size(); icol++) {
		std::cout << " " << colNames[icol];
	      }
	      std::cout << "\n";
	    }
	  }
	  if (d->numberOfColumns()) {
	    //HERE("list columns");
	    std::cout << " -- Columns: \n";
	    for (int iCol=0; iCol<d->numberOfColumns(); iCol++) {
	      const coral::IColumn* col= & d->columnDescription(iCol);
	      std::cout << "iCol=" << iCol << ", name=" << col->name() << ", C++ type=" << col->type() << ", indexInTable=" << col->indexInTable() << ", isNotNull=" << col->isNotNull() << ", isUnique=" << col->isUnique() << ", size=" << col->size() << ", isSizeFixed=" << col->isSizeFixed() << "\n";
	    }
	  }
	}
	catch ( const std::exception & e ) {
	  std::cout << "exception " << e.what() << " caught\n";
	  continue;
	}
      }
      std::cout << std::endl;
    }
    if (views.size()) {
      std::cout << "\nviews:\n";
      int idx=0;
      for (std::set<std::string>::const_iterator it= views.begin();
	   it!=views.end(); it++, idx++) {
	std::cout << "\nviews idx=" << idx << ": " << *it << "\n";
	try {
	const coral::IView *viewH= & schema.viewHandle(*it);
	if (!viewH) {
	  std::cout << "failed to get view\n";
	  return;
	}
	std::cout << " ** has " << viewH->numberOfColumns() << " columns\n";
	    if (idx!=4)
	if (viewH->numberOfColumns()) {
	  //HERE("list view columns");
	  std::cout << " -- Columns: \n";
	  for (int iCol=0; iCol<viewH->numberOfColumns(); iCol++) {
	    const coral::IColumn* col= & viewH->column(iCol);
	    std::cout << "iCol=" << iCol << ", name=" << col->name();
	    std::cout << " C++ type=" << col->type();
	    std::cout << ", indexInTable=" << col->indexInTable() << ", isNotNull=" << col->isNotNull() << ", isUnique=" << col->isUnique() << ", size=" << col->size() << ", isSizeFixed=" << col->isSizeFixed() << "\n";
	  }
	}
	}
	catch ( const std::exception &e ) {
	  std::cout << "exception " << e.what() << " caught\n";
	}
      }
      std::cout << std::endl;
    }

  }

  std::cout << std::endl <<"GEMPVSSFWCAENChannelSourceHandler:: listDBTablesInSchema DONE" << std::endl << std::endl;
}


void popcon::GEMPVSSFWCAENChannelSourceHandler::readGEMPVSSFWCAENChannel()
{
  std::cout << "\nGEMPVSSFWCAENChannelSourceHandler readGEMPVSSFWCAENChannel" << std::endl;

  session.transaction().start( true );
  std::cout << "transaction started" << std::endl;
  coral::ISchema& schema = session.nominalSchema();
  std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;

  std::cout << std::endl <<"GEMPVSSFWCAENChannelSourceHandler: start to build GEMPVSSFWCAENChannel..." << std::flush << std::endl << std::endl;
  std::cout << " * Time frame " << m_sinceTime << " - " << m_untilTime << "\n";
  coral::TimeStamp timeMin = GEMPVSSFWCAENChannelSourceHandler::convertTimeStamp(m_sinceTime, true);
  coral::TimeStamp timeMax = GEMPVSSFWCAENChannelSourceHandler::convertTimeStamp(m_untilTime, false);


  coral::IQuery* query1 = schema.newQuery();
  query1->addToTableList( "FWCAENCHANNELA1515");
  query1->addToOutputList("FWCAENCHANNELA1515.UPDATEID", "UPDATEID");
  query1->addToOutputList("FWCAENCHANNELA1515.DPID", "DPID");
  query1->addToOutputList("FWCAENCHANNELA1515.CHANGE_DATE", "CHANGE_DATE");
  query1->addToOutputList("FWCAENCHANNELA1515.DPE_STATUS", "DPE_STATUS");
  query1->addToOutputList("FWCAENCHANNELA1515.DPE_POSITION", "DPE_POSITION");
  query1->addToOutputList("FWCAENCHANNELA1515.ACTUAL_IMON", "ACTUAL_IMON");
  query1->addToOutputList("FWCAENCHANNELA1515.ACTUAL_VMON", "ACTUAL_VMON");
  query1->addToOutputList("FWCAENCHANNELA1515.ACTUAL_STATUS", "ACTUAL_STATUS");

  coral::AttributeList conditionData;
  conditionData.extend<coral::TimeStamp>( "timeMin" );
  conditionData.extend<coral::TimeStamp>( "timeMax" );
  conditionData["timeMin"].data<coral::TimeStamp>() = timeMin;
  conditionData["timeMax"].data<coral::TimeStamp>() = timeMax;

  std::string conditionTime=  "FWCAENCHANNELA1515.CHANGE_DATE >:timeMin AND FWCAENCHANNELA1515.CHANGE_DATE <: timeMax";
  std::string conditionImon= "FWCAENCHANNELA1515.ACTUAL_IMON IS NOT NULL AND " + conditionTime;
  std::string conditionVmon= "FWCAENCHANNELA1515.ACTUAL_VMON IS NOT NULL AND " + conditionTime;

  // IMon values
  query1->setCondition( conditionImon, conditionData );
  coral::ICursor& cursorImon = query1->execute();
  std::cout<<"cursor Imon OK"<<std::endl;
  while ( cursorImon.next() ) {
    const coral::AttributeList& row = cursorImon.currentRow();
    GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_IMonParam> d;
    float dpid_float= row["DPID"].data<float>();
    d.dpid=static_cast<int>(dpid_float);
    coral::TimeStamp changeDate= row["CHANGE_DATE"].data<coral::TimeStamp>();
    d.setDate(changeDate); // set d.date and d.time
    d.value= row["ACTUAL_IMON"].data<float>();

    long long updateId=-1;
    float dpeStatus=-1, dpePosition=-1, actualStatus=-1, vmon=-1;
    try {
      updateId= row["UPDATEID"].data<long long>();
      dpeStatus= row["DPE_STATUS"].data<float>();
      dpePosition= row["DPE_POSITION"].data<float>();
    }
    catch ( const std::exception & e ) {
      std::cout << "exception " << e.what() << " caught\n";
      //continue;
    }

    try { vmon= row["ACTUAL_VMON"].data<float>(); }
    catch ( const std::exception & e ) {
      //std::cout << "exception " << e.what() << " caught\n";
    }

    try { actualStatus= row["ACTUAL_STATUS"].data<float>(); }
    catch ( const std::exception & e ) {
      //std::cout << "exception " << e.what() << " caught\n";
    }

    if (m_printValues) {
      std::cout << "iMon data " << d << "\n";
      std::cout << "  otherInfo: updateId=" << updateId << ", dpeStatus=" << dpeStatus << ", dpePosition=" << dpePosition << ", actualStatus=" << actualStatus << ", vmon=" << vmon << "\n";
      std::cout << std::endl;
    }

    fwCh->gem_ObImon_.push_back(d);
  }

  delete query1;

  // VMon data

  coral::IQuery* query2 = schema.newQuery();
  query2->addToTableList( "FWCAENCHANNELA1515");
  query2->addToOutputList("FWCAENCHANNELA1515.UPDATEID", "UPDATEID");
  query2->addToOutputList("FWCAENCHANNELA1515.DPID", "DPID");
  query2->addToOutputList("FWCAENCHANNELA1515.CHANGE_DATE", "CHANGE_DATE");
  query2->addToOutputList("FWCAENCHANNELA1515.DPE_STATUS", "DPE_STATUS");
  query2->addToOutputList("FWCAENCHANNELA1515.DPE_POSITION", "DPE_POSITION");
  query2->addToOutputList("FWCAENCHANNELA1515.ACTUAL_IMON", "ACTUAL_IMON");
  query2->addToOutputList("FWCAENCHANNELA1515.ACTUAL_VMON", "ACTUAL_VMON");
  query2->addToOutputList("FWCAENCHANNELA1515.ACTUAL_STATUS", "ACTUAL_STATUS");

  query2->setCondition( conditionVmon, conditionData );
  coral::ICursor& cursorVmon = query2->execute();
  std::cout<<"cursor Vmon OK"<<std::endl;
  while ( cursorVmon.next() ) {
    const coral::AttributeList& row = cursorVmon.currentRow();
    GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_VMonParam> d;
    float dpid_float= row["DPID"].data<float>();
    d.dpid=static_cast<int>(dpid_float);
    coral::TimeStamp changeDate= row["CHANGE_DATE"].data<coral::TimeStamp>();
    d.setDate(changeDate); // set d.date and d.time
    d.value= row["ACTUAL_VMON"].data<float>();

    long long updateId=-1;
    float dpeStatus=-1, dpePosition=-1, actualStatus=-1, imon=-1;
    try {
      updateId= row["UPDATEID"].data<long long>();
      dpeStatus= row["DPE_STATUS"].data<float>();
      dpePosition= row["DPE_POSITION"].data<float>();
    }
    catch ( const std::exception & e ) {
      std::cout << "exception " << e.what() << " caught\n";
      //continue;
    }

    try { imon= row["ACTUAL_IMON"].data<float>(); }
    catch ( const std::exception & e ) {
      //std::cout << "exception " << e.what() << " caught\n";
    }

    try { actualStatus= row["ACTUAL_STATUS"].data<float>(); }
    catch ( const std::exception & e ) {
      //std::cout << "exception " << e.what() << " caught\n";
    }

    if (m_printValues) {
      std::cout << "vMon data " << d << "\n";
      std::cout << "  otherInfo: updateId=" << updateId << ", dpeStatus=" << dpeStatus << ", dpePosition=" << dpePosition << ", actualStatus=" << actualStatus << ", imon=" << imon << "\n";
      std::cout << std::endl;
    }

    fwCh->gem_ObVmon_.push_back(d);
  }

  delete query2;

  std::cout << "GEMPVSSFWCAENChannelSourceHandler readGEMPVSSFWCAENChannel done" << std::endl;

}
