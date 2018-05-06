#include "CondTools/GEM/interface/GEMListTablesSourceHandler.h"
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

#include <boost/date_time/posix_time/posix_time.hpp>

#include <fstream>
#include <cstdlib>
#include <vector>

#include <sstream>

//#include <DataFormats/MuonDetId/interface/GEMDetId.h>

inline void HERE(std::string msg)
{ std::cout << msg << std::endl; }


popcon::GEMListTablesSourceHandler::GEMListTablesSourceHandler( const edm::ParameterSet& ps ):
  m_connect( ps.getParameter<std::string>( "connect" ) )
  , m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) )
{
  std::cout << "GEMListTablesSourceHandler constructor\n";
  std::cout << "  * m_connect= " << m_connect << "\n";
  std::cout << "  * DBParameters= " << m_connectionPset << "\n";
}

popcon::GEMListTablesSourceHandler::~GEMListTablesSourceHandler()
{
}


void popcon::GEMListTablesSourceHandler::getNewObjects()
{
  GEMPVSSFWCAENChannel ch;
  std::cout << "GEMListTablesSourceHandler getNewObjects" << std::endl;
  listDBTablesInSchema();
  std::cout << "GEMListTablesSourceHandler getNewObjects done" << std::endl;
}


void popcon::GEMListTablesSourceHandler::listDBTablesInSchema()
{
  edm::LogInfo( "GEMListTablesSourceHandler" ) << "[" << "GEMListTablesSourceHandler::" << __func__ << "]: " << "listDBTablesInSchema BEGIN" << std::endl;

  //std::cout << "GEMListTablesSourceHandler ConnectOnlineDB\n";
  cond::persistency::ConnectionPool connection;
  edm::LogInfo( "GEMListTablesSourceHandler" ) << "[" << "GEMListTablesSourceHandler::" << __func__ << "]: " << "GEMEMapConfigSourceHandler: connecting to " << m_connect << "..." << std::endl;
  connection.setParameters( m_connectionPset );
  //std::cout << "connectionPset = " << connectionPset << "\n";
  connection.configure();
  cond::persistency::Session session = connection.createSession( m_connect,false );
  //session = connection.createSession( connect,false ); // false -- not writeCapable
  std::cout << "GEMListTablesSourceHandler ConnectOnlineDB leaving" << std::endl;

  std::cout << "\nGEMListTablesSourceHandler listDBTablesInSchema"<< std::endl;

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

  // close the session
  session.close();


  std::cout << std::endl <<"GEMListTablesSourceHandler:: listDBTablesInSchema DONE" << std::endl << std::endl;

}

