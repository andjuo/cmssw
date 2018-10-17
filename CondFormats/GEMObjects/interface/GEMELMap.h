#ifndef CondFormats_GEMObjects_GEMELMap_h
#define CondFormats_GEMObjects_GEMELMap_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include <string>
#include <vector>
#include <iostream>

class GEMROmap;

class GEMELMap {
 public:
  GEMELMap();
  GEMELMap(const GEMELMap *ptr);
  explicit GEMELMap(const std::string & version);

  virtual ~GEMELMap();

  const std::string & version() const;
  void convert(GEMROmap & romap);
  void convertDummy(GEMROmap & romap);
  void print(std::ostream &out = std::cout, int detailed=0, int checkArrays=0) const;

  struct GEMVFatMap {
    int VFATmapTypeId;
    std::vector<int> vfat_position;
    std::vector<int> z_direction;
    std::vector<int> iEta;
    std::vector<int> iPhi;
    std::vector<int> depth;
    std::vector<int> vfatType;
    std::vector<uint16_t> vfatId;
    std::vector<uint16_t> amcId;
    std::vector<uint16_t> gebId;
    std::vector<int> sec; 

    unsigned int size() const { return vfat_position.size(); }

    COND_SERIALIZABLE;
  };
  struct GEMStripMap {
    std::vector<int> vfatType;
    std::vector<int> vfatCh;
    std::vector<int> vfatStrip;
 
    unsigned int size() const { return vfatType.size(); }

    COND_SERIALIZABLE;
  };

  std::vector<GEMVFatMap>  theVFatMap_;
  std::vector<GEMStripMap> theStripMap_;
  
 private:
  std::string theVersion;

  COND_SERIALIZABLE;
  
 public:
  // size of ID bits
  static const int chipIdBits_ = 12;     // ID size from VFat
  static const int chipIdMask_ = 0xfff;  // chipId mask for 12 bits
  static const int gebIdBits_  = 5;      // ID size from GEB
  static const int maxGEBs_    = 24;     // 24 gebs per amc
  static const int maxVFatGE0_ = 12;     // vFat per eta partition, not known yet for ME0
  static const int maxVFatGE11_= 3;      // vFat per eta partition in GE11
  static const int maxVFatGE21_= 6;      // vFat per eta partition in GE21
  static const int maxChan_    = 128;    // channels per vFat
  static const int amcBX_      = 25;     // amc BX to get strip bx
  //static const uint16_t noValUInt16_ = uint16_t(-1); // "incorrect" value
};
#endif // GEMELMap_H
