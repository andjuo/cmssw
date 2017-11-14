import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("Write2DB")


from CondCore.CondDB.CondDB_cfi import *
#process.CondDB.DBParameters.messageLevel = cms.untracked.int32(3)


#sourceConnection = "oracle://cms_orcoff_prep/CMS_GEM_APPUSER_R"
sourceConnection = 'oracle://cms_omds_lb/CMS_RPC_CONF'
#sourceConnection = 'oracle://CMS_COND_GENERAL_R:p3105rof@cms_omds_adg/'
#sourceConnection = 'oracle://cms_omds_lb/CMS_COND_GENERAL_R'
#sourceConnection = 'oracle://cms_omds_adg/CMS_GEM_MUON_COND' # bad key: Service "cms_omds_adg" can't be open with the current key. from cond::CredentialStore::setUpConnection
#sourceConnection = 'oracle://CMS_GEM_APPUSER_R:GEM_Reader_2015@cms_omds_adg/CMS_GEM_MUON_COND' # Service "CMS_GEM_APPUSER_R" can't be open with the current key. from cond::CredentialStore::setUpConnection
#sourceConnection = 'oracle://CMS_GEM_APPUSER_R:GEM_Reader_2015@cms_omds_adg' # Service "CMS_GEM_APPUSER_R" can't be open with the current key. from cond::CredentialStore::setUpConnection
#sourceConnection = 'oracle://cms_omds_adg'
#sourceConnection = 'frontier://FrontierPrep/' # connects, but no tables
#sourceConnection = 'frontier://FrontierPrep/CMS_GEM_MUON_COND' # cannot get data
#sourceConnection = 'frontier://dev/CMS_GEM_MUON_COND'  # cannot get data
#sourceConnection = 'frontier://dev/' # does not work at all
#sourceConnection = 'oracle://CMS_COND_GENERAL_R:p3105rof@(DESCRIPTION=(ADDRESS= (PROTOCOL=TCP) (HOST=cmsonradg1-s.cern.ch)) (PORT=10121) )(LOAD_BALANCE=on)(ENABLE=BROKEN)(CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=cms_omds_adg.cern.ch)))'
#sourceConnection = 'oracle://CMS_GEM_APPUSER_R:GEM_Reader_2015@cms_orcoff_prep/' # service cannot be open with the current key
#sourceConnection = 'oracle://CMS_GEM_APPUSER_R:GEM_Reader_2015@cms_orcoff_prep/CMS_GEM_MUON_COND' # service cannot be open with the current key
#sourceConnection = 'oracle://cms_orcoff_prep/'  # no authorication: Connection to "oracle://cms_orcoff_prep/" with role "COND_READER_ROLE" is not available for COND_USERS_G from cond::RelationalAuthenticationService::RelationalAuthenticationService::credentials
sourceConnection = 'oracle://cms_orcoff_prep/CMS_GEM_MUON_COND'  # xml auth
#sourceConnection = 'oracle://cms_orcoff_prep/CMS_GEM_MUON_VIEW'  # no authorication: Connection to "oracle://cms_orcoff_prep/" with role "COND_READER_ROLE" is not available for COND_USERS_G from cond::RelationalAuthenticationService::RelationalAuthenticationService::credentials
#sourceConnection = 'oracle://cms_orcoff_prep/'  # no authorication: Connection to "oracle://cms_orcoff_prep/" with role "COND_READER_ROLE" is not available for COND_USERS_G from cond::RelationalAuthenticationService::RelationalAuthenticationService::credentials
sourceConnection = 'oracle://cms_omds_adg/CMS_GEM_MUON_COND'


options = VarParsing.VarParsing()
options.register( 'runNumber',
                  1, #default value
                  VarParsing.VarParsing.multiplicity.singleton,
                  VarParsing.VarParsing.varType.int,
                  "Run number to be uploaded." )
options.register( 'destinationConnection',
                  'sqlite_file:GEMEMap.db', #default value
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
                  'GEMEMap_v2',
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
#SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('./cond_db__cond/')
#SourceDBConnection.DBParameters.authenticationPath = cms.untracked.string('/nfshome0/popcondev/conddb')
#SourceDBConnection.DBParameters.onlineAuthentication = cms.untracked.string('/afs/cern.ch/cms/DB/conddb/ADG/.cms_cond/db.key'),
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
                             lastValue = cms.uint64( options.runNumber ),
                             timetype = cms.string( 'runnumber' ),
                             firstValue = cms.uint64( options.runNumber ),
                             interval = cms.uint64( 1 ) )

process.PoolDBOutputService = cms.Service( "PoolDBOutputService",
                                           CondDBConnection,
                                           timetype = cms.untracked.string( 'runnumber' ),
                                           toPut = cms.VPSet( cms.PSet( record = cms.string( 'GEMEMapRcd' ),
                                                                        tag = cms.string( options.tag ) ) ) )

process.WriteInDB = cms.EDAnalyzer( "GEMEMapDBWriter",
#process.WriteInDB = cms.EDAnalyzer( "GEMEMapDBReader",
                                    Validate = cms.untracked.int32( 0 ),

                                    SinceAppendMode = cms.bool( True ),
#                                    SinceAppendMode = cms.bool( False ),
                                    record = cms.string( 'GEMEMapRcd' ),
                                    loggingOn = cms.untracked.bool( False ),
                                    Source = cms.PSet( SourceDBConnection ,
#                                    DBParameters = cms.PSet(
#
                                    Validate = cms.untracked.int32( 0 ),
#                                                       authenticationPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb/'),
#                                                       OnlineAuthPath = cms.untracked.string('/afs/cern.ch/cms/DB/conddb/ADG/.cms_cond/db.key'),
#                                                       OnlineConn = cms.untracked.string('oracle://cms_omds_adg/CMS_COND_GENERAL_R') ) )
#                                                       OnlineConn = cms.untracked.string('oracle://cms_omds_adg/CMS_COND_GENERAL') ) )
                                                       #OnlineConn = cms.untracked.string('oracle://cms_omds_lb/CMS_GEM_MUON_VIEW') ) )
)
 )

process.p = cms.Path( process.WriteInDB )

