/** \file
 *
 *  \author A. Juodagalvis - Vilnius University, Oct 2018
 */

/*
#include "Geometry/GEMGeometryBuilder/plugins/GEMGeometryESModule.h"
#include "Geometry/GEMGeometryBuilder/src/GEMGeometryBuilderFromDDD.h"
#include "Geometry/GEMGeometryBuilder/src/GEMGeometryBuilderFromCondDB.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/MuonNumbering/interface/MuonDDDConstants.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include "Geometry/Records/interface/GEMRecoGeometryRcd.h"
#include "CondFormats/GeometryObjects/interface/RecoIdealGeometry.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include <memory>

using namespace edm;

*/

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "GeometryReaders/XMLIdealGeometryESSource/interface/XMLIdealGeometryESSource.h"
#include "Geometry/GEMGeometry/GEMCosmicStandGeomESSource.h"


GEMCosmicStandGeomESSource::GEMCosmicStandGeomESSource(const edm::ParameterSet & p)
{
  spChmbrNames_ = p.getParameter<std::vector<std::string> >("spChmbrNames");
  setWhatProduced(this);
}

GEMCosmicStandGeomESSource::~GEMCosmicStandGeomESSource(){}

std::unique_ptr<DDCompactView>
GEMCosmicStandGeomESSource::produce()
{
  return XMLIdealGeometryESSource::produce();
}

#include "FWCore/Framework/interface/SourceFactory.h"

DEFINE_FWK_EVENTSETUP_SOURCE(GEMCosmicStandGeomESSource);
