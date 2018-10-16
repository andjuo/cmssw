#include "CondFormats/GEMObjects/interface/GEMELMap.h"
#include "CondFormats/GEMObjects/interface/GEMROmap.h"
#include "DataFormats/MuonDetId/interface/GEMDetId.h"

GEMELMap::GEMELMap():
  theVersion("") {}

GEMELMap::GEMELMap(const std::string & version):
  theVersion(version) {}

GEMELMap::~GEMELMap() {}

const std::string & GEMELMap::version() const{
  return theVersion;
}

void GEMELMap::convert(GEMROmap & romap) {

  for (auto imap : theVFatMap_){
    for (unsigned int ix=0;ix<imap.vfatId.size();ix++){
      GEMROmap::eCoord ec;
      ec.vfatId = imap.vfatId[ix] & chipIdMask_;
      ec.gebId = imap.gebId[ix];
      ec.amcId = imap.amcId[ix];      

      int st = std::abs(imap.z_direction[ix]);
      GEMROmap::dCoord dc;
      dc.gemDetId = GEMDetId(imap.z_direction[ix], 1, st, imap.depth[ix], imap.sec[ix], imap.iEta[ix]);
      dc.vfatType = imap.vfatType[ix]; 
      dc.iPhi = imap.iPhi[ix];

      romap.add(ec,dc);
      romap.add(dc,ec);
    }
  }
  
  for (auto imap : theStripMap_){
    for (unsigned int ix=0;ix<imap.vfatType.size();ix++){
      GEMROmap::channelNum cMap;
      cMap.vfatType = imap.vfatType[ix];
      cMap.chNum = imap.vfatCh[ix];

      GEMROmap::stripNum sMap;
      sMap.vfatType = imap.vfatType[ix];
      sMap.stNum = imap.vfatStrip[ix];

      romap.add(cMap, sMap);
      romap.add(sMap, cMap);
    }
  }
}

void GEMELMap::convertDummy(GEMROmap & romap) {
  // 12 bits for vfat, 5 bits for geb, 8 bit long GLIB serial number
  uint16_t amcId = 1; //amc
  uint16_t gebId = 0; 
	
  for (int re = -1; re <= 1; re = re+2) {
    for (int st = GEMDetId::minStationId; st<=GEMDetId::maxStationId; ++st) {
      int maxVFat = maxVFatGE11_;
      if (st == 2) maxVFat = maxVFatGE21_;      
      if (st == 0) maxVFat = maxVFatGE0_;
      
      for (int ch = 1; ch<=GEMDetId::maxChamberId; ++ch) {
	for (int ly = 1; ly<=GEMDetId::maxLayerId; ++ly) {
	  // 1 geb per chamber
	  gebId++;	  	  	  
	  uint16_t chipId = 0;	 	  
	  for (int roll = 1; roll<=GEMDetId::maxRollId; ++roll) {
	    
	    GEMDetId gemId(re, 1, st, ly, ch, roll);

	    for (int nphi = 1; nphi <= maxVFat; ++nphi){
	      chipId++;
	      
	      GEMROmap::eCoord ec;
	      ec.vfatId = chipId;
	      ec.gebId = gebId;
	      ec.amcId = amcId;

	      GEMROmap::dCoord dc;
	      dc.gemDetId = gemId;
	      dc.vfatType = 1;
	      dc.iPhi = nphi;

	      romap.add(ec,dc);
	      romap.add(dc,ec);
	    }
	  }
	  // 5 bits for geb
	  if (gebId == maxGEBs_){
	    // 24 gebs per amc
	    gebId = 0;
	    amcId++;
	  }
	}
      }
    }
  }

  for (int i = 0; i < maxChan_; ++i){
    // only 1 vfat type for dummy map
    GEMROmap::channelNum cMap;
    cMap.vfatType = 1;
    cMap.chNum = i;

    GEMROmap::stripNum sMap;
    sMap.vfatType = 1;
    sMap.stNum = i+1;

    romap.add(cMap, sMap);
    romap.add(sMap, cMap);
  }
}


void GEMELMap::print(std::ostream &out, int detailed, int checkArrays) const
{
  out << "GEMELMap: theVFATMap[" << theVFatMap_.size() << "], theStripMap["
      << theStripMap_.size() << "]\n";
  if (detailed) {
    out << "  theVFATMap[" << theVFatMap_.size() << "]:\n";
    for (unsigned int i=0; i<theVFatMap_.size(); i++) {
      const GEMVFatMap *ptr= & theVFatMap_.at(i);
      unsigned int sz= ptr->vfat_position.size();
      out << " i=" << i << ", VFATmapTypeId=" << ptr->VFATmapTypeId << ", sz="
	  << sz << "\n";
      int ok=1;
      if (checkArrays) {
	if ((sz!=ptr->z_direction.size()) || (sz!=ptr->iEta.size()) || (sz!=ptr->iPhi.size()) || (sz!=ptr->depth.size()) || (sz!=ptr->vfatType.size()) || (sz!=ptr->vfatId.size()) || (sz!=ptr->amcId.size()) || (sz!=ptr->gebId.size()) || (sz!=ptr->sec.size())) {
	  out << "GEMVFatMap size error : z_direction[" << ptr->z_direction.size() << "], iEta[" << ptr->iEta.size() << "], iPhi[" << ptr->iPhi.size() << "], depth[" << ptr->depth.size() << "], vfatType[" << ptr->vfatType.size() << "], vfatId[" << ptr->vfatId.size() << "], amcId[" << ptr->amcId.size() << "], gebId[" << ptr->gebId.size() << "], sec[" << ptr->sec.size() << "\n";
	  ok=0;
	}
      }
      if (ok) {
	for (unsigned int ii=0; ii<sz; ii++) {
	  out << " " << ii << " vfat_pos=" << ptr->vfat_position[ii];
	  if (ii < ptr->z_direction.size()) out << " z_dir=" << ptr->z_direction[ii];
	  if (ii < ptr->iEta.size()) out << " iEta=" << ptr->iEta[ii];
	  if (ii < ptr->iPhi.size()) out << " iPhi=" << ptr->iPhi[ii];
	  if (ii < ptr->depth.size()) out << " depth=" << ptr->depth[ii];
	  if (ii < ptr->vfatType.size()) out << " vfatType=" << ptr->vfatType[ii];
	  if (ii < ptr->vfatId.size()) out << " vfatId=0x" << std::hex << ptr->vfatId[ii] << std::dec;
	  if (ii < ptr->amcId.size()) out << " amcId=" << ptr->amcId[ii];
	  if (ii < ptr->gebId.size()) out << " gebId=" << ptr->gebId[ii];
	  if (ii < ptr->sec.size()) out << " sect=" << ptr->sec[ii];
	  out << "\n";
	}
      }
    } // for theVFatMap_

    out << " theStripMap[" << theStripMap_.size() << "]:\n";
    for (unsigned int i=0; i<theStripMap_.size(); i++) {
      const GEMStripMap *ptr = & theStripMap_.at(i);
      unsigned int sz= ptr->vfatType.size();
      out << " i=" << i << ", sz=" << sz << "\n";
      int ok=1;
      if (checkArrays) {
	if ((sz!=ptr->vfatCh.size()) || (sz!=ptr->vfatStrip.size())) {
	  out << "GEMStripMap size error : vfatType[" << ptr->vfatType.size() << "], vfatCh[" << ptr->vfatCh.size() << "], vfatStrip[" << ptr->vfatStrip.size() << "]\n";
	  ok=0;
	}
      }
      if (ok) {
	for (unsigned int ii=0; ii<sz; ii++) {
	  out << " " << ii << " vfatType=" << ptr->vfatType[ii]
	      << " vfatCh=" << ptr->vfatCh[ii]
	      << " vfatStrip=" << ptr->vfatStrip[ii] << "\n";
	}
      }
    } // for theStripMap
  } // if (detailed)
}
