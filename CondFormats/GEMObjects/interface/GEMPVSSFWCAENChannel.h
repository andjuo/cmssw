#ifndef CondFormats_GEMObjects_GEMPVSSFWCAENChannel_h
#define CondFormats_GEMObjects_GEMPVSSFWCAENChannel_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include "CoralBase/TimeStamp.h"
#include <string>
#include <vector>
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
      int day, time; // day and time
      float value;   // value
      TGEMMonType_t type; // template parameter without a value

      explicit GEMMonItem(int setDPid=-1, int setDay=0, int setTime=0,
			  float setValue=0) :
      dpid(setDPid), day(setDay), time(setTime), value(setValue) {}

      void setDate(const coral::TimeStamp &ts);
      void print(std::ostream &out=std::cout) const;
      COND_SERIALIZABLE;
    };

  void print(std::ostream &out=std::cout) const;

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
void GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t>::setDate(const coral::TimeStamp &ts)
{
  day= (ts.year() * 10000) + (ts.month() * 100) + ts.day();
  time= (ts.hour() * 10000) + (ts.minute() * 100) + ts.second();
}

template<class TGEMMonType_t>
void GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t>::print(std::ostream &out) const
{
  out << this->type.getName() << " " << dpid << " " << day << " " << time << "  " << value << "\n";
}

template<class TGEMMonType_t>
std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannel::GEMMonItem<TGEMMonType_t> & monData)
{
  monData.print(out);
  return out;
}



#endif // GEMPVSSFWCAENChannel_h
