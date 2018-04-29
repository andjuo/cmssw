#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgSourceHandler.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgReadDBs.h"
//#include<iostream>
//#include<vector>

GEMPVSSFWCAENChannelIVRunAvgSourceHandler::GEMPVSSFWCAENChannelIVRunAvgSourceHandler( const edm::ParameterSet& pset ) :
   m_since( pset.getParameter<unsigned long long>( "runNumber" ) )
  ,m_name( pset.getUntrackedParameter<std::string>( "name", "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" ) )
  ,m_runinfo_schema( pset.getUntrackedParameter<std::string>( "RunInfoSchema", "CMS_RUNINFO" ) )
  ,m_pvss_schema( pset.getUntrackedParameter<std::string>( "GEMPVSSSchema", "CMS_GEM_PVSS_COND") )
  ,m_connectionString( pset.getParameter<std::string>( "connect" ) )
  ,m_connectionPset( pset.getParameter<edm::ParameterSet>( "DBParameters" ) ) {
}

GEMPVSSFWCAENChannelIVRunAvgSourceHandler::~GEMPVSSFWCAENChannelIVRunAvgSourceHandler() {}

void GEMPVSSFWCAENChannelIVRunAvgSourceHandler::getNewObjects() {
  //check whats already inside of database
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
      << m_name << ": "
      << "Destination Tag Info: name " << tagInfo().name
      << ", size " << tagInfo().size
      << ", last object valid since " << tagInfo().lastInterval.first
      << ", hash " << tagInfo().lastPayloadToken << std::endl;
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
    << m_name << ": runnumber/first since = " << m_since << std::endl;

  //check if a transfer is needed:
  //if the new run number is smaller than or equal to the latest IOV, exit.
  //This is needed as now the IOV Editor does not always protect for insertions:
  //ANY and VALIDATION sychronizations are allowed to write in the past.
  if( tagInfo().size > 0  && tagInfo().lastInterval.first >= m_since ) {
    edm::LogWarning( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
      << m_name << ": "
      << "last IOV " << tagInfo().lastInterval.first
      << ( tagInfo().lastInterval.first == m_since ? " is equal to" : " is larger than" )
      << " the run proposed for insertion " << m_since
      << ". No transfer needed." << std::endl;
    return;
  }

  //fill with null info if empty run are found beetween the two last valid ones
  std::ostringstream ss;
  /*
  size_t n_empty_run = 0;
  if( tagInfo().size > 0  && (tagInfo().lastInterval.first + 1) < m_since ) {
    n_empty_run = m_since - tagInfo().lastInterval.first - 1;
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
      << m_name << ": "
      << "entering fake run from "
      << tagInfo().lastInterval.first + 1
      <<  " to " << m_since - 1
      << std::endl;
  }

  // transfer fake run for 1 to since for the first time
  if( tagInfo().size == 0 && m_since != 1 ) {
    GEMPVSSFWCAENChannelIVRunAvg *emptyPtr= new GEMPVSSFWCAENChannelIVRunAvg();
    m_to_transfer.push_back( std::make_pair( emptyPtr, 1 ) ); BAD PAIR!
    ss << "empty data for run number: " << 1 << ", ";
  }
  if ( n_empty_run != 0 ) {
    GEMPVSSFWCAENChannelIVRunAvg *emptyPtr= new GEMPVSSFWCAENChannelIVRunAvg();
    m_to_transfer.push_back(std::make_pair( emptyPtr, tagInfo().lastInterval.first + 1 ) );
    ss << "empty data for run number: " << tagInfo().lastInterval.first + 1 << ", ";
  }
  */

  //reading from PVSS
  try {
    GEMPVSSFWCAENChannelIVRunAvg* ivRunAvg = new GEMPVSSFWCAENChannelIVRunAvg();
    GEMPVSSFWCAENChannelIVRunAvgReadDBs ivread( m_connectionString, m_connectionPset );
    *ivRunAvg = ivread.readData( m_runinfo_schema, m_pvss_schema, (int)m_since );

    // put the record into DB
    m_to_transfer.push_back( std::make_pair( (GEMPVSSFWCAENChannelIVRunAvg*)ivRunAvg, m_since) );
    ss << "run number: " << m_since << ";";
    m_userTextLog = ss.str();
  }
  catch (const std::exception &e) {
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
      << m_name << ": failed to get a payload for run_number=" << m_since << std::endl
      << "\tOriginal Exception: " << e.what() << std::endl;
  }

  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgSourceHandler" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgSourceHandler::" << __func__ << "]:"
    << m_name << ": END." << std::endl;
}
