import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("Write2DB")


from CondCore.CondDB.CondDB_cfi import *
#process.CondDB.DBParameters.messageLevel = cms.untracked.int32(3)

#sourceConnection = 'oracle://cms_omds_adg/CMS_GEM_MUON_COND'
sourceConnection = 'oracle://cms_omds_adg/CMS_GEM_PVSS_COND'


options = VarParsing.VarParsing()
options.register( 'runNumber',
                  1, #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Run number to be uploaded." )
options.register( 'numberOfRuns',
                  1, #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Run number to be uploaded." )
options.register( 'destinationConnection',
                  'sqlite_file:GEM-PVSS.db', #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "Connection string to the DB where payloads will be possibly written." )
options.register( 'targetConnection',
                  '', #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  """Connection string to the target DB:
                     if not empty (default), this provides the latest IOV and payloads to compare;
                     it is the DB where payloads should be finally uploaded.""" )
options.register( 'tag',
                  'GEMPVSSFWCAENChannel_v1',
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.string,
                  "Tag written in destinationConnection and finally appended in targetConnection." )
options.register( 'messageLevel',
                  0, #default value # 3 is veryverbose
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Message level; default to 0" )
options.parseArguments()

CondDBConnection = CondDB.clone( connect = cms.string( options.destinationConnection ) )
CondDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )

SourceDBConnection = CondDB.clone( connect = cms.string( sourceConnection ) )
SourceDBConnection.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )
#SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb')
SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('.')
#SourceDBConnection.DBParameters.onlineAuthentication = cms.untracked.string('/afs/cern.ch/cms/DB/conddb/ADG/.cms_cond/db.key'),
#SourceDBConnection.DBParameters.onlineAuthentication = cms.untracked.string('/afs/cern.ch/cms/DB/conddb/ADG/')
SourceDBConnection.DBParameters.authenticationSystem = cms.untracked.int32(2)


from pprint import pprint
pprint(vars(SourceDBConnection))

#process.load("CondCore.DBCommon.CondDBCommon_cfi")
process.load("CondCore.CondDB.CondDB_cfi")
process.MessageLogger = cms.Service( "MessageLogger",
                                     destinations = cms.untracked.vstring( 'cout' ),
                                     cout = cms.untracked.PSet( #default = cms.untracked.PSet( limit = cms.untracked.int32( 0 ) ),
                                                                threshold = cms.untracked.string( 'INFO' ) ) )

if options.messageLevel == 3:
    #enable LogDebug output: remember the USER_CXXFLAGS="-DEDM_ML_DEBUG" compilation flag!
    process.MessageLogger.cout = cms.untracked.PSet( threshold = cms.untracked.string( 'DEBUG' ) )
    process.MessageLogger.debugModules = cms.untracked.vstring( '*' )

process.source = cms.Source( "EmptyIOVSource",
                             timetype = cms.string( 'runnumber' ),
                             firstValue = cms.uint64( options.runNumber ),
                             lastValue = cms.uint64( options.runNumber + options.numberOfRuns ),
                             interval = cms.uint64( 1 ) )

process.PoolDBOutputService = cms.Service( "PoolDBOutputService",
                                           CondDBConnection,
                                           timetype = cms.untracked.string( 'runnumber' ),
                                           toPut = cms.VPSet( cms.PSet( record = cms.string( 'GEMPVSSFWCAENChannelRcd' ),
                                                                        tag = cms.string( options.tag ) ) ) )


process.WriteInDB = cms.EDAnalyzer("GEMPVSSFWCAENChannelDBWriter",
    Validate = cms.untracked.int32( 0 ),
    SinceAppendMode = cms.bool( True ),
    #SinceAppendMode = cms.bool( False ),
    record = cms.string( 'GEMPVSSFWCAENChannelRcd' ),
    loggingOn = cms.untracked.bool( False ),
    Source = cms.PSet( SourceDBConnection,
                       WriteDummy = cms.untracked.int32(0),#fakeData for testing
                       Validate = cms.untracked.int32( 0 ),
#                       sinceTime = cms.untracked.int64( 1521986075 ), # 20180325 ),
                       sinceTime = cms.untracked.int64( 20180407 ),
                       untilTime = cms.untracked.int64 (20180409 ),
                       #untilTime = cms.untracked.int64( 1522070221 ), # 20180326
                       #untilTime = cms.untracked.int64( 1844674407370955 ), # 20280615 9:33:27.371
                       printValues = cms.untracked.bool( False ), # whether to print obtained values
                       listTables = cms.untracked.int32(0) #listAllTables and their fields
                   )
)

process.p = cms.Path( process.WriteInDB )

