#ifndef CondFormats_GEMObjects_GEMQC8Geom_h
#define CondFormats_GEMObjects_GEMQC8Geom_h

#include "CondFormats/Serialization/interface/Serializable.h"
#include <vector>
#include <iostream>

class GEMQC8Geom {

 public:
  GEMQC8Geom(){}
  GEMQC8Geom(const GEMQC8Geom *);
  ~GEMQC8Geom(){}

  int runNo() const { return run_number_; }
  const std::vector<std::string>& chSerNums() const { return chSerialNums_; }
  const std::string& chSerNum(int idx) const { return chSerialNums_.at(idx); }
  const std::vector<std::string>& chPositions() const { return chPositions_; }
  const std::string& chPos(int idx) const { return chPositions_.at(idx); }
  const std::vector<float> chGasFlow() const { return chGasFlow_; }
  float chGasFlow(int idx) const { return chGasFlow_.at(idx); }

  void assign(const GEMQC8Geom&);
  void print(std::ostream &out = std::cout) const;

 public:
  int run_number_;
  std::vector<std::string> chSerialNums_;
  std::vector<std::string> chPositions_;
  std::vector<float> chGasFlow_;

  COND_SERIALIZABLE;
};
#endif
