#include <iostream>

#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/GEMObjects/interface/GEMEMap.h"
#include "CondFormats/DataRecord/interface/GEMEMapRcd.h"

#include <iomanip>

template<class type_t>
void printRange(const char *msg, const std::vector<type_t> &v)
{
  std::cout << msg << " range: ";
  if (v.size()==0) std::cout << "empty";
  else {
    type_t min_v= v[0], max_v=v[0];
    for (unsigned int i=1; i<v.size(); i++) {
      if (v[i]>max_v) max_v=v[i];
      if (v[i]<min_v) min_v=v[i];
    }
    std::cout << min_v << " .. " << max_v << "\n";
  }
  std::cout << "\n";
}


template<class type_t>
void printDiff(const char *msg, const std::vector<type_t> &v)
{
  if (v.size()==0) {
    std::cout << msg << " is empty\n";
    return;
  }

  std::map<type_t,int> vals;
  for (unsigned int i=0; i<v.size(); i++) {
    vals[ v[i] ] ++;
  }
  std::cout << msg << " has " << vals.size() << " different values: ";
  for ( auto x : vals ) {
    std::cout << " " << x.first;
  }
  std::cout << "\n";
}

template<class type_t>
void printAll(const char *msg, const std::vector<type_t> &v)
{
  printDiff(msg,v);
  printRange(msg,v);
}


// class declaration
class GEMEMapDBReader : public edm::EDAnalyzer {
public:
  explicit GEMEMapDBReader( const edm::ParameterSet& );
  ~GEMEMapDBReader() override;
  void analyze( const edm::Event&, const edm::EventSetup& ) override;
private:
  bool m_flag;
};

GEMEMapDBReader::GEMEMapDBReader( const edm::ParameterSet& iConfig ){}

GEMEMapDBReader::~GEMEMapDBReader(){}

void GEMEMapDBReader::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {

  std::cout << "====== GEMEMapDBReader" << std::endl;

  edm::ESHandle<GEMEMap> readoutMapping;
  iSetup.get<GEMEMapRcd>().get(readoutMapping);
  const GEMEMap* eMap=readoutMapping.product();
  std::cout << eMap << std::endl;
  //if(eMap) return;

  std::cout << "version: " << eMap->version() << " and " << eMap->theVFatMaptype.size() << std::endl;

  // loop on the GEMEMap elements
  // for (std::vector<GEMEMapItem>::const_iterator i=eMap->theEMapItem.begin(); i<eMap->theEMapItem.end();i++){

  //   std::cout <<" Gem Map  Chamber="<<i->ChamberID<<std::endl;

  //   std::vector<int>::const_iterator p=i->positions.begin();
  //   for (std::vector<int>::const_iterator v=i->VFatIDs.begin(); v<i->VFatIDs.end();v++,p++){
  //     std::cout <<" Position="<<std::setw(2)<<std::setfill('0')<<*p
  // 		<<"  vfat=0x"<<std::setw(4)<<std::setfill('0')<<std::hex<<*v<<std::dec<<std::endl;
  //   }

  //   std::vector<GEMEMap::GEMVFatMapInPos>::const_iterator ipos;
  //   for (ipos=eMap->theVFatMapInPos.begin();ipos<eMap->theVFatMapInPos.end();ipos++){
  //     std::cout <<" In position "<<std::setw(2)<<std::setfill('0')<<ipos->position
  // 		<<" I have this map_id "<<ipos->VFATmapTypeId<<std::endl;
  //   }

  if (1) { // plain print
   std::vector<GEMEMap::GEMVFatMaptype>::const_iterator imap;
  for (imap=eMap->theVFatMaptype.begin(); imap<eMap->theVFatMaptype.end();imap++){

    std::cout <<"  Map TYPE "<<imap->VFATmapTypeId<<std::endl;
    //std::cout << "size=" << imap->strip_number.size() << std::endl;
    for (unsigned int ix=0;ix<imap->strip_number.size();ix++){
      std::cout <<
    	" z_direction "<<imap->z_direction[ix] <<
    	" iEta "<<imap->iEta[ix] <<
    	" iPhi "<<imap->iPhi[ix] <<
    	" depth "<<imap->depth[ix] <<
    	" vfat_position "<<imap->vfat_position[ix] <<
    	" strip_number "<<imap->strip_number[ix] <<
    	" vfat_chnnel_number "<<imap->vfat_chnnel_number[ix]<<std::endl;
    }
  }
  }
  else {
    // brute-force sorted print
    int cnt=0;
    for (auto imap : eMap->theVFatMaptype ) {

      std::cout <<"  Map TYPE "<<imap.VFATmapTypeId<<std::endl;
      if (1) {
	printAll("z_dir",imap.z_direction);
	printAll("sector",imap.sec);
	printAll("depth",imap.depth);
	printAll("ieta",imap.iEta);
	printAll("iphi",imap.iPhi);
	printAll("vfatPos",imap.vfat_position);
	printAll("vfatChan",imap.vfat_chnnel_number);
	//continue;
      }

      for (int zdir=-1; zdir<=1; zdir+=2) {
	if (zdir>0) continue;
	for (int isec=1; isec<=30; isec++) {
	  if ((isec>1) && (isec<27)) continue;
	 for (int depth=1; depth<=2; depth++) {
	   std::cout << "zdir=" << zdir << ", sector=" << isec << ", depth=" << depth << "\n";
	   for (int ieta=1; ieta<=8; ieta++) {
	     for (int iphi=1; iphi<=90; iphi++) {
	       if ((iphi>4) && (iphi<78)) continue;
	       for (int vfatPos=0; vfatPos<=24; vfatPos++) {
		 for (int vfatChan=0; vfatChan<=128; vfatChan++) {
		   for (unsigned int ix=0;ix<imap.strip_number.size();ix++){
		     if ((imap.z_direction[ix]!=zdir) ||
			 (imap.sec[ix]!=isec) ||
			 (imap.depth[ix]!=depth) ||
			 (imap.iEta[ix]!=ieta) ||
			 (imap.iPhi[ix]!=iphi) ||
			 (imap.vfat_position[ix]!=vfatPos) ||
			 (imap.vfat_chnnel_number[ix]!=vfatChan))
			 continue;
		     cnt++;
		     if (1)
		     std::cout << cnt <<
		       " z_direction "<<imap.z_direction[ix] <<
		       " iEta "<<imap.iEta[ix] <<
		       " iPhi "<<imap.iPhi[ix] <<
		       " depth "<<imap.depth[ix] <<
		       " vfat_position "<<imap.vfat_position[ix] <<
		       " strip_number "<<imap.strip_number[ix] <<
		       " vfat_chnnel_number "<<imap.vfat_chnnel_number[ix]<<
		       " vfat_id 0x"<<std::hex << imap.vfatId[ix]<<std::dec<<
		       " sector " << imap.sec[ix] << "\n";
		   }
		 }
	       }
	     }
	   }
	 }
       }
     }
   }
   std::cout << "printed  " << cnt << " lines" << std::endl;
   std::cout << std::endl;
  }
  //}
  
}

//define this as a plug-in
DEFINE_FWK_MODULE(GEMEMapDBReader);
