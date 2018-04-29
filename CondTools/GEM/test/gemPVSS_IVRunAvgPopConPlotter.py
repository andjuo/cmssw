import socket
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from CondCore.CondDB.CondDB_cfi import *

#sourceConnection = 'oracle://cms_omds_adg/CMS_RUNINFO'
sourceConnection = 'oracle://cms_omds_adg/CMS_RUNINFO_R'
if socket.getfqdn().find('.cms') != -1:
    sourceConnection = 'oracle://cms_omds_lb/CMS_RUNINFO'

options = VarParsing.VarParsing()
options.register( 'fromRun'
                , 314324 #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.int
                , "Run number to be uploaded."
                  )
options.register( 'toRun'
                , 314324 #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.int
                , "Run number to be uploaded."
                  )
options.register( 'numberOfRuns'
                , 0 #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.int
                , "Number of runs to be uploaded."
                  )
options.register( 'destinationConnection'
                , 'sqlite_file:GEM_IVRunAvgPopCon_plottest.db' #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.string
                , "Connection string to the DB where payloads will be possibly written."
                  )
options.register( 'targetConnection'
                , '' #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.string
                , """Connection string to the target DB:
                     if not empty (default), this provides the latest IOV and payloads to compare;
                     it is the DB where payloads should be finally uploaded."""
                  )
options.register( 'tag'
                , 'GEMPVSSIVRunAvg_v1'
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.string
                , "Tag written in destinationConnection and finally appended in targetConnection."
                  )
options.register( 'messageLevel'
                , 0 #default value
                , VarParsing.VarParsing.multiplicity.singleton
                , VarParsing.VarParsing.varType.int
                , "Message level; default to 0"
                  )
options.parseArguments()

if options.numberOfRuns > 0 :
    options.toRun = options.fromRun + options.numberOfRuns

CondDBConnection = CondDB.clone( connect = cms.string( options.destinationConnection ) )
CondDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )

OMDSDBConnection = CondDB.clone( connect = cms.string( sourceConnection ) )
OMDSDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )
# added by aj
OMDSDBConnection.DBParameters.authenticationSystem = cms.untracked.int32(2)

process = cms.Process( "GEMPVSSIVRunAvgPlotter" )

process.MessageLogger = cms.Service( "MessageLogger"
                                   , destinations = cms.untracked.vstring( 'cout' )
                                   , cout = cms.untracked.PSet( threshold = cms.untracked.string( 'INFO' ) )
                                     )

if options.messageLevel == 3:
    #enable LogDebug output: remember the USER_CXXFLAGS="-DEDM_ML_DEBUG" compilation flag!
    process.MessageLogger.cout = cms.untracked.PSet( threshold = cms.untracked.string( 'DEBUG' ) )
    process.MessageLogger.debugModules = cms.untracked.vstring( '*' )

process.source = cms.Source( "EmptyIOVSource"
                           , timetype = cms.string( 'runnumber' )
                           , firstValue = cms.uint64( options.fromRun )
                           , lastValue = cms.uint64( options.fromRun )
                           , interval = cms.uint64( 1 )
                             )

#process.source = cms.Source( "EmptySource"
#                           , timetype = cms.string( 'runnumber' )
#                           , firstRun = cms.untracked.uint32( options.runNumber )
                             #, lastRun = cms.untracked.uint32( options.runNumber + options.numberOfRuns )
                           #, interval = cms.untracked.uint32( 1 )
#                             )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 1 ) #options.numberOfRuns )
)

process.PoolDBOutputService = cms.Service( "PoolDBOutputService"
                                         , CondDBConnection
                                         , timetype = cms.untracked.string( 'runnumber' )
                                         , toPut = cms.VPSet( cms.PSet( record = cms.string( 'GEMPVSSFWCAENChannelIVRunAvgRcd' )
                                                                      , tag = cms.string( options.tag )
                                                                        )
                                                              )
                                          )

print "from run ",options.fromRun," to run ",options.toRun

process.popConGEMIVRunAvg = cms.EDAnalyzer( "GEMPVSSFWCAENChannelIVRunAvgPopConPlotter"
                                      , SinceAppendMode = cms.bool( True )
                                      , record = cms.string( 'GEMPVSSFWCAENChannelIVRunAvgRcd' )
                                      , Source = cms.PSet( OMDSDBConnection
                                                         , fromRunNumber = cms.uint64( options.fromRun )
                                                           , toRunNumber = cms.uint64( options.toRun )
                                                           , outHistoFile = cms.untracked.string("outHistos.root")
                                                           , savePVSShistos = cms.untracked.int32( 1 )
                                                           , saveToDB = cms.untracked.int32( 0 )
                                                           )
                                      , loggingOn = cms.untracked.bool( True )
                                      , targetDBConnectionString = cms.untracked.string( options.targetConnection )
                                        )

process.p = cms.Path( process.popConGEMIVRunAvg )
