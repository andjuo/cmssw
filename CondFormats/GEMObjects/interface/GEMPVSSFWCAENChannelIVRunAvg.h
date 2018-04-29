#ifndef CondFormats_GEMObjects_GEMPVSSFWCAENChannelIVRunAvg_h
#define CondFormats_GEMObjects_GEMPVSSFWCAENChannelIVRunAvg_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
//#include "CondCore/CondDB/interface/Types.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <string>
//#include <vector>
#include <iostream>

/*
 *  \class GEMPVSSFWCAENChannelIVRunAvg
 *
 *  Keeps run averages for Imon and Vmon from CMS_GEM_PVSS_COND.FWCAENCHANNELA1515
 *
 *  \author Andrius Juodagalvis (andjuo) - Vilnius University (Apr 15, 2018)
 *
*/

extern int __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg;

class GEMPVSSFWCAENChannelIVRunAvg {
 public:
  // The same as used in RunInfo_t in CondCore/CondDB/interface/Types.h
  typedef std::tuple<long long unsigned int, boost::posix_time::ptime,boost::posix_time::ptime> runTimeTupleType;

  struct RunTime {
   public:
    RunTime();
    explicit RunTime(const GEMPVSSFWCAENChannelIVRunAvg::runTimeTupleType & data);
    explicit RunTime(long long unsigned int r, const boost::posix_time::ptime &tic, const boost::posix_time::ptime &tac);
    void setStartTime(const boost::posix_time::ptime &tic) { setTime(tic,start,start_ll,start_str); }
    void setEndTime(const boost::posix_time::ptime &tac) { setTime(tac,end,end_ll,end_str); }
    void print(std::ostream &out, int fullOutput=0) const;
    void printStartTime(std::ostream &out) const { printTime(out,start,start_ll,start_str); }
    void printEndTime(std::ostream &out) const { printTime(out,end,end_ll,end_str); }

  private:
    void setTime(const boost::posix_time::ptime &t, boost::posix_time::ptime &storeTime, long long &storeTimeLL, std::string &storeTimeStr);
    void printTime(std::ostream &out, const boost::posix_time::ptime &t, const long long &t_ll, const std::string &t_str) const;

    public:
    long long unsigned int run;
    boost::posix_time::ptime start;
    long long start_ll; // microseconds from epoch
    std::string start_str; // boost::posix_time::to_iso_extended_string(time_duration)
    boost::posix_time::ptime end;
    long long end_ll;
    std::string end_str;

    COND_SERIALIZABLE;
  };

  // define the main class
 public:
  GEMPVSSFWCAENChannelIVRunAvg();
  explicit GEMPVSSFWCAENChannelIVRunAvg(const std::string & set_version);
  virtual ~GEMPVSSFWCAENChannelIVRunAvg();
  const std::string & version() const;
  int calcStat(); // assuming monData_ is filled, fill the avg and rms maps
  void print(std::ostream &out=std::cout, int fullOutput=1) const;

  RunTime runTime_; // CondCore/CondDB, not CondCore/RunInfo!
  GEMPVSSFWCAENChannel monData_;
  std::map<int,float> avgImon_, avgImonRMS_; // monitored current
  std::map<int,float> avgVmon_, avgVmonRMS_; // monitored voltage
 private:
  std::string theVersion;

  COND_SERIALIZABLE;
};

// allow printing
std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannelIVRunAvg::RunTime &);
std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannelIVRunAvg &);

//
// define templated function
//

#endif // GEMPVSSFWCAENChannelIVRunAvg_h
