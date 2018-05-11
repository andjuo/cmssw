#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgPlotter.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelIVRunAvgReadDBs.h"
#include "CondTools/GEM/interface/GEMPVSSFWCAENChannelReadDPIDs.h"
#include <TH1F.h>
#include <TFile.h>
#include <TObjString.h>
#include <TDatime.h>
//#include<iostream>
//#include<vector>
#include <boost/algorithm/string/replace.hpp>


TDatime convert2Datime(const boost::posix_time::ptime &ts, int gmt=0)
{
  std::string str= boost::posix_time::to_iso_extended_string(ts);
  str.replace(str.find('T'),1," ");
  if (gmt) str += " GMT";
  TDatime dt(str.c_str());
  return dt;
}


GEMPVSSFWCAENChannelIVRunAvgPlotter::GEMPVSSFWCAENChannelIVRunAvgPlotter( const edm::ParameterSet& pset ) :
  m_fromRun( pset.getParameter<unsigned long long>( "fromRunNumber" ) )
  ,m_toRun( pset.getParameter<unsigned long long>( "toRunNumber" ) )
  ,m_name( pset.getUntrackedParameter<std::string>( "name", "GEMPVSSFWCAENChannelIVRunAvgPlotter" ) )
  ,m_outHistoFile( pset.getUntrackedParameter<std::string>( "outHistoFile", "outHistos.root" ) )
  ,m_savePVSShistos( pset.getUntrackedParameter<int>( "savePVSShistos", 0 ) )
  ,m_saveToDB( pset.getUntrackedParameter<int>( "saveToDB", 0 ) )
  ,m_runinfo_schema( pset.getUntrackedParameter<std::string>( "RunInfoSchema", "CMS_RUNINFO" ) )
  ,m_pvss_schema( pset.getUntrackedParameter<std::string>( "GEMPVSSSchema", "CMS_GEM_PVSS_COND") )
  ,m_alias_connectionString( pset.getUntrackedParameter<std::string>( "GEMDPIDAliasConnection", "oracle://INT2R/CMS_GEM_PVSS_TEST_COND") )
  ,m_connectionString( pset.getParameter<std::string>( "connect" ) )
  ,m_connectionPset( pset.getParameter<edm::ParameterSet>( "DBParameters" ) )
  ,m_h1runAvgV()
  ,m_h1trendsV()
  ,m_dpidAliases()
{
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
      << m_name << ": runRange " << m_fromRun << " - " << m_toRun
      << ", outHistoFile=<" << m_outHistoFile << ">\n" << std::endl;

  if (m_outHistoFile.size()) fill_dpidAliases();
}

GEMPVSSFWCAENChannelIVRunAvgPlotter::~GEMPVSSFWCAENChannelIVRunAvgPlotter() {}


void GEMPVSSFWCAENChannelIVRunAvgPlotter::fill_dpidAliases()
{
  const int from_string=0;
  if (from_string) {
    const std::string aliases=
      std::string("48      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G3bot ")+
      std::string("49      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G3top ")+
      std::string("50      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G2bot ")+
      std::string("51      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G2top ")+
      std::string("52      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G1bot ")+
      std::string("53      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/G1top ")+
      std::string("54      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/1/Drift ")+
      std::string("55      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G3bot ")+
      std::string("56      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G3top ")+
      std::string("57      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G2bot ")+
      std::string("58      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G2top ")+
      std::string("59      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G1bot ")+
      std::string("60      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/G1top ")+
      std::string("61      CMS_GEM_Main/HVchan/Multi/GE-1/1/01/2/Drift ");

    std::stringstream ss(aliases);
    int dpid;
    std::string alias;
    while (!ss.eof()) {
      ss >> dpid >> alias;
      boost::replace_all(alias, "CMS_GEM_Main/HVchan/Multi/", "HV-");
      //boost::replace_all(alias, "GE-1/1/01/", "GE-1/1-");
      //boost::replace_all(alias, "GE1/1/01/", "GE1/1-");
      m_dpidAliases[dpid]= alias;
    }
  }
  else {
    GEMPVSSFWCAENChannelReadDPIDs dpidReader( m_alias_connectionString, m_connectionPset );
    //GEMPVSSFWCAENChannelReadDPIDs dpidReader( "oracle://INT2R/CMS_GEM_PVSS_TEST_COND", m_connectionPset );
    GEMPVSSFWCAENChannelReadDPIDs::TDPIDAliasMap aliasMap;
    try {
      //aliasMap = dpidReader.readData( "CMS_GEM_PVSS_TEST_COND" );
      size_t pos = m_alias_connectionString.find_last_of('/');
      std::string schemaName(m_alias_connectionString.substr(pos+1, m_alias_connectionString.size()));
      aliasMap = dpidReader.readData( schemaName );
    }
    catch (const std::exception &e) {
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
	<< "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
	<< "got error\n"
	<< "\tOriginal Exception: " << e.what() << std::endl;
      throw std::runtime_error("dpidReader failed");
    }

    // keep only HVchan aliases
    for ( auto it : aliasMap ) {
      if (it.second.find("Main/HVchan")!=std::string::npos) {
	std::string alias= it.second;
	boost::replace_all(alias, "CMS_GEM_Main/HVchan/Multi/", "HV-");
	//boost::replace_all(alias, "GE-1/1/01/", "GE-1/1-");
	//boost::replace_all(alias, "GE1/1/01/", "GE1/1-");
	m_dpidAliases[ it.first ] = alias;
	std::cout << " keep " << it.first << "\t" << it.second << "; made alias=<" << alias << ">\n";
      }
    }
  }
}

int GEMPVSSFWCAENChannelIVRunAvgPlotter::createHistos(int dpid)
{
  if (m_h1runAvgV.find(dpid)!=m_h1runAvgV.end()) {
    return 0;
  }

  std::string dpidName= (m_dpidAliases.find(dpid)!=m_dpidAliases.end()) ? m_dpidAliases.at(dpid) : std::to_string(dpid);

  std::stringstream stitle, shName;
  float rMin= m_fromRun - 1.5;
  float rMax= m_toRun + 1.5;
  int nBins= m_toRun - m_fromRun + 3;

  shName << "h1avgImon_" << dpid;
  stitle << dpidName << " avgImon runs=" << m_fromRun << " - " << m_toRun
	 << ";runNo;<#it{I}>_{mon}";
  TH1F *h1_avgImon= new TH1F(shName.str().c_str(),stitle.str().c_str(), nBins,rMin,rMax);
  h1_avgImon->SetDirectory(0);
  h1_avgImon->GetXaxis()->SetNoExponent();
  h1_avgImon->SetMarkerStyle(24); // open circles

  shName.str(std::string()); // clear value
  stitle.str(std::string());
  shName << "h1avgVmon_" << dpid;
  stitle << dpidName << " avgVmon runs=" << m_fromRun << " - " << m_toRun
       << ";runNo;<#it{V}>_{mon}";
  TH1F *h1_avgVmon= new TH1F(shName.str().c_str(),stitle.str().c_str(), nBins,rMin,rMax);
  h1_avgVmon->SetDirectory(0);
  h1_avgVmon->GetXaxis()->SetNoExponent();
  h1_avgVmon->SetMarkerStyle(24);

  m_h1runAvgV[dpid].push_back(h1_avgImon);
  m_h1runAvgV[dpid].push_back(h1_avgVmon);
  return 1;
}

int GEMPVSSFWCAENChannelIVRunAvgPlotter::fillHistos(int iRun, const GEMPVSSFWCAENChannelIVRunAvg &ivRunAvg)
{
  // store only matching-run values
  if (decltype(ivRunAvg.runTime_.run)(iRun) != ivRunAvg.runTime_.run) return -1;

  // check whether detailed plots should be produced
  if (m_savePVSShistos) {
    if (!fillRunTrendHistos(iRun, ivRunAvg.monData_)) return 0;
  }

  std::cout << "\n\tcalling fillHistos for iRun=" << iRun << std::endl;
  int res=(fillHistos(iRun,ivRunAvg.avgImon_,ivRunAvg.avgImonRMS_,_h1_avgImon) &&
	   fillHistos(iRun,ivRunAvg.avgVmon_,ivRunAvg.avgVmonRMS_,_h1_avgVmon));
  return res;
}

int GEMPVSSFWCAENChannelIVRunAvgPlotter::fillHistos(int iRun, const std::map<int,float> &avg, const std::map<int,float> &rms, int hidx)
{
  for ( auto it : avg ) {
    if (m_h1runAvgV.find(it.first) == m_h1runAvgV.end()) createHistos(it.first);
    int ibin= m_h1runAvgV[it.first][hidx]->Fill( iRun, it.second );
    float unc= rms.at(it.first);
    if (unc==static_cast<float>(0)) unc=1e-5; // force same style plotting
    m_h1runAvgV[it.first][hidx]->SetBinError( ibin, unc );
    std::cout << "for dpid=" << it.first << " hidx=" << hidx << " iRun="
	      << iRun << " val=" << it.second << " rms=" << rms.at(it.first)
	      << "; ibin=" << ibin << std::endl;
  }
  return 1;
}


template<class TGEMMonType_t>
void GEMIVRunAvgPlotter__adjustTimeRange(const std::vector<GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> > &data, boost::posix_time::ptime &tic, boost::posix_time::ptime &tac)
{
  if (data.size()==0) return;
  for ( auto it : data ) {
    if (it.timeStamp < tic) tic= it.timeStamp;
    if (it.timeStamp > tac) tac= it.timeStamp;
  }
}


template<class TGEMMonType_t>
int GEMIVRunAvgPlotter__fillRunTrendHistos(const std::vector<GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> > &data, std::vector<TH1F*> &h1V, const std::string &baseTag, const boost::posix_time::ptime &tic, const boost::posix_time::ptime &tac, const std::map<int,std::string> *dpidAliases=NULL)
{
  if (data.size()==0) return -1;

  // assume sorted list
  int oldDPID=0;
  TH1F *h1=NULL; // the bin-averaged value
  TH1F *h1count=NULL, *h1sq=NULL; // helpers
  unsigned int hidxStart=h1V.size();
  std::vector<TH1F*> h1countV, h1sqV;
  for ( auto it : data ) {
    //const boost::posix_time::ptime time0= boost::posix_time::from_time_t(0);
    //const TDatime dt_time0(convert2Datime(time0));

    if (it.dpid!=oldDPID) {
      // create hnew histos
      std::stringstream shname, shtitle;
      shname << baseTag << it.dpid;
      std::string titleBase=baseTag.substr(3,baseTag.size());
      boost::replace_all(titleBase,"_"," ");
      shtitle << titleBase;
      if (dpidAliases) {
	if (dpidAliases->find(it.dpid) != dpidAliases->end()) {
	  shtitle << dpidAliases->at(it.dpid);
	}
	else shtitle << it.dpid;
      }
      shtitle << ", " << it.timeStamp.date();
      shtitle << ";time; obs " << it.type.getName();
      TDatime dt_tic(convert2Datime(tic));
      TDatime dt_tac(convert2Datime(tac));
      const int nBins=100;
      h1= new TH1F(shname.str().c_str(),shtitle.str().c_str(),nBins, dt_tic.Convert(),dt_tac.Convert());
      h1count= new TH1F((shname.str()+"count").c_str(),(shtitle.str()+"count").c_str(),nBins, dt_tic.Convert(),dt_tac.Convert());
      h1sq= new TH1F((shname.str()+"sq").c_str(),(shtitle.str()+"sq").c_str(),nBins, dt_tic.Convert(),dt_tac.Convert());

      if (!h1 || !h1count || !h1sq) return 0;

      h1V.push_back(h1); // store the histogram
      h1count->SetDirectory(0);   h1countV.push_back(h1count);
      h1sq->SetDirectory(0);   h1sqV.push_back(h1sq);
      h1->SetDirectory(0);
      h1->GetXaxis()->SetTimeDisplay(1);
      //std::string tf("%y-%b-%d %H:%M%F1970-01-01 00:00:00"); // +"%F" + time0str
      std::string tf("%H:%M%F1970-01-01 00:00:00"); // +"%F" + time0str
      h1->GetXaxis()->SetTimeFormat(tf.c_str());
      //h1->GetXaxis()->SetTimeOffset(dt_time0.Convert());
      //h1->GetXaxis()->SetNdivisions(502,false);
      h1->GetXaxis()->SetNdivisions(504,false);
      h1->SetStats(0);
      h1->SetMarkerStyle(20); // filled circles
    }

    // put the value into the histogram
    TDatime dt_pt(convert2Datime( it.timeStamp ));
    double ts = dt_pt.Convert();
    h1count->Fill( ts, 1 );
    h1->Fill( ts, it.value );
    h1sq->Fill( ts, it.value*it.value );
    oldDPID = it.dpid;
  }

  // finish calculation
  for (unsigned int ih=hidxStart; ih<h1V.size(); ih++) {
    TH1F *h1= h1V[ih];
    TH1F *h1c= h1countV[ih-hidxStart];
    TH1F *h1sq= h1sqV[ih-hidxStart];
    if (!h1 || !h1c || !h1sq) {
      std::cout << "code error: null pointer\n";
      return 0;
    }
    for (int ibin=1; ibin<=h1->GetNbinsX(); ibin++) {
      if (h1c->GetBinContent(ibin)!=0) {
	float avg= h1->GetBinContent(ibin)/h1c->GetBinContent(ibin);
	float rms= h1sq->GetBinContent(ibin)/h1c->GetBinContent(ibin) - avg*avg;
	if (rms>0) rms=sqrt(rms);
	else {
	  if (-rms/avg<1e-4) rms=-1e-6; // calculation accuracy
	  else {
	    if (fabs(rms)>1e-4) {
	      std::cout << "negative rms ibin=" << ibin << ", avg=" << avg << ", rmssq=" << rms << " h1->GetName()=" << h1->GetName() << "\n";
	    }
	    rms=sqrt(-rms);
	  }
	}
	h1->SetBinContent(ibin, avg);
	h1->SetBinError( ibin, rms);
      }
    }
    delete h1c;  h1countV[ih-hidxStart]=NULL;
    delete h1sq;  h1sqV[ih-hidxStart]=NULL;
  }

  return 1;
}


int GEMPVSSFWCAENChannelIVRunAvgPlotter::fillRunTrendHistos(int iRun, const GEMPVSSFWCAENChannel &data)
{
  if ((data.gem_ObImon_.size()==0) &&
      (data.gem_ObVmon_.size()==0)) return 1; // nothing to do

  std::vector<TH1F*>* h1v= new std::vector<TH1F*>();
  m_h1trendsV[iRun]= h1v;

  // determine time spans
  boost::posix_time::ptime tic=boost::posix_time::max_date_time;
  boost::posix_time::ptime tac=boost::posix_time::min_date_time;
  GEMIVRunAvgPlotter__adjustTimeRange(data.gem_ObImon_, tic,tac);
  GEMIVRunAvgPlotter__adjustTimeRange(data.gem_ObVmon_, tic,tac);
  boost::posix_time::time_duration ts= boost::posix_time::seconds(3);
  tic -= ts;
  tac += ts;

  std::stringstream ss;
  ss << iRun;
  std::string sRun= ss.str();

  // create and fill the histograms
  int res= (GEMIVRunAvgPlotter__fillRunTrendHistos(data.gem_ObImon_, *h1v, "h1_"+sRun+"_obsImon_", tic,tac, &m_dpidAliases) &&
	    GEMIVRunAvgPlotter__fillRunTrendHistos(data.gem_ObVmon_, *h1v, "h1_"+sRun+"_obsVmon_", tic,tac, &m_dpidAliases)) ? 1:0;

  return res;
}


int GEMPVSSFWCAENChannelIVRunAvgPlotter::saveHistos()
{

  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
    << "save to " << m_outHistoFile << std::endl;

  std::cout << "m_outHistoFile=" << m_outHistoFile << std::endl;

  if (m_outHistoFile.size()) {
    TFile fout(m_outHistoFile.c_str(), "RECREATE");
    if (fout.IsOpen()) {
      for ( auto ihrunAvgV : m_h1runAvgV ) {
	std::cout << "saving runAvg dpid=" << ihrunAvgV.first << "\n";
	for ( auto h : ihrunAvgV.second ) {
	  h->Write();
	}
      }
    }

    if (m_savePVSShistos) {
      for ( auto ihtrendV : m_h1trendsV ) {
	fout.cd();
	if (ihtrendV.second==NULL) continue; // should be warning
	std::stringstream sout;
	sout << ihtrendV.first;
	fout.mkdir( sout.str().c_str() );
	fout.cd( sout.str().c_str() );
	for ( auto h : (*ihtrendV.second) ) {
	  h->Write();
	}
      }
      fout.cd();
    }


    namespace pt = boost::posix_time;
    std::string tstr= pt::to_iso_string(pt::second_clock::local_time());
    TObjString timeostr(tstr.c_str());
    timeostr.Write(timeostr.String());
    fout.Close();

    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
      << " file <" << fout.GetName() << "> saved" << std::endl;

    return 1;
  }
  return 0;
}



void GEMPVSSFWCAENChannelIVRunAvgPlotter::getNewObjects() {
  //check whats already inside of database
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
      << m_name << ": "
      << "Destination Tag Info: name " << tagInfo().name
      << ", size " << tagInfo().size
      << ", last object valid since " << tagInfo().lastInterval.first
      << ", hash " << tagInfo().lastPayloadToken << std::endl;
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
    << m_name << ": fromRun = " << m_fromRun << ", toRun=" << m_toRun << std::endl;

  //reading from PVSS
  for (auto iRun= m_fromRun; iRun <= m_toRun; iRun++) {
    edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
      << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
      << " work on iRun=" << iRun << "\n";
    try {
      GEMPVSSFWCAENChannelIVRunAvg* ivRunAvg = new GEMPVSSFWCAENChannelIVRunAvg();
      GEMPVSSFWCAENChannelIVRunAvgReadDBs ivread( m_connectionString, m_connectionPset );
      *ivRunAvg = ivread.readData( m_runinfo_schema, m_pvss_schema, (int)iRun );

      // put the record into DB
      if (m_saveToDB) {
	m_to_transfer.push_back( std::make_pair( (GEMPVSSFWCAENChannelIVRunAvg*)ivRunAvg, iRun) );
      }
      //ss << "run number: " << iRun << ";";
      //m_userTextLog = ss.str();

      if (!fillHistos(iRun,*ivRunAvg)) {
	edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
	  << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
	  << " histos not filled for run=" << iRun << std::endl;
      }
    }
    catch (const std::exception &e) {
      edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
	<< "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
	<< m_name << ": failed to get a payload for run_number=" << iRun << std::endl
	<< "\tOriginal Exception: " << e.what() << std::endl;
      std::string errMsg=e.what();
      if (errMsg.find("ConnectionPool::getSessionFromNewConnection")!=std::string::npos) {
	errMsg= "rethrowing  " + errMsg;
	throw std::runtime_error(errMsg.c_str());
      }
    }
  }

  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
    << m_name << ": SaveHistos" << std::endl;
  int res= this->saveHistos();
  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
    << m_name << ": SaveHistos res=" << res << std::endl;


  edm::LogInfo( "GEMPVSSFWCAENChannelIVRunAvgPlotter" )
    << "[" << "GEMPVSSFWCAENChannelIVRunAvgPlotter::" << __func__ << "]:"
    << m_name << ": END." << std::endl;
}
