#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannelIVRunAvg.h"
#include <string>
//#include <vector>

const boost::posix_time::ptime zeroBoostTime = boost::posix_time::from_time_t(0);
int __prnFormLL_GEMPVSSFWCAENChannelIVRunAvg=0;

GEMPVSSFWCAENChannelIVRunAvg::RunTime::RunTime() :
  run(0), start(), start_ll(0), start_str(), end(), end_ll(0), end_str()
{}

GEMPVSSFWCAENChannelIVRunAvg::RunTime::RunTime(const GEMPVSSFWCAENChannelIVRunAvg::runTimeTupleType & data) :
  run(std::get<0>(data)),
  start(std::get<1>(data)), start_ll(0), start_str(),
  end(std::get<2>(data)), end_ll(0), end_str()
{
  boost::posix_time::ptime temp;
  setTime(start, temp,start_ll,start_str);
  setTime(end, temp,end_ll,end_str);
}

GEMPVSSFWCAENChannelIVRunAvg::RunTime::RunTime(long long unsigned int r, const boost::posix_time::ptime &tic, const boost::posix_time::ptime &tac) :
  run(r),
  start(tic), start_ll(0), start_str(),
  end(tac), end_ll(0), end_str()
{
  boost::posix_time::ptime temp;
  setTime(start, temp,start_ll,start_str);
  setTime(end, temp,end_ll,end_str);
}


void GEMPVSSFWCAENChannelIVRunAvg::RunTime::print(std::ostream &out, int fullOutput) const
{
  out << "RunTime(" << run << "  " << start_str << "  " << end_str << ")";
  if (fullOutput) out << ": " << boost::posix_time::to_iso_extended_string(start)
		      << " - " << boost::posix_time::to_iso_extended_string(end);
}

void GEMPVSSFWCAENChannelIVRunAvg::RunTime::setTime(const boost::posix_time::ptime &t, boost::posix_time::ptime &storeTime, long long &storeTimeLL, std::string &storeTimeStr)
{
  storeTime = t;
  if (storeTime == zeroBoostTime) {
    storeTimeLL = -1;
    storeTimeStr = "null";
  }
  else {
    boost::posix_time::time_duration startTimeFromEpoch = t - zeroBoostTime;
    storeTimeLL = startTimeFromEpoch.total_microseconds();
    storeTimeStr = boost::posix_time::to_iso_extended_string(t);
  }
}

void GEMPVSSFWCAENChannelIVRunAvg::RunTime::printTime(std::ostream &out, const boost::posix_time::ptime &t, const long long &t_ll, const std::string &t_str) const
{
  out << "run number: " << this->run << std::endl;
  out << "Posix Time: " << t << std::endl;
  out << "ISO string: " << t_str << std::endl;
  out << "Microsecond since Epoch (UTC): " << t_ll << std::endl;
}


GEMPVSSFWCAENChannelIVRunAvg::GEMPVSSFWCAENChannelIVRunAvg():
  runTime_(),
  monData_(),
  avgImon_(), avgImonRMS_(),
  avgVmon_(), avgVmonRMS_(),
  theVersion("")
{}

GEMPVSSFWCAENChannelIVRunAvg::GEMPVSSFWCAENChannelIVRunAvg(const std::string & set_version):
  runTime_(),
  monData_(),
  avgImon_(), avgImonRMS_(),
  avgVmon_(), avgVmonRMS_(),
  theVersion(set_version)
{}

GEMPVSSFWCAENChannelIVRunAvg::~GEMPVSSFWCAENChannelIVRunAvg() {}


const std::string& GEMPVSSFWCAENChannelIVRunAvg::version() const
{ return theVersion; }


int GEMPVSSFWCAENChannelIVRunAvg::calcStat()
{
  int resI= monData_.getImonStat(avgImon_, avgImonRMS_);
  int resV= monData_.getVmonStat(avgVmon_, avgVmonRMS_);
  int res= ((resI==0) || (resV==0)) ? 0 : 1;
  return res;
}


void GEMPVSSFWCAENChannelIVRunAvg::print(std::ostream & out, int fullOutput) const
{
  out << "GEMPVSSFWCAENChannelIVRunAvg(version=" << theVersion << ") :\n"
      << "  runTime: "; runTime_.print(out,fullOutput);
  out << ", ";
  if (fullOutput) {
    if (__prnFormLL_GEMPVSSFWCAENChannelIVRunAvg==1) monData_.printLL(out);
    else if (__prnFormLL_GEMPVSSFWCAENChannelIVRunAvg==2) monData_.printSizes(out);
    else monData_.print(out);
  }
  for (auto i : avgImon_) {
    out << " avgImon[" << i.first << "]=" << i.second << " +- " << avgImonRMS_.at(i.first) << "\n";
  }
  for (auto v : avgVmon_) {
    out << " avgVmon[" << v.first << "]=" << v.second << " +- " << avgVmonRMS_.at(v.first) << "\n";
  }
}

std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannelIVRunAvg::RunTime &rt)
{ rt.print(out); return out; }

std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannelIVRunAvg &info)
  { info.print(out); return out; }
