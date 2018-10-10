#ifndef Geometry_GEMGeometry_GEMCosmicStandGeomESSource_h
#define Geometry_GEMGeometry_GEMCosmicStandGeomESSource_h

/** \class GEMCosmicStandGeomESSource
 * 
 *  ESSource for MuonGeometryRecord for GEM Cosmic Stand
 *
 *  \author A. Juodagalvis - Vilnius University, Oct 2018
 */

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "GeometryReaders/XMLIdealGeometryESSource/interface/XMLIdealGeometryESSource.h"

#include <memory>
#include <string>

class GEMCosmicStandGeomESSource : public XMLIdealGeometryESSource
{
 public:
  /// Constructor
  GEMCosmicStandGeomESSource(const edm::ParameterSet & p);
  
  /// Destructor
  ~GEMCosmicStandGeomESSource() override;
  
  /// Produce XMLIdealGeometry record with GEM Cosmic Stand info embeded
  std::unique_ptr<DDCompactView> produce();
  
 private:
  // description of superchambers
  std::vector<std::string> spChmbrNames_;

};
#endif
