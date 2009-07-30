#ifndef BooHighMAnalyzer_H
#define BooHighMAnalyzer_H

/** \class BooHighMAnalyzer
 *
 *  Author: Francisco Yumiceva
 */

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "DataFormats/PatCandidates/interface/PATObject.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/Lepton.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "TopQuarkAnalysis/TopPairBSM/interface/BooHistograms.h"
#include "TopQuarkAnalysis/TopPairBSM/interface/JetCombinatorics.h"
#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"

#include "TFile.h"
#include "TLorentzVector.h"
#include <vector>
#include <map>
#include <string>
#include <utility> 
#include <fstream>

struct partonMaxPt
{
  bool operator()(const reco::Candidate *c1, const reco::Candidate *c2) const
  {
    return c1->pt() >= c1->pt();
  }
};


class BooHighMAnalyzer : public edm::EDAnalyzer {

  public:


    /// Constructor
    BooHighMAnalyzer(const edm::ParameterSet& pset);

    /// Destructor
    virtual ~BooHighMAnalyzer();

    /// Perform the real analysis
    void analyze(const edm::Event & iEvent, const edm::EventSetup& iSetup);

    /// 3d angles
	double Psi(TLorentzVector p1, TLorentzVector p2, double mass);
	double dij(TLorentzVector p1, TLorentzVector p2, double mass, bool min = true);
	double PtRel(TLorentzVector p, TLorentzVector paxis);
	
	typedef math::XYZTLorentzVector LorentzVector;

	/// Jet to parton matching
	bool IsTruthMatch( Combo acombo, const edm::View<pat::Jet> jets,  TtGenEvent genEvt, bool MatchFlavor = false);

  private:


    // Histogram containers
	BooHistograms *hcounter;
	BooHistograms *hmuons_;
	BooHistograms *helectrons_;
	BooHistograms *hmet_;
	BooHistograms *hjets_;
	BooHistograms *hgen_;
	BooHistograms *hmass_;
	BooHistograms *hdisp_;
	
	std::map<TString, TString> cut_map;
	    
    // The file which will store the histos
    TFile *theFile;
    // ascii outpur filename
	std::ofstream fasciiFile;

	JetCombinatorics myCombi_;
	JetCombinatorics myCombi0_;
	JetCombinatorics myCombi2_;
	JetCombinatorics myCombi3_;

	
    // configuration
	bool fwriteAscii;// flag to dump ASCII file
    std::string fasciiFileName; // ASCII filename
    // csa07 weights
    bool fApplyWeights;
    bool fIsMCTop;

    // verbose
    bool debug;
	bool fdisplayJets; // make lego plots
    int feventToProcess;
	
    std::string rootFileName;
	edm::InputTag genEvnSrc;
    edm::InputTag muonSrc;
    edm::InputTag electronSrc;
    edm::InputTag metSrc;
    edm::InputTag jetSrc;
	edm::InputTag jetSrc1;
    edm::InputTag jetSrc2;

    edm::InputTag evtsols;

	bool fUsebTagging;
	
	int nevents;
	int nbadmuons;
	int nWcomplex;
	int MCAllmatch_chi2_;
	int MCAllmatch_sumEt_;
		
	double fMinMuonPt;
	double fMaxMuonEta;
	double fMuonRelIso;
	double fMaxMuonEm;
	double fMaxMuonHad;
	
	double fMinElectronPt;
	double fMaxElectronEta;
	double fElectronRelIso;
	
	double fMinLeadingJetPt;
	double fMinJetPt;
	double fMaxJetEta;
	double fMinHt;
	double fMinMET;
	bool fUseMyMET;
	bool fApplyJetAsymmetricCuts;
};


#endif
