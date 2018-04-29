#ifndef CondFormats_GEMObjects_GEMPVSSFWCAENChannel_h
#define CondFormats_GEMObjects_GEMPVSSFWCAENChannel_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include "CoralBase/TimeStamp.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>
#include <string>
#include <vector>
#include <map>
#include <iostream>

// Based on CMS_GEM_PVSS_COND.FWCAENCHANNELA1515

class GEMPVSSFWCAENChannel {
 public:
  GEMPVSSFWCAENChannel();
  explicit GEMPVSSFWCAENChannel(const std::string & version);
  virtual ~GEMPVSSFWCAENChannel();
  const std::string & version() const;

  typedef enum { _gem_imon, _gem_vmon } GEMMonParam_t;

  struct GEM_IMonParam {
    GEMPVSSFWCAENChannel::GEMMonParam_t getID() const { return _gem_imon; }
    const char* getName() const { return "Imon"; }
    COND_SERIALIZABLE;
  };
  struct GEM_VMonParam {
    GEMPVSSFWCAENChannel::GEMMonParam_t getID() const { return _gem_vmon; }
    const char* getName() const { return "Vmon"; }
    COND_SERIALIZABLE;
  };

  template<class TGEMMonType_t>
    struct GEMMonItem {
      int dpid;      // data-point id
      boost::posix_time::ptime timeStamp;
      float value;   // value
      TGEMMonType_t type; // template parameter without a value

      explicit GEMMonItem(int setDPid=-1,
			  const boost::posix_time::ptime setTime=boost::posix_time::from_time_t(0),
			  float setValue=0) :
      dpid(setDPid), timeStamp(setTime), value(setValue) {}

      void setDate(const coral::TimeStamp &ts) { timeStamp=ts.time(); }
      void print(std::ostream &out=std::cout) const;
      void printLL(std::ostream &out=std::cout, const boost::posix_time::ptime fromTime=boost::posix_time::from_time_t(0)) const;
      COND_SERIALIZABLE;
    };

  template<class TGEMMonType_t>
    int getStat(const std::vector<GEMMonItem<TGEMMonType_t> > &vec,
		std::map<int,float> &avg, std::map<int,float> &rms) const;
  int getImonStat(std::map<int,float> &avg, std::map<int,float> &rms) const;
  int getVmonStat(std::map<int,float> &avg, std::map<int,float> &rms) const;

  void print(std::ostream &out=std::cout) const;
  void printLL(std::ostream &out=std::cout) const;
  void printSizes(std::ostream &out=std::cout) const;

  std::vector<GEMMonItem<GEM_IMonParam> > gem_ObImon_;
  std::vector<GEMMonItem<GEM_VMonParam> > gem_ObVmon_;
 private:
  std::string theVersion;

  COND_SERIALIZABLE;
};

// allow printing
std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannel &info);

//
// define templated function
//

template<class TGEMMonType_t>
void GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t>::print(std::ostream &out) const
{
  out << this->type.getName() << " " << dpid << " " << timeStamp << "  " << value;
}

template<class TGEMMonType_t>
void GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t>::printLL(std::ostream &out, const boost::posix_time::ptime fromTime) const
{
  boost::posix_time::time_duration dTime = timeStamp - fromTime;
  out << this->type.getName() << " " << dpid << " " << dTime.total_microseconds() << "  " << value;
}

template<class TGEMMonType_t>
int GEMPVSSFWCAENChannel::getStat(const std::vector<GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> > &vec, std::map<int,float> &avg, std::map<int,float> &rms) const
{
  if (vec.size()==0) return 0;

  avg.clear(); rms.clear();
  std::vector<int> processed;
  std::map<int,boost::posix_time::ptime> lastTime;
  std::map<int,float> countTime;
  std::map<int,float> firstValue;
  std::map<int,int> count;
  //float oldVal=0; // old val is needed for averaging, which is not good for equal-size fluctuations
  for (auto i : vec) {
    count[i.dpid]++;
    if ( lastTime.find(i.dpid) == lastTime.end() ) {
      lastTime[i.dpid] = i.timeStamp;
      firstValue[i.dpid] = i.value;
      //oldVal= i.value;
    }
    else {
      boost::posix_time::time_duration tdiff= i.timeStamp - lastTime[i.dpid];
      lastTime[i.dpid] = i.timeStamp;
      float w= tdiff.total_microseconds();
      float useVal= i.value;
      //float useVal= 0.5*(i.value + oldVal);  // avg.value for the time interval
      //oldVal=i.value;
      countTime[i.dpid]+= w;
      avg[i.dpid]+= w* useVal;
      rms[i.dpid]+= w* useVal*useVal;
    }
  }

  // correct for 1-entry cases
  for (auto i : count) {
    if (i.second==1) {
      //std::cout << "correct 1-entry cases: i.first=" << i.first << std::endl;
      avg[i.first] = firstValue.at(i.first);
      rms[i.first] = 0;
    }
  }

  // calculate averages
  for (auto ic : countTime) {
    if (ic.second!=0) {
      avg[ic.first] /= ic.second;
      rms[ic.first] /= ic.second;
    }
  }

  // finish calculation of rms
  int err=0;
  for (auto ir : rms) {
    //std::cout << "finish rms " << ir.first << std::endl;
    if (count.at(ir.first)<=2) {
      rms[ir.first]=0; // too few entries
      continue;
    }
    double a= avg.at(ir.first);
    double v= ir.second - a*a; // avgX2 - (avgX)^2
    if (v>=0) v=sqrt(v); else { v=-sqrt(-v); err=1; }
    rms[ir.first]= v;
  }
  if (err) std::cout << "\t\tnegative rms detected" << std::endl;
  return 1;
}

template<class TGEMMonType_t>
std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> & monData)
{
  monData.print(out);
  return out;
}



#endif // GEMPVSSFWCAENChannel_h
