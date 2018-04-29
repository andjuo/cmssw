#include "CondFormats/GEMObjects/interface/GEMPVSSFWCAENChannel.h"
#include <string>
#include <vector>

// Based on CMS_GEM_PVSS_COND.FWCAENCHANNELA1515

GEMPVSSFWCAENChannel::GEMPVSSFWCAENChannel():
  gem_ObImon_(), gem_ObVmon_(), theVersion("") {}

GEMPVSSFWCAENChannel::GEMPVSSFWCAENChannel(const std::string & version):
  gem_ObImon_(), gem_ObVmon_(), theVersion(version) {}

GEMPVSSFWCAENChannel::~GEMPVSSFWCAENChannel() {}

const std::string& GEMPVSSFWCAENChannel::version() const
{ return theVersion; }

int GEMPVSSFWCAENChannel::getImonStat(std::map<int,float> &avg, std::map<int,float> &rms) const
{ return this->getStat(gem_ObImon_, avg,rms); }

int GEMPVSSFWCAENChannel::getVmonStat(std::map<int,float> &avg, std::map<int,float> &rms) const
{ return this->getStat(gem_ObVmon_, avg,rms); }

void GEMPVSSFWCAENChannel::print(std::ostream & out) const
{
  out << "GEMPVSSFWCAENChannel(version=" << theVersion << ") :"
      << "ObImon[" << gem_ObImon_.size() << "], "
      << "ObVmon[" << gem_ObVmon_.size() << "]"
      << "\n";
  out << "ObImon :\n";
  for (auto im : gem_ObImon_) { im.print(out); out << "\n"; }
  out << "ObVmon :\n";
  for (auto vm : gem_ObVmon_) { vm.print(out); out << "\n"; }
  out << std::endl;
}

void GEMPVSSFWCAENChannel::printLL(std::ostream & out) const
{
  out << "GEMPVSSFWCAENChannel::printLL(version=" << theVersion << ") :"
      << "ObImon[" << gem_ObImon_.size() << "], "
      << "ObVmon[" << gem_ObVmon_.size() << "]"
      << "\n";

  boost::posix_time::ptime firstTime(boost::posix_time::max_date_time);
  for (auto im : gem_ObImon_)
    if (im.timeStamp < firstTime) firstTime=im.timeStamp;
  for (auto vm : gem_ObVmon_)
    if (vm.timeStamp < firstTime) firstTime=vm.timeStamp;

  out << "ObImon :\n";
  for (auto im : gem_ObImon_) { im.printLL(out,firstTime); out << "\n"; }
  out << "ObVmon :\n";
  for (auto vm : gem_ObVmon_) { vm.printLL(out,firstTime); out << "\n"; }
  out << std::endl;
}

void GEMPVSSFWCAENChannel::printSizes(std::ostream & out) const
{
  out << "GEMPVSSFWCAENChannel::printSizes(version=" << theVersion << ") :"
      << "ObImon[" << gem_ObImon_.size() << "], "
      << "ObVmon[" << gem_ObVmon_.size() << "]"
      << "\n";
}

std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannel &info)
  { info.print(out); return out; }
