#include "CondTools/GEM/interface/GEMEMapSourceHandler.h"
#include "CondCore/CondDB/interface/ConnectionPool.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
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

#include <DataFormats/MuonDetId/interface/GEMDetId.h>

popcon::GEMEMapSourceHandler::GEMEMapSourceHandler( const edm::ParameterSet& ps ):
  m_name( ps.getUntrackedParameter<std::string>( "name", "GEMEMapSourceHandler" ) ),
  m_dummy( ps.getUntrackedParameter<int>( "WriteDummy", 0 ) ),
  m_validate( ps.getUntrackedParameter<int>( "Validate", 1 ) ),
  m_connect( ps.getParameter<std::string>( "connect" ) ),
  m_connectionPset( ps.getParameter<edm::ParameterSet>( "DBParameters" ) )
{
  //std::cout << "m_dummy=" << m_dummy << "\n";
}

popcon::GEMEMapSourceHandler::~GEMEMapSourceHandler()
{
}

void popcon::GEMEMapSourceHandler::getNewObjects()
{
  
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "BEGIN" << std::endl;
  
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  
  // first check what is already there in offline DB
  Ref payload;
  if(m_validate==1 && tagInfo().size>0) {
    edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
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
  std::string eMap_version( buffer );
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEM eMap version: " << eMap_version << std::endl;
  eMap =  new GEMEMap(eMap_version);
  if (m_dummy==0) {
    ConnectOnlineDB( m_connect, m_connectionPset );
    readGEMEMap();
    DisconnectOnlineDB();
  }
  else {
    edm::LogInfo( "GEMEMapSourceHandler") << "[GEMEMapSourceHandler::" <<
      __func__ << "]: " << m_dummy << std::endl;

    // use edm::FileInPath
    //TODO: data files go in a separate git repo, if needed
    std::string baseCMS = std::string(getenv("CMSSW_BASE"))+std::string("/src/CondTools/GEM/data/");
    std::vector<std::string> mapfiles;

    //TString WhichConf = "CMSGE1/1";

    mapfiles.push_back("vfat_position.csv");

    for (unsigned int ifm=0;ifm<mapfiles.size();ifm++){
      GEMEMap::GEMVFatMaptype vmtype;
      std::string filename(baseCMS+mapfiles[ifm]);
      edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
					     <<"Opening CSV file "<< filename << std::endl;
      vmtype.VFATmapTypeId=ifm+1;//this is 1 and 2 if there are two input files
      std::ifstream maptype(filename.c_str());
      //std::string buf("");

      std::string line, tmp_sec;
      while(std::getline(maptype, line)){
	//mapping v1:      VFAT_POSN	Z	IETA	IPHI	DEPTH	Detector Strip Number	VFAT channel Number	Px Connector Pin #
	//mapping v2:             SUBDET   SECTOR	         TYPE	ZPOSN	IETA   IPHI   DEPTH   VFAT_POSN	  DET_STRIP   VFAT_CHAN   CONN_PIN
	//mapping CS (7 Nov 2016) SUBDET   TSCOL   TSROW   TYPE   ZPOSN   IETA   IPHI   DEPTH   VFAT_POSN   DET_STRIP   VFAT_CHAN   CONN_PIN
      
	int vfat_pos, z_dir, ieta, iphi, dep, str_num, vfat_chn_num, sec;
	uint16_t vfat_add;
	char c;
	// First 2 input lines:
	// GE1M01,-1,7,2,1,9,127,128,0xfef4
	// GE1M01,-1,1,1,2,7,127,128,0xfe2c
	tmp_sec=line.substr(0,6);
	std::stringstream ssline(line.substr(6,line.size()-6));
	ssline >> c >> z_dir >> c >> ieta >> c >> iphi >> c >> dep
	       >> c >> vfat_pos >> c >> str_num >> c >> vfat_chn_num >> c
	       >> std::hex >> vfat_add >> std::dec;
	sec= atoi(tmp_sec.c_str()+4);
     
	LogDebug( "GEMMapSourceHandler" ) << ", z_direction="<< z_dir
					  << ", ieta="<< ieta
					  << ", iphi="<< iphi
					  << ", depth="<< dep
					  << ", vfat position="<< vfat_pos
					  << ", strip no.=" << str_num
					  << ", vfat channel no.="<< vfat_chn_num
					  << std::endl;

	//std::cout<<" Sector="<<sec<<" z_direction="<<z_dir<<" ieta="<<ieta<<" iphi="<<iphi<<" depth="<<dep<<" vfat position="<<vfat_pos<<" strip no.="<<str_num<<" vfat channel no.="<<vfat_chn_num<<" vfat address = " << std::hex << "0x" << vfat_add << std::dec <<std::endl;
	//GEMDetId id(z_dir, 1, 1, dep, sec, ieta);
	//std::cout  << id.rawId() << std::endl;
	vmtype.vfat_position.push_back(vfat_pos);
	vmtype.z_direction.push_back(z_dir);
	vmtype.iEta.push_back(ieta);
	vmtype.iPhi.push_back(iphi);
	vmtype.depth.push_back(dep);
	vmtype.strip_number.push_back(str_num);
	vmtype.vfat_chnnel_number.push_back(vfat_chn_num);
	vmtype.vfatId.push_back(vfat_add);
	vmtype.sec.push_back(sec);
      }
      vmtype.VFATmapTypeId=1;
      eMap->theVFatMaptype.push_back(vmtype); 
    }

    if (eMap->theVFatMaptype.size())
      std::cout << "eMap size: " <<eMap->theVFatMaptype[0].iPhi.size() << "\n";
  } // dummy map

  cond::Time_t snc = mydbservice->currentTime();  
  // look for recent changes
  int difference=1;
  if (difference==1) {
    m_to_transfer.push_back(std::make_pair((GEMEMap*)eMap,snc));
    edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                           << "Emap size: " << eMap->theVFatMaptype.size()
                                           << ", payloads to transfer: " << m_to_transfer.size() << std::endl;
  }
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "END." << std::endl;
}

// // additional work (I added these two functions: ConnectOnlineDB and DisconnectOnlineDB)
void popcon::GEMEMapSourceHandler::ConnectOnlineDB( const std::string& connect, const edm::ParameterSet& connectionPset )
{
  cond::persistency::ConnectionPool connection;
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "GEMEMapConfigSourceHandler: connecting to " << connect << "..." << std::endl;
  connection.setParameters( connectionPset );
  connection.configure();
  //session = connection.createSession( connect,true );
  session = connection.createSession( connect,false ); // not writeCapable
  edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << m_name << ": "
                                         << "Done." << std::endl;
}

void popcon::GEMEMapSourceHandler::DisconnectOnlineDB()
{
  session.close();
}

void popcon::GEMEMapSourceHandler::readGEMEMap()
{


  session.transaction().start( true );
  coral::ISchema& schema = session.nominalSchema();

  /* // AndJuo: auxiliary info for a developer
  std::cout << "got schema with name <" << schema.schemaName() << ">" << std::endl;
  if (schema.schemaName().size()) {
    std::set<std::string> tables= schema.listTables();
    std::set<std::string> views= schema.listViews();
    std::cout << "\n\ttables.size=" << tables.size() << ", views.size=" << views.size() << "\n" << std::endl;

    if (tables.size()) {
      std::cout << "tables:\n";
      int idx=0;
      for (std::set<std::string>::const_iterator it= tables.begin();
	   it!=tables.end(); it++, idx++) {
	std::cout << "table idx=" << idx << ": " << *it << "\n";
      }
      std::cout << std::endl;
    }
    if (views.size()) {
      std::cout << "views:\n";
      int idx=0;
      for (std::set<std::string>::const_iterator it= views.begin();
	   it!=views.end(); it++, idx++) {
	std::cout << "views idx=" << idx << ": " << *it << "\n";
      }
      std::cout << std::endl;
    }
  }
  */

  std::string condition="";
  coral::AttributeList conditionData;

  std::cout << std::endl <<"GEMEMapSourceHandler: start to build GEM e-Map..." << std::flush << std::endl << std::endl;

  coral::IQuery* query1 = schema.newQuery();
  /*select c.SECTOR, c.ZPOSN,c.IETA,c.IPHI,c.DEPTH,c.VFAT_POSN,c.DET_STRIP,c.VFAT_CHAN, b.VFAT_ADDRESS  from gem_omds.gem_vfat_channels c inner join  gem_omds.gem_sprchmbr_opthyb_vfats_v b on c.vfat_posn = b.vfat_posn and c.sector= b.sector and c.depth = b.depth*/

  /* // AndJuo: old code lines
  query1->addToTableList( "CMS_GEM_MUON_COND.gem_vfat_channels" );
  query1->addToTableList( "CMS_GEM_MUON_VIEW.gem_sprchmbr_opthyb_vfats_view" );
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.SECTOR", "SECTOR");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.ZPOSN", "ZPOSN");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.IETA", "IETA");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.IPHI", "IPHI");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.DEPTH", "DEPTH");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.VFAT_POSN", "VFAT_POSN");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.DET_STRIP", "DET_STRIP");
  query1->addToOutputList("CMS_GEM_MUON_COND.gem_vfat_channels.VFAT_CHAN", "VFAT_CHAN");
  query1->addToOutputList("CMS_GEM_MUON_VIEW.gem_sprchmbr_opthyb_vfats_view.VFAT_ADDRESS", "VFAT_ADDRESS");
  condition = "CMS_GEM_MUON_COND.gem_vfat_channels.IETA>0";
  }
   */

  // updated code lines
  query1->addToTableList( "GEM_VFAT_CHANNELS" );
  query1->addToTableList( "GEM_SUPRCHMBR_VFAT_VIEW_RH" );
  query1->addToOutputList("GEM_VFAT_CHANNELS.SECTOR", "SECTOR");
  query1->addToOutputList("GEM_VFAT_CHANNELS.ZPOSN", "ZPOSN");
  query1->addToOutputList("GEM_VFAT_CHANNELS.IETA", "IETA");
  query1->addToOutputList("GEM_VFAT_CHANNELS.IPHI", "IPHI");
  query1->addToOutputList("GEM_VFAT_CHANNELS.DEPTH", "DEPTH");
  query1->addToOutputList("GEM_VFAT_CHANNELS.VFAT_POSN", "VFAT_POSN");
  query1->addToOutputList("GEM_VFAT_CHANNELS.DET_STRIP", "DET_STRIP");
  query1->addToOutputList("GEM_VFAT_CHANNELS.VFAT_CHAN", "VFAT_CHAN");
  query1->addToOutputList("GEM_SUPRCHMBR_VFAT_VIEW_RH.VFAT_ADDRESS", "VFAT_ADDRESS");

  // require overlap in the table info for a correct match
  condition =  "GEM_VFAT_CHANNELS.VFAT_POSN=GEM_SUPRCHMBR_VFAT_VIEW_RH.VFAT_POSN ";
  condition += " AND GEM_VFAT_CHANNELS.SECTOR=GEM_SUPRCHMBR_VFAT_VIEW_RH.SECTOR";
  condition += " AND GEM_VFAT_CHANNELS.DEPTH=GEM_SUPRCHMBR_VFAT_VIEW_RH.CHMBR_DEPTH";

  // additional restrict for debug purposes
  //std::cout << "\n\t debug restriction: iPhi=1 and VFAT_POSN<2, and iEta>0\n";
  //condition += " AND GEM_VFAT_CHANNELS.IPHI=1 AND GEM_VFAT_CHANNELS.VFAT_POSN<2" ; // AND GEM_VFAT_CHANNELS.IETA=2";

  // select only one side of the channels
  //condition += " AND GEM_VFAT_CHANNELS.IETA>0";

  query1->setCondition( condition, conditionData );


  coral::ICursor& cursor1 = query1->execute();
  //std::cout<<"OK"<<std::endl;
  GEMEMap::GEMVFatMaptype vmtype;
  std::pair<int,int> tmp_tbl;
  std::vector< std::pair<int,int> > theDAQ;
  while ( cursor1.next() ) {
    const coral::AttributeList& row = cursor1.currentRow();
    //vmtype.vfat_position.push_back( row["VFAT_POSN"].data<int>()  );
    vmtype.vfat_position.push_back( row["VFAT_POSN"].data<short int>()  );
    //vmtype.z_direction.push_back( row["ZPOSN"].data<int>() );
    vmtype.z_direction.push_back( row["ZPOSN"].data<short int>() );
    //vmtype.iEta.push_back( row["IETA"].data<int>() );
    vmtype.iEta.push_back( row["IETA"].data<short int>() );
    //vmtype.iPhi.push_back( row["IPHI"].data<short int>() );
    vmtype.iPhi.push_back( row["IPHI"].data<short int>() );
    //vmtype.depth.push_back( row["DEPTH"].data<int>()  );
    vmtype.depth.push_back( row["DEPTH"].data<short int>()  );
    vmtype.strip_number.push_back( row["DET_STRIP"].data<int>() );
    vmtype.vfat_chnnel_number.push_back( row["VFAT_CHAN"].data<int>()  );

    //vmtype.vfatId.push_back( row["VFAT_ADDRESS"].data<uint16_t>()  );
    std::string vfat_addr_str= row["VFAT_ADDRESS"].data<std::string>();
    uint16_t addr= (vfat_addr_str.size()) ? static_cast<uint16_t>(strtol(vfat_addr_str.c_str(),NULL,16)) : -9999;
    std::cout << "vfat_addr_str=" << vfat_addr_str << ", addr=" << std::hex << "0x" << addr << std::dec << "\n";
    vmtype.vfatId.push_back(addr);

    std::string a = row["SECTOR"].data<std::string>();
    int sector= atoi(a.c_str()+4);
    std::cout <<" a  "<< a << ", sector=" << sector << std::endl;
    vmtype.sec.push_back(sector);

    //std::cout << "vmtype size =" << vmtype.iEta.size() << " : "; vmtype.printLast(std::cout); std::cout << "\n";
     edm::LogInfo( "GEMEMapSourceHandler" ) << "[" << "GEMEMapSourceHandler::" << __func__ << "]:" << " check last : " << vmtype << std::endl;
  }
  vmtype.VFATmapTypeId=1;
  if (!vmtype.isConsistent()) std::cout << "\nvmtype is not consistent" << std::endl;
  eMap->theVFatMaptype.push_back(vmtype); 


  if (condition.find("GEM_VFAT_CHANNELS.IPHI=")!=condition.npos) {
    std::cout << "\n\t NOTE: debug restriction was applied" << std::endl;
  }

  std::cout << "eMap size: " << eMap->theVFatMaptype[0].iPhi.size() << "\n";

  delete query1;

}
