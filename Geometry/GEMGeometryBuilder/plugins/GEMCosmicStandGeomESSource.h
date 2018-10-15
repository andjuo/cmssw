#ifndef Geometry_GEMGeometryBuilder_plugins_GEMCosmicStandGeomESSource_h
#define Geometry_GEMGeometryBuilder_plugins_GEMCosmicStandGeomESSource_h

/** \class GEMCosmicStandGeomESSource
 * 
 *  ESSource for MuonGeometryRecord for GEM Cosmic Stand
 *
 *  \author A. Juodagalvis - Vilnius University, Oct 2018
 */
// Based on GeometryReaders/XMLIdealGeometryESSource/interface/XMLIdealGeometryESSource.h

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "GeometryReaders/XMLIdealGeometryESSource/interface/XMLIdealGeometryESSource.h"
#include "GeometryReaders/XMLIdealGeometryESSource/interface/GeometryConfiguration.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

#include "CondTools/GEM/interface/GEMQC8ConfSourceHandler.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Conf.h"
//#include "CondFormats/DataRecord/interface/GEMQC8ConfRcd.h"

#include <memory>
#include <string>

class GEMCosmicStandGeomESSource : public edm::ESProducer, 
                                   public edm::EventSetupRecordIntervalFinder
// public XMLIdealGeometryESSource
{
 public:
  GEMCosmicStandGeomESSource(const edm::ParameterSet & p);
  ~GEMCosmicStandGeomESSource() override;

  /// Produce XMLIdealGeometry record with GEM Cosmic Stand info embeded
  std::unique_ptr<DDCompactView> produceGeom(const IdealGeometryRecord &);
  std::unique_ptr<DDCompactView> produce();

  void addGeoFile(const std::string &rel_fname);

protected:
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
		      const edm::IOVSyncValue &,edm::ValidityInterval &) override;
  //GEMCosmicStandGeomESSource(const GEMCosmicStandGeomESSource &) = delete;
  //const GEMCosmicStandESSource & operator=(const GEMCosmicStandGeomESSource &) = delete;

private:
  // description of superchambers
  std::vector<std::string> spChmbrNames_;
  bool useDB_; // whether the chambers should be read from a DB
  popcon::GEMQC8ConfSourceHandler confSrcHandler_;

  // from XMLIdealGeometryESSource
  std::string rootNodeName_;
  bool userNS_;
  GeometryConfiguration geoConfig_;

};
#endif
