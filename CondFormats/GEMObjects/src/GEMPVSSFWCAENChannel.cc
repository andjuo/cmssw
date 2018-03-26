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

void GEMPVSSFWCAENChannel::print(std::ostream & out) const
{
  out << "GEMPVSSFWCAENChannel(version=" << theVersion << ") :"
      << "ObImon[" << gem_ObImon_.size() << "], "
      << "ObVmon[" << gem_ObVmon_.size() << "]"
      << "\n";
  out << "ObImon :\n";
  for (auto im : gem_ObImon_) im.print(out);
  out << "ObVmon :\n";
  for (auto vm : gem_ObVmon_) vm.print(out);
  out << std::endl;
}

std::ostream & operator<< (std::ostream & out, const GEMPVSSFWCAENChannel &info)
  { info.print(out); return out; }
