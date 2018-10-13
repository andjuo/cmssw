#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "FWCore/Utilities/interface/Exception.h"

GEMQC8Geom::GEMQC8Geom(const GEMQC8Geom *g)
{
  if (!g) return;
  run_number_ = g->run_number_;
  chSerialNums_ = g->chSerialNums_;
  chGasFlow_ = g->chGasFlow_;
}

void GEMQC8Geom::assign(const GEMQC8Geom &g)
{
  run_number_ = g.run_number_;
  chSerialNums_ = g.chSerialNums_;
  chGasFlow_ = g.chGasFlow_;
}

void GEMQC8Geom::print(std::ostream &out) const
{
  out << "GEMQC8Geom for run_number=" << run_number_ << "\n";
  for (unsigned int i=0; i<chSerialNums_.size(); i++) {
    out << " " << chPositions_.at(i) << " " << chSerialNums_.at(i) << "  "
	<< chGasFlow_.at(i) << "\n";
  }
}
