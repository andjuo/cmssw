#ifndef CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgPlotter_H
#define CondTools_GEM_GEMPVSSFWCAENChannelIVRunAvgPlotter_H

//#include <string>

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannelIVRunAvg.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <TH1F.h>

class GEMPVSSFWCAENChannelIVRunAvgPlotter : public popcon::PopConSourceHandler<GEMPVSSFWCAENChannelIVRunAvg> {
 public:
  typedef enum { _h1_avgImon=0, _h1_avgVmon } THistoIdx;

 public:
  GEMPVSSFWCAENChannelIVRunAvgPlotter(const edm::ParameterSet& pset);
  ~GEMPVSSFWCAENChannelIVRunAvgPlotter() override;
  void getNewObjects() override;
  std::string id() const override { return m_name; }

 protected:
  void fill_dpidAliases();
  int createHistos(int dpid);
  int fillHistos(int iRun, const GEMPVSSFWCAENChannelIVRunAvg &ivRunAvg); // fill all requested histos
  int fillHistos(int iRun, const std::map<int,float> &avg, const std::map<int,float> &rms, int hidx); // fill run-avg histos (m_h1V)
  int fillRunTrendHistos(int iRun, const GEMPVSSFWCAENChannel &data); // fill dpid trends // m_h1pvssV

  //template<class TGEMMonType_t>
    //int fillRunTrendHistos(const std::vector<GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> > &data, std::vector<TH1F*> &h1V);

  int saveHistos();

 private:
  unsigned long long m_fromRun, m_toRun;
  std::string m_name;
  std::string m_outHistoFile;
  int m_savePVSShistos;
  int m_saveToDB;

  // for reading from omds
  std::string m_runinfo_schema;
  std::string m_pvss_schema;
  std::string m_alias_connectionString;
  std::string m_connectionString;
  edm::ParameterSet m_connectionPset;
  std::map<int,std::vector<TH1F*> > m_h1runAvgV; // run-averaged values
  std::map<int,std::vector<TH1F*>* > m_h1trendsV; // dpid trends during individual runs
  std::map<int,std::string> m_dpidAliases;
};

// ----------------------------------------------------------------------
/*
template<class TGEMMonType_t>
int GEMPVSSFWCAENChannelIVRunAvgPlotter::fillRunTrendHistos(const std::vector<GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> > &data, std::vector<TH1F*> &h1V,
							    const std::string &baseTag,
							    const boost::posix_time::ptime &tic, const boost::posix_time::ptime &tac)
{
  if (data.size()==0) return 0;

  std::stringstream shnameBase;
  shnameBase << "h1", data[0].type.getName();

  // assume sorted list
  int oldDPID=0;
  TH1F *h1=NULL;
  for ( auto it : data ) {
    boost::posix_time::time_duration ts= data.timeStamp - tic;
    if (it.dpid!=oldDPID) {
      // create hnew histo
      std::stringstream shname;
      shname >>
      h1= new TH1F(
    }
      int idx= h1->Fill( ts.total_seconds(), it.value );
      h1->SetBinError( idx, 1e-5 ); // prevent histograms
  }
}
*/
// ----------------------------------------------------------------------

#endif
