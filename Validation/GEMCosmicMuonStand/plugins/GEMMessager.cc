#ifndef GEMMessager_H
#define GEMMessager_H

/** \class GEMMessager
 *
 *  An EDAnalyzer to print a specified message
 *
 *  \author A. Juodagalvis - Vilnius University
 *  Sep 2018
 */

//#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <iostream>
#include <string>

//using namespace std;

class GEMMessager : public edm::EDAnalyzer
{
public:
explicit GEMMessager( const edm::ParameterSet& );
void analyze(const edm::Event& e, const edm::EventSetup&) override;

private:
  std::string message;
  int printMsg;
};

GEMMessager::GEMMessager(const edm::ParameterSet & cfg)
{
  message = cfg.getParameter<std::string>("message");
  printMsg = cfg.getParameter<int>("printMsg");
  std::cout << "GEMMessager constructor: printMsg=" << printMsg
	    << "msg=" << message << std::endl;
}

void GEMMessager::analyze(const edm::Event& e, const edm::EventSetup&)
{ if (printMsg) std::cout << "GEMMessager " << message << std::endl; }


DEFINE_FWK_MODULE (GEMMessager) ;
#endif

