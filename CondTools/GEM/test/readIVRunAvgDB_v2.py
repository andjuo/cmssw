import time
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from CondCore.CondDB.CondDB_cfi import *

options = VarParsing.VarParsing()
options.register('connectionString',
                 'sqlite_file:GEM_IVRunAvgPopCon_test.db', #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Connection string")
#options.register('tag',
#                 'GEMPVSSFWCAENChannelIVRunAvg_v1', #default value
#                 VarParsing.VarParsing.multiplicity.singleton,
#                 VarParsing.VarParsing.varType.string,
#                 "Tag")
options.register('runNumber',
                 314324, #default value, int limit -3
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Run number; default gives latest IOV")
#options.register('numberOfRuns',
#                 1, #default value
#                 VarParsing.VarParsing.multiplicity.singleton,
#                 VarParsing.VarParsing.varType.int,
#                 "number of runs in the job")
options.register('messageLevel',
                 0, #default value
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Message level; default to 0")
options.parseArguments()

CondDBReference = CondDB.clone( connect = cms.string( options.connectionString ) )
CondDBReference.DBParameters.messageLevel = cms.untracked.int32( options.messageLevel )

process = cms.Process( "DBTest" )

process.MessageLogger = cms.Service( "MessageLogger",
                                     destinations = cms.untracked.vstring( 'cout' ),
                                     cout = cms.untracked.PSet( threshold = cms.untracked.string( 'INFO' ) ),
                                     )

process.source = cms.Source("EmptyIOVSource",
                            timetype = cms.string('runnumber'),
                            firstValue = cms.uint64(options.runNumber),
                            lastValue = cms.uint64(options.runNumber),
                            interval = cms.uint64(1)
                        )

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( options.numberOfRuns ) ) #options.numberOfRuns runs per job

process.GEMPVSSFWCAENChannelESSource = cms.ESSource( "PoolDBESSource",
                                   CondDBReference,
                                                     timetype = cms.string('runnumber'),
                                   toGet = cms.VPSet( cms.PSet( record = cms.string('GEMPVSSFWCAENChannelIVRunAvgRcd'),
                                                                tag = cms.string('GEMPVSSIVRunAvg_v1')
                                                                )
                                                      ),
                                   )

process.reader = cms.EDAnalyzer( "GEMPVSSFWCAENChannelIVRunAvgDBReader",
                                 dumpFileName = cms.untracked.string( "dumpGEMPVSSIVRunAvg.out")
 )

process.recordDataGetter = cms.EDAnalyzer( "EventSetupRecordDataGetter",
                                           toGet =  cms.VPSet(),
                                           verbose = cms.untracked.bool( True )
                                           )

process.escontent = cms.EDAnalyzer( "PrintEventSetupContent",
                                    compact = cms.untracked.bool( True ),
                                    printProviders = cms.untracked.bool( True )
                                    )
process.esretrieval = cms.EDAnalyzer( "PrintEventSetupDataRetrieval",
                                      printProviders = cms.untracked.bool( True )
                                      )

#Path definition
process.GEMPVSSFWCAENChannelIVRunAvgReaderSourcePath = cms.Path( process.reader + process.recordDataGetter )
process.esout = cms.EndPath( process.escontent + process.esretrieval )

#Schedule definition
process.schedule = cms.Schedule( process.GEMPVSSFWCAENChannelIVRunAvgReaderSourcePath,
                                 process.esout
                                 )

for name, module in process.es_sources_().iteritems():
    print "ESModules> provider:%s '%s'" % ( name, module.type_() )
for name, module in process.es_producers_().iteritems():
    print "ESModules> provider:%s '%s'" % ( name, module.type_() )
