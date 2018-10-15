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
//#include "CondFormats/DataFormats/interface/GEMQC8ConfRcd.h"
#include "CondFormats/GEMObjects/interface/GEMQC8Conf.h"
#include <iostream>
#include <sstream>
#include <algorithm>


GEMCosmicStandGeomESSource::GEMCosmicStandGeomESSource(const edm::ParameterSet & p) :
  spChmbrNames_(p.getUntrackedParameter<std::vector<std::string> >("spChmbrNames")),
  useDB_(p.getUntrackedParameter<bool>("useDB", false)),
  confSrcHandler_(p.getUntrackedParameter<edm::ParameterSet>("DBSource")),
  rootNodeName_(p.getParameter<std::string>("rootNodeName")),
  userNS_(p.getUntrackedParameter<bool>("userControlledNamespace", false)),
  geoConfig_(p)
// : XMLIdealGeometryESSource(p)
{
  std::cout << "spChmbrNames=";
  for (unsigned int i=0; i<spChmbrNames_.size(); i++)
    std::cout << " " << spChmbrNames_[i];
  std::cout << std::endl;
  std::cout << "useDB=" << useDB_ << std::endl;

  //setWhatProduced(this, &GEMCosmicStandGeomESSource::produceGeom, edm::es::Label(p.getParameter<std::string>("@module_label")));
  setWhatProduced(this, &GEMCosmicStandGeomESSource::produceGeom, edm::es::Label()); //p.getParameter<std::string>("@module_label")));
  findingRecord<IdealGeometryRecord>();
}

GEMCosmicStandGeomESSource::~GEMCosmicStandGeomESSource(){}

std::unique_ptr<DDCompactView>
GEMCosmicStandGeomESSource::produceGeom(const IdealGeometryRecord &)
{
  std::cout << "\n\ncalled GEMCosmicStandGeomESSource::produceGeom\n\n";

  if (useDB_) {
    std::cout << "GEMCosmicStandGeomESSource: calling getNewObjects" << std::endl;
    confSrcHandler_.setOnlyConfDef(1);  // 1 - only chambers, no electronics
    confSrcHandler_.getNewObjects();
    std::cout << "GEMCosmicStandGeomESSource: getNewObjects done" << std::endl;
    if (!confSrcHandler_.getQC8conf()) {
      std::cout << "failed to get qc8conf" << std::endl;
    }
    else {
      //confSrcHandler_.editQC8conf()->sortByPos();
      const std::vector<std::string> *chNames= & confSrcHandler_.getQC8conf()->chSerNums();
      const std::vector<std::string> *chPos = & confSrcHandler_.getQC8conf()->chPositions();
      spChmbrNames_ = *chNames;
      std::vector<int> loaded;
      std::vector<char> chSize;
      loaded.reserve(15); chSize.reserve(15);
      for (unsigned int i=0; i<chNames->size(); i++) {
	char chamberSize='0';
	if (chNames->at(i).find('L') != std::string::npos) chamberSize='L';
	if (chNames->at(i).find('S') != std::string::npos) chamberSize='S';
	if (chamberSize == '0') continue;

	std::stringstream ss(chPos->at(i));
	int ir=0, ic=0;
	char c1;
	ss >> ir >> c1 >> ic;
	std::cout << "from " << chPos->at(i) << " " << ss.str() << ", got ir=" << ir << ", ic=" << ic << "\n";
	std::vector<int>::const_iterator it = std::find(loaded.begin(),loaded.end(),ir*10+ic);
	if (it != loaded.end()) {
	  int idx=(it-loaded.begin());
	  if (chSize.at(idx)!=chamberSize) {
	    std::cout << "different chamber size! at " << ir << "," << ic << std::endl;
	  }
	  continue;
	}
	else {
	  loaded.push_back(ir*10+ic);
	  chSize.push_back(chamberSize);
	  std::stringstream sout;
	  sout << "Geometry/MuonCommonData/data/GEMQC8/gem11" << chamberSize
	       << "_c" << ic << "_r" << ir << ".xml";
	  std::cout << "GEMCosmicStandGeomESSource::produceGeom: adding file " << ss.str() << std::endl;
	  geoConfig_.addFile(ss.str());
	}
      }
    }
  }
  else {
    // based on spChmbrNames variable
    for (unsigned int i=0; i<spChmbrNames_.size(); i++) {
      char chamberSize='0';
      if (spChmbrNames_.at(i).find('L') != std::string::npos) chamberSize='L';
      if (spChmbrNames_.at(i).find('S') != std::string::npos) chamberSize='S';
      if (chamberSize == '0') continue;

      std::stringstream ss;
      ss << "Geometry/MuonCommonData/data/GEMQC8/gem11" << chamberSize
	 << "_c" << (i/5+1) << "_r" << (i%5+1) << ".xml";
      std::cout << "GEMCosmicStandGeomESSource::produceGeom: adding file " << ss.str() << std::endl;
      geoConfig_.addFile(ss.str());
    }
  }

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
