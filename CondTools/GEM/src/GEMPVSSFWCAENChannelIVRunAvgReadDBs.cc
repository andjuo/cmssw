#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgReadDBs.h"
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

std::ostream& operator<< (std::ostream &out, const coral::TimeStamp &cts)
{
  out << "-->year " << cts.year()
      << "-- month " << cts.month()
      << "-- day " << cts.day()
      << "-- hour " << cts.hour()
      << "-- minute " << cts.minute()
      << "-- second " << cts.second()
      << "-- nanosecond " << cts.nanosecond();
  return out;
}


GEMPVSSFWCAENChannelIVRunAvgReadDBs::GEMPVSSFWCAENChannelIVRunAvgReadDBs( const std::string& connectionString,
	  const edm::ParameterSet& connectionPset ):
  m_connectionString( connectionString ),
  m_connectionPset( connectionPset )
{}

GEMPVSSFWCAENChannelIVRunAvgReadDBs::~GEMPVSSFWCAENChannelIVRunAvgReadDBs() {}

GEMPVSSFWCAENChannelIVRunAvg
GEMPVSSFWCAENChannelIVRunAvgReadDBs::readData(const std::string & runinfo_schema
					      , const std::string & pvss_schema
					      , const int r_number )
{
  //RunInfo temp_sum;
  GEMPVSSFWCAENChannelIVRunAvg temp_ivRunAvg;

  // First, fill runTime information
  GEMPVSSFWCAENChannelIVRunAvg::RunTime temp_runTime;

  //from TimeConversions.h
  const boost::posix_time::ptime time0 = boost::posix_time::from_time_t(0);

  //if cursor is null setting null values
  //temp_sum.m_run = r_number;
  temp_runTime.run = r_number;
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Initialising Connection Pool" << std::endl;
  cond::persistency::ConnectionPool connection;
  connection.setParameters( m_connectionPset );
  connection.configure();
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Initialising read-only session to " << m_connectionString << std::endl;
  std::shared_ptr<coral::ISessionProxy> session = connection.createCoralSession( m_connectionString, false );
  try{
    session->transaction().start( true );
    coral::ISchema& schema = session->schema( runinfo_schema );
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Accessing schema " << runinfo_schema << std::endl;
    //new query to obtain the start_time
    std::unique_ptr<coral::IQuery> query( schema.newQuery() );
    query->addToTableList( "RUNSESSION_PARAMETER" );
    query->addToTableList( "RUNSESSION_DATE" );
    query->addToOutputList( "VALUE" );
    coral::AttributeList runTimeDataOutput;
    runTimeDataOutput.extend<coral::TimeStamp>( "VALUE" );
    query->defineOutput( runTimeDataOutput );
    std::string runStartWhereClause( "RUNNUMBER=:n_run AND NAME='CMS.LVL0:START_TIME_T' AND ID=RUNSESSION_PARAMETER_ID" );
    coral::AttributeList runNumberBindVariableList;
    runNumberBindVariableList.extend<int>( "n_run" );
    runNumberBindVariableList[ "n_run" ].data<int>() = r_number;
    query->setCondition( runStartWhereClause, runNumberBindVariableList );
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader") << " calling query->execute()" << std::endl;
    coral::ICursor& runStartCursor = query->execute();
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader") << " calling query->execute() done" << std::endl;
    coral::TimeStamp ctsStart; //now all times are UTC!
    if( runStartCursor.next() ) { // only 1 value expected
      //std::cout << "runStartCursor.next() is ok" << std::endl;
      std::ostringstream osstartdebug;
      runStartCursor.currentRow().toOutputStream( osstartdebug );
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << osstartdebug.str() << std::endl;
      const coral::AttributeList& row = runStartCursor.currentRow();
      ctsStart = row[ "VALUE" ].data<coral::TimeStamp>();
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "UTC start time extracted == " << ctsStart << std::endl;
      temp_runTime.setStartTime(ctsStart.time());
      std::ostringstream osstart;
      temp_runTime.printStartTime(osstart);
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgRead" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Timestamp for start of " << osstart.str() << std::endl;
    }
    else {
      //std::cout << "forming error msg" << std::endl;
      std::stringstream errMsg;
      errMsg << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: run " << r_number << " start time not found.";
      throw std::runtime_error(errMsg.str());
    }


    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader") << " working on new query" << std::endl;

    //new query to obtain the stop_time
    query.reset( schema.newQuery() );
    query->addToTableList( "RUNSESSION_PARAMETER" );
    query->addToTableList( "RUNSESSION_DATE" );
    query->addToOutputList( "VALUE" );
    query->defineOutput( runTimeDataOutput );
    std::string runStopWhereClause = "RUNNUMBER=:n_run AND NAME='CMS.LVL0:STOP_TIME_T' AND ID=RUNSESSION_PARAMETER_ID";
    query->setCondition( runStopWhereClause, runNumberBindVariableList );
    coral::ICursor& runStopCursor = query->execute();
    coral::TimeStamp ctsStop;
    if( runStopCursor.next() ) { // only 1 value expected
      std::ostringstream osstopdebug;
      runStopCursor.currentRow().toOutputStream( osstopdebug );
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << osstopdebug.str() << std::endl;
      const coral::AttributeList& row = runStopCursor.currentRow();
      ctsStop = row[ "VALUE" ].data<coral::TimeStamp>();
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "stop time extracted == " << ctsStop << std::endl;
      temp_runTime.setEndTime(ctsStop.time());
      std::ostringstream osstop;
      temp_runTime.printEndTime(osstop);
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Timestamp for stop of " << osstop.str() << std::endl;
    }
    else {
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: run " << r_number
                                      << " stop time not found." << std::endl;
      temp_runTime.setEndTime(time0);
    }

    //new query for obtaining the list of FEDs included in the run
    /* // works but is not used here
    query.reset( schema.newQuery() );
    query->addToTableList( "RUNSESSION_PARAMETER" );
    query->addToTableList( "RUNSESSION_STRING" );
    query->addToOutputList( "VALUE" );
    query->defineOutputType( "VALUE", "string" );
    std::string fedWhereClause(  "RUNNUMBER=:n_run AND NAME='CMS.LVL0:FED_ENABLE_MASK' AND ID=RUNSESSION_PARAMETER_ID" );
    query->setCondition( fedWhereClause, runNumberBindVariableList );
    coral::ICursor& fedCursor = query->execute();
    std::string fed;
    if ( fedCursor.next() ) {
      std::ostringstream osfeddebug;
      fedCursor.currentRow().toOutputStream( osfeddebug );
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgRead" ) << osfeddebug.str() << std::endl;
      const coral::AttributeList& row = fedCursor.currentRow();
      fed = row[ "VALUE" ].data<std::string>();
    }
    else {
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgRead" )
	<< "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: run " << r_number
	<< "has no FED included." << std::endl;
      fed="null";
    }
    std::replace(fed.begin(), fed.end(), '%', ' ');
    std::stringstream stream(fed);
    std::vector<int> loc_fed_in;
    for(;;) {
      std::string word;
      if ( !(stream >> word) ){break;}
      std::replace(word.begin(), word.end(), '&', ' ');
      std::stringstream ss(word);
      int fedNumber;
      int val;
      ss >> fedNumber >> val;
      LogDebug( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "FED: " << fedNumber << " --> value: " << val << std::endl;
      //val bit 0 represents the status of the SLINK, but 5 and 7 means the SLINK/TTS is ON but NA or BROKEN (see mail of alex....)
      if( (val & 0001) == 1 && (val != 5) && (val != 7) )
	//temp_sum.m_fed_in.push_back(fedNumber);
	loc_fed_in.push_back(fedNumber);
    }
    std::ostringstream osfed;
    osfed << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: feds included in run " << r_number << ": ";
    //std::copy(temp_sum.m_fed_in.begin(), temp_sum.m_fed_in.end(), std::ostream_iterator<int>(osfed, ", "));
    std::copy(loc_fed_in.begin(), loc_fed_in.end(), std::ostream_iterator<int>(osfed, ", "));
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << osfed.str() << std::endl;
    */

    // Got runtime information
    temp_ivRunAvg.runTime_ = temp_runTime;

    // Prepare time window for the PVSS data
    coral::AttributeList conditionData;
    conditionData.extend<coral::TimeStamp>( "run_start_time" );
    conditionData.extend<coral::TimeStamp>( "run_stop_time" );
    conditionData["run_start_time"].data<coral::TimeStamp>() = ctsStart;
    conditionData["run_stop_time"].data<coral::TimeStamp>() = ctsStop;

    std::string conditionTime=  "FWCAENCHANNELA1515.CHANGE_DATE >:run_start_time AND FWCAENCHANNELA1515.CHANGE_DATE <: run_stop_time";
    std::string conditionImon= "FWCAENCHANNELA1515.ACTUAL_IMON IS NOT NULL AND " + conditionTime;
    std::string conditionVmon= "FWCAENCHANNELA1515.ACTUAL_VMON IS NOT NULL AND " + conditionTime;

    //we connect now to the PVSS schema in order to retrieve the current and voltage values
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: Accessing schema " << pvss_schema << std::endl;

    coral::ISchema& schema2 = session->schema( pvss_schema );

    //++ prepare Imon values
    query.reset( schema2.tableHandle( "FWCAENCHANNELA1515" ).newQuery() );
    //query->addToOutputList("FWCAENCHANNELA1515.UPDATEID", "UPDATEID");
    query->addToOutputList("FWCAENCHANNELA1515.DPID", "DPID");
    query->addToOutputList("FWCAENCHANNELA1515.CHANGE_DATE", "CHANGE_DATE");
    //query->addToOutputList("FWCAENCHANNELA1515.DPE_STATUS", "DPE_STATUS");
    //query->addToOutputList("FWCAENCHANNELA1515.DPE_POSITION", "DPE_POSITION");
    query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_IMON", "ACTUAL_IMON");
    //query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_VMON", "ACTUAL_VMON");
    //query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_STATUS", "ACTUAL_STATUS");
    query->addToOrderList("FWCAENCHANNELA1515.DPID ASC");
    query->addToOrderList("FWCAENCHANNELA1515.CHANGE_DATE ASC"); // DESC?
    query->limitReturnedRows( 10000 );
    // limit time span
    query->setCondition( conditionImon, conditionData );

    //std::cout << "try execute query imon" << std::endl;
    coral::ICursor& cursorImon = query->execute();
    //std::cout<<"cursor Imon OK"<<std::endl;

    //if ( !cursorImon.next() ) { this consumes 1 row
    //  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: cursorImon returned no value" << std::endl;
    //}

    std::ostringstream ostr_debug;
    while ( cursorImon.next() ) {
      const coral::AttributeList& row = cursorImon.currentRow();
      GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_IMonParam> d;
      float dpid_float= row["DPID"].data<float>();
      d.dpid=static_cast<int>(dpid_float);
      coral::TimeStamp changeDate= row["CHANGE_DATE"].data<coral::TimeStamp>();
      d.setDate(changeDate); // set d.date and d.time
      d.value= row["ACTUAL_IMON"].data<float>();

      if (0) {
	ostr_debug << "read " << d << "\n";
      }
      temp_ivRunAvg.monData_.gem_ObImon_.push_back(d);
    }

    if (ostr_debug.str().size())
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "Imon raw data\n" << ostr_debug.str() << std::endl;

    //++ Prepare for Vmon values
    query.reset( schema2.tableHandle( "FWCAENCHANNELA1515" ).newQuery() );
    //query->addToOutputList("FWCAENCHANNELA1515.UPDATEID", "UPDATEID");
    query->addToOutputList("FWCAENCHANNELA1515.DPID", "DPID");
    query->addToOutputList("FWCAENCHANNELA1515.CHANGE_DATE", "CHANGE_DATE");
    //query->addToOutputList("FWCAENCHANNELA1515.DPE_STATUS", "DPE_STATUS");
    //query->addToOutputList("FWCAENCHANNELA1515.DPE_POSITION", "DPE_POSITION");
    //query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_IMON", "ACTUAL_IMON");
    query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_VMON", "ACTUAL_VMON");
    //query->addToOutputList("FWCAENCHANNELA1515.ACTUAL_STATUS", "ACTUAL_STATUS");
    query->addToOrderList("FWCAENCHANNELA1515.DPID ASC");
    query->addToOrderList("FWCAENCHANNELA1515.CHANGE_DATE  ASC"); // DESC?
    query->limitReturnedRows( 10000 );
    // set time span
    query->setCondition( conditionVmon, conditionData );

    //std::cout << "try execute query vmon" << std::endl;
    coral::ICursor& cursorVmon = query->execute();
    //std::cout<<"cursor Vmon OK"<<std::endl;

    //if ( !cursorVmon.next() ) { This reads 1st value!
    //  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: cursorVmon returned no value" << std::endl;
    //}

    ostr_debug.clear();
    while ( cursorVmon.next() ) {
      const coral::AttributeList& row = cursorVmon.currentRow();
      GEMPVSSFWCAENChannel::GEMMonItem<GEMPVSSFWCAENChannel::GEM_VMonParam> d;
      float dpid_float= row["DPID"].data<float>();
      d.dpid=static_cast<int>(dpid_float);
      coral::TimeStamp changeDate= row["CHANGE_DATE"].data<coral::TimeStamp>();
      d.setDate(changeDate); // set d.date and d.time
      d.value= row["ACTUAL_VMON"].data<float>();

      if (0) {
	ostr_debug << "read " << d << "\n";
      }
      temp_ivRunAvg.monData_.gem_ObVmon_.push_back(d);
    }

    if (ostr_debug.str().size())
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "Vmon raw data\n" << ostr_debug.str() << std::endl;

    // calculate averages
    if (!temp_ivRunAvg.calcStat()) {
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: calcStat returned error" << std::endl;
    }

    session->transaction().commit();
  }
  catch (const std::exception& e) {
    throw cms::Exception( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: "
                                            << "Unable to create a payload. Original Exception:\n"
                                            << e.what() << std::endl;
  }

  int storeFlag= __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg;
  if (0) { // detailed print
    __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg=0;
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: loaded " << temp_ivRunAvg << std::endl;
    __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg= storeFlag;
  }
  else {
    __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg=2; // sizes only
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgReader" ) << "[GEMPVSSFWCAENChannelIVRunAvgRead::" << __func__ << "]: tried to read run=" << r_number << ", got " << temp_ivRunAvg.runTime_.run << std::endl;
    __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg= storeFlag;
  }

  return temp_ivRunAvg;
}
