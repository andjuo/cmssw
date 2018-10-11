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
#include "FWCore/Framework/interface/ESProducer.h"

#include <memory>

using namespace edm;

*/

#include "DetectorDescription/Parser/interface/DDLParser.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"
#include "DetectorDescription/Core/interface/DDRoot.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "GeometryReaders/XMLIdealGeometryESSource/interface/XMLIdealGeometryESSource.h"
#include "Geometry/GEMGeometryBuilder/plugins/GEMCosmicStandGeomESSource.h"
#include <iostream>


GEMCosmicStandGeomESSource::GEMCosmicStandGeomESSource(const edm::ParameterSet & p) :
  spChmbrNames_(p.getParameter<std::vector<std::string> >("spChmbrNames")),
  rootNodeName_(p.getParameter<std::string>("rootNodeName")),
  userNS_(p.getUntrackedParameter<bool>("userControlledNamespace", false)),
  geoConfig_(p)
// : XMLIdealGeometryESSource(p)
{
  std::cout << "spChmbrNames=";
  for (unsigned int i=0; i<spChmbrNames_.size(); i++)
    std::cout << " " << spChmbrNames_[i];
  std::cout << std::endl;

  //setWhatProduced(this, &GEMCosmicStandGeomESSource::produceGeom, edm::es::Label(p.getParameter<std::string>("@module_label")));
  setWhatProduced(this, &GEMCosmicStandGeomESSource::produceGeom, edm::es::Label()); //p.getParameter<std::string>("@module_label")));
  findingRecord<IdealGeometryRecord>();
}

GEMCosmicStandGeomESSource::~GEMCosmicStandGeomESSource(){}

std::unique_ptr<DDCompactView>
GEMCosmicStandGeomESSource::produceGeom(const IdealGeometryRecord &)
{
  std::cout << "\n\ncalled GEMCosmicStandGeomESSource::produceGeom\n\n";
  return GEMCosmicStandGeomESSource::produce();
}

std::unique_ptr<DDCompactView>
GEMCosmicStandGeomESSource::produce()
{
  std::cout << "\n\ncalled GEMCosmicStandGeomESSource::produce\n\n";
  //return XMLIdealGeometryESSource::produce();

  DDName ddName(rootNodeName_);
  DDLogicalPart rootNode(ddName);
  DDRootDef::instance().set(rootNode);
  std::unique_ptr<DDCompactView> returnValue(new DDCompactView(rootNode));
  DDLParser parser(*returnValue); //* parser = DDLParser::instance();
  parser.getDDLSAX2FileHandler()->setUserNS(userNS_);
  int result2 = parser.parse(geoConfig_);
  if (result2 != 0) throw cms::Exception("DDException") << "DDD-Parser: parsing failed!";

  // after parsing the root node should be valid!

  if( !rootNode.isValid() ){
    throw cms::Exception("Geometry")<<"GEMCosmicStandGeomESSource: "
				    << "There is no valid node named \""
                                    <<rootNodeName_<<"\"";
  }
  returnValue->lockdown();
  return returnValue;
}

void GEMCosmicStandGeomESSource::setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
                                              const edm::IOVSyncValue & iosv, 
                                              edm::ValidityInterval & oValidity)
{
  edm::ValidityInterval infinity(iosv.beginOfTime(), iosv.endOfTime());
  oValidity = infinity;
}


void GEMCosmicStandGeomESSource::addGeoFile(const std::string &rel_fname)
{ geoConfig_.addFile(rel_fname); }

#include "FWCore/Framework/interface/SourceFactory.h"

DEFINE_FWK_EVENTSETUP_SOURCE(GEMCosmicStandGeomESSource);
