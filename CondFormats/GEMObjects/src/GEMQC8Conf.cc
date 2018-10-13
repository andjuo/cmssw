#include "CondFormats/GEMObjects/interface/GEMQC8Conf.h"
#include "FWCore/Utilities/interface/Exception.h"

GEMQC8Conf::GEMQC8Conf(const GEMQC8Conf *g)
{
  if (!g) return;
  run_number_ = g->run_number_;
  chSerialNums_ = g->chSerialNums_;
  chPositions_ = g->chPositions_;
  chGasFlow_ = g->chGasFlow_;
}

void GEMQC8Conf::assign(const GEMQC8Conf &g)
{
  run_number_ = g.run_number_;
  chSerialNums_ = g.chSerialNums_;
  chPositions_ = g.chPositions_;
  chGasFlow_ = g.chGasFlow_;
}

void GEMQC8Conf::print(std::ostream &out) const
{
  out << "GEMQC8Conf for run_number=" << run_number_ << "\n";
  unsigned int sz= chSerialNums_.size();
  if ((sz!=chPositions_.size()) || (sz!=chGasFlow_.size())) {
    out << " different sizes of arrays: chSerialNums[" << sz
	<< "], chPositions[" << chPositions_.size() << "], chGasFlow["
	<< chGasFlow_.size() << "]\n";
  }
  else {
    for (unsigned int i=0; i<chSerialNums_.size(); i++) {
      out << " " << chPositions_.at(i) << " " << chSerialNums_.at(i) << "  "
	  << chGasFlow_.at(i) << "\n";
    }
  }
}
