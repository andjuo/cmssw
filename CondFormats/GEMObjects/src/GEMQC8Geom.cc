#include "CondFormats/GEMObjects/interface/GEMQC8Geom.h"
#include "FWCore/Utilities/interface/Exception.h"

void GEMQC8Geom::print(std::ostream &out) const
{
  out << "GEMQC8Geom for run_number=" << run_number_ << "\n";
  for (unsigned int i=0; i<chSerialNums_.size(); i++) {
    out << " " << chPositions_.at(i) << " " << chSerialNums_.at(i) << "  "
	<< chGasFlow_.at(i) << "\n";
  }
}
