#include "CondFormats/GEMObjects/interface/GEMEMap.h"
#include "CondFormats/GEMObjects/interface/GEMROmap.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"

GEMEMap::GEMEMap():
  theVersion("") {}

GEMEMap::GEMEMap(const std::string & version):
  theVersion(version) {}

GEMEMap::~GEMEMap() {}

const std::string & GEMEMap::version() const{
  return theVersion;
}

GEMROmap* GEMEMap::convert() const{
  GEMROmap* romap=new GEMROmap();

  for (auto imap : theVFatMaptype){
    for (unsigned int ix=0;ix<imap.strip_number.size();ix++){
      GEMROmap::eCoord ec;
      ec.vfatId= imap.vfatId[ix] & chipIdMask_;// chip ID is 12 bits
      ec.channelId=imap.vfat_chnnel_number[ix];

      int st = std::abs(imap.z_direction[ix]);
      int maxVFat = maxVFatGE11_;
      if (st == 2) maxVFat = maxVFatGE21_;      
      
      GEMROmap::dCoord dc;
      dc.stripId = 1 + imap.strip_number[ix]+(imap.iPhi[ix]-1)%maxVFat*maxChan_;
      dc.gemDetId = GEMDetId(imap.z_direction[ix], 1, st, imap.depth[ix], imap.sec[ix], imap.iEta[ix]);
      
      romap->add(ec,dc);
      romap->add(dc,ec);
    }
  }
  return romap;
}

GEMROmap* GEMEMap::convertDummy() const{
  GEMROmap* romap=new GEMROmap();
  uint16_t amcId = 1; //amc
  uint16_t gebId = 1; 
  romap->addAMC(amcId);
	
  for (int re = -1; re <= 1; re = re+2) {
    for (int st = GEMDetId::minStationId; st<=GEMDetId::maxStationId; ++st) {
      int maxVFat = maxVFatGE11_;
      if (st == 2) maxVFat = maxVFatGE21_;      
      
      for (int ch = 1; ch<=GEMDetId::maxChamberId; ++ch) {
      	for (int ly = 1; ly<=GEMDetId::maxLayerId; ++ly) {
	  
	  // 1 geb per chamber
	  // 24 gebs per amc
	  // make new amcId once 24 gebs are used up
	  if (gebId > maxGEBs_){
	    gebId = 1;
	    amcId++;
	    romap->addAMC(amcId);
	  }

	  romap->addAMC2GEB(amcId, gebId);
	  
	  GEMDetId chamDetId(re, 1, st, ly, ch, 0);
	  uint32_t chamberId = (amcId << gebIdBits_) | gebId;	  
	  romap->add(chamDetId,chamberId);
	  romap->add(chamberId,chamDetId);
	  
	  uint16_t chipId = 0;	 	  
	  for (int roll = 1; roll<=GEMDetId::maxRollId; ++roll) {
	    
	    int stripId = 0;
	    GEMDetId gemId(re, 1, st, ly, ch, roll);
	    
	    for (int nVfat = 0; nVfat < maxVFat; ++nVfat){
	      chipId++;
	      
	      for (unsigned chan = 0; chan < maxChan_; ++chan){
		GEMROmap::dCoord dc;
		dc.stripId = ++stripId;
		dc.gemDetId = gemId;

		// make 1 full vfat ID from amc + geb + chip Ids
		uint32_t vfatId = (amcId << (gebIdBits_+chipIdBits_)) | (gebId << chipIdBits_) | chipId;
		
		GEMROmap::eCoord ec;
		ec.vfatId =  vfatId;
		ec.channelId = chan;
		romap->add(ec,dc);
		romap->add(dc,ec);

	      }
	    }
	  }

	  gebId++;

	}
      }
    }
  }
  
  return romap;
}

// ------

int GEMEMap::isConsistent(int verbose, std::ostream &out) const
{
  int ok=1;
  for (unsigned int i=0; (verbose || ok) && i<theEMapItem.size(); i++) {
    if (theEMapItem[i].isNotConsistent()) {
      ok=0;
      if (verbose) out << "theEMapItem @" << i << " is not consistent\n";
    }
  }
  for (unsigned int i=0; (verbose || ok) && i<theVFatMaptype.size(); i++) {
    if (theVFatMaptype[i].isNotConsistent()) {
      ok=0;
      if (verbose) out << "theVFatMaptype @" << i << " is not consistent\n";
    }
  }
  for (unsigned int i=0; (verbose || ok) && i<theVFatMapInPos.size(); i++) {
    if (theVFatMapInPos[i].isNotConsistent()) {
      ok=0;
      if (verbose) out << "theVFatMapInPos @" << i << " is not consistent\n";
    }
  }
  return ok;
}

// ------

int GEMEMap::GEMEMapItem::isConsistent() const
{
  return (VFatIDs.size()==positions.size()) ? 1:0;
}


int GEMEMap::GEMEMapItem::isNotConsistent() const
{
  return (VFatIDs.size()!=positions.size()) ? 1:0;
}


void GEMEMap::GEMEMapItem::print(std::ostream &out, int idx) const
{
  if (!isConsistent()) {
    out << "GEMEMapItem::print(" << idx << "): object not consistent. Refusing to print\n";
    return;
  }
  if (static_cast<unsigned int>(idx)>=VFatIDs.size()) {
    out << "GEMEMapItem::print(" << idx << "): idx too large. Refusing to print\n";
  }

  out << "@" << idx << " " << VFatIDs[idx] << " , " << positions[idx];
}


void GEMEMap::GEMEMapItem::printLast(std::ostream &out) const
{
  if (VFatIDs.size()==0) out << "nothing to print" << std::endl;
  print(out,VFatIDs.size()-1);
}

// ------

int GEMEMap::GEMVFatMaptype::isConsistent() const
{
  return (isNotConsistent()==0) ? 1 : 0;
}

int GEMEMap::GEMVFatMaptype::isNotConsistent() const
{
  unsigned int sz= vfat_position.size();
  int bad=0;
  if (z_direction.size()!=sz) bad|=1;
  if (iEta.size()!=sz) bad|=2;
  if (iPhi.size()!=sz) bad|=4;
  if (depth.size()!=sz) bad|=8;
  if (strip_number.size()!=sz) bad|=16;
  if (vfat_chnnel_number.size()!=sz) bad|=32;
  if (vfatId.size()!=sz) bad|=64;
  if (sec.size()!=sz) bad|=128;
  return bad;
}


void GEMEMap::GEMVFatMaptype::print(std::ostream &out, int idx) const
{
  int errCode= isNotConsistent();
  if (errCode!=0) {
    out << "GEMVFatMaptype::print(" << idx << ") is not consistent. Code="
	<< errCode << std::endl;
    return;
  }
  if (static_cast<unsigned int>(idx)>=vfat_position.size()) {
    out << "GEMVFatMaptype::print(" << idx << ") idx too large." << std::endl;
    return;
  }
  out << " vfat_position=" << vfat_position[idx] << ", z_direction=" << z_direction[idx] << ", iEta=" << iEta[idx] << ", iPhi=" << iPhi[idx] << ", depth=" << depth[idx] << ", strip_number=" << strip_number[idx] << ", vfat_chnnel_number=" << vfat_chnnel_number[idx] << ", vfatId=" << std::hex << "0x" << vfatId[idx] << std::dec << ", sec=" << sec[idx] << std::endl;
}


void GEMEMap::GEMVFatMaptype::printLast(std::ostream &out) const
{
  if (vfat_position.size()==0) {
    out << "GEMVFatMaptype::printLast: nothing to print" << std::endl;
    return;
  }
  print(out, vfat_position.size()-1);
}
