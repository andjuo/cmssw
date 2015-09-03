// -*- C++ -*-
//
// Package:    Calibration/HcalAlCaRecoProducers/AlCaGammaJetSelector
// Class:      AlCaGammaJetSelector
//
/**\class AlCaGammaJetSelector AlCaGammaJetSelector.cc Calibration/HcalAlCaRecoProducers/AlCaGammaJetSelector/src/AlCaGammaJetSelector.cc

 Description: Enable filtering of good events based on the AlCaGammaJetProducer info

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrius Juodagalvis
//         Created:  Fri, 15 Aug 2015 12:09:55 GMT
//
//


// system include files
#include <memory>
#include <iostream>
#include <sstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

//#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include <TH1D.h>

//
// class declaration
//

class AlCaGammaJetSelector : public edm::EDFilter {
public:
  typedef enum { _nProc=0, _nPhoCollOk, _nJetCollOk,
		 _nOnePhoOk, _nOneJetOk,
		 _nPhoPtOk, _nVertexHandleOk, _nVerticesOk, _nJetPtOk,
		 _nDPhiOk,
		 _nHoEOk, _nSieieHandleOk, _nSieieOk,
		 _nRhoHandleOk, _nEgmIsoHandleOk, _nIsoOk,
		 _nSelectionOk,
		 _nLastFlag
  } TSelectionFlag_t;
  typedef enum { _idxCHIso=0, _idxNHIso, _idxPhIso } TIsoType_t;
public:
      explicit AlCaGammaJetSelector(const edm::ParameterSet&);
      ~AlCaGammaJetSelector();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------

  ULong_t nProcessed_, nSelected_;

  // minimal requirements
  edm::InputTag labelPhoton_, labelPFJet_, labelVertexColl_;
  double minPtJet_, minPtPhoton_;
  double minDPhiPhoJet_;

  bool applyPhoQualityCuts_;
  // collections
  edm::InputTag labelRhoColl_;
  edm::InputTag labelPhoSieieColl_;
  edm::InputTag labelPhoChIsoColl_, labelPhoNhIsoColl_, labelPhoPhIsoColl_;

  // index 0 - barrel, 1 - endcap
  double phoMaxHE_[2], phoMaxSieie_[2];
  double phoMaxChIso_[2], phoMaxNhIso_[2], phoMaxPhIso_[2];
  std::vector<double> phoEffAreas_barrelV_, phoEffAreas_endcapV_;

  edm::EDGetTokenT<reco::PhotonCollection> tok_Photon_;
  edm::EDGetTokenT<reco::PFJetCollection> tok_PFJet_;
  edm::EDGetTokenT<reco::VertexCollection> tok_vertices_;
  edm::EDGetTokenT<double> tok_rho_;
  edm::EDGetTokenT<edm::ValueMap<float> > tok_phoSieieMap_;
  edm::EDGetTokenT<edm::ValueMap<float> > tok_phoChIsoMap_, tok_phoNhIsoMap_, tok_phoPhIsoMap_;

  TH1D* h1SelFlag;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
AlCaGammaJetSelector::AlCaGammaJetSelector(const edm::ParameterSet& iConfig)
{
  nProcessed_ = 0;
  nSelected_ = 0;

  // get input
  labelPhoton_     = iConfig.getParameter<edm::InputTag>("PhoInput");
  labelPFJet_      = iConfig.getParameter<edm::InputTag>("PFjetInput");
  labelVertexColl_ = iConfig.getParameter<edm::InputTag>("VertexCollection");
  minPtJet_        = iConfig.getParameter<double>("MinPtJet");
  minPtPhoton_     = iConfig.getParameter<double>("MinPtPhoton");
  minDPhiPhoJet_   = iConfig.getParameter<double>("MinDPhiPhoJet");

  applyPhoQualityCuts_ = iConfig.getParameter<bool>("ApplyPhoQualityCuts");
  labelRhoColl_      = iConfig.getParameter<edm::InputTag>("RhoCollection");
  labelPhoSieieColl_ = iConfig.getParameter<edm::InputTag>("PhoSieieCollection");
  labelPhoChIsoColl_ = iConfig.getParameter<edm::InputTag>("PhoChIsoCollection");
  labelPhoNhIsoColl_ = iConfig.getParameter<edm::InputTag>("PhoNhIsoCollection");
  labelPhoPhIsoColl_ = iConfig.getParameter<edm::InputTag>("PhoPhIsoCollection");

  phoMaxHE_[0]    = iConfig.getParameter<double>("PhoMaxHE_barrel");
  phoMaxSieie_[0] = iConfig.getParameter<double>("PhoMaxSieie_barrel");
  phoMaxChIso_[0] = iConfig.getParameter<double>("PhoMaxChIso_barrel");
  phoMaxNhIso_[0] = iConfig.getParameter<double>("PhoMaxNhIso_barrel");
  phoMaxPhIso_[0] = iConfig.getParameter<double>("PhoMaxPhIso_barrel");

  phoMaxHE_[1]    = iConfig.getParameter<double>("PhoMaxHE_endcap");
  phoMaxSieie_[1] = iConfig.getParameter<double>("PhoMaxSieie_endcap");
  phoMaxChIso_[1] = iConfig.getParameter<double>("PhoMaxChIso_endcap");
  phoMaxNhIso_[1] = iConfig.getParameter<double>("PhoMaxNhIso_endcap");
  phoMaxPhIso_[1] = iConfig.getParameter<double>("PhoMaxPhIso_endcap");

  phoEffAreas_barrelV_ = iConfig.getParameter<std::vector<double> >("PhoEffAreas_barrel");
  phoEffAreas_endcapV_ = iConfig.getParameter<std::vector<double> >("PhoEffAreas_endcap");

  // Register consumption
  tok_Photon_ = consumes<reco::PhotonCollection>(labelPhoton_);
  tok_PFJet_  = consumes<reco::PFJetCollection>(labelPFJet_);
  tok_vertices_=consumes<reco::VertexCollection>(labelVertexColl_);
  tok_rho_    = consumes<double>(labelRhoColl_);
  tok_phoSieieMap_ = consumes<edm::ValueMap<float> >(labelPhoSieieColl_);
  tok_phoChIsoMap_ = consumes<edm::ValueMap<float> >(labelPhoChIsoColl_);
  tok_phoNhIsoMap_ = consumes<edm::ValueMap<float> >(labelPhoNhIsoColl_);
  tok_phoPhIsoMap_ = consumes<edm::ValueMap<float> >(labelPhoPhIsoColl_);

}


AlCaGammaJetSelector::~AlCaGammaJetSelector()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
AlCaGammaJetSelector::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  nProcessed_++;
  if (h1SelFlag) h1SelFlag->Fill(int(_nProc));

  // Access the collections from iEvent
  edm::Handle<reco::PhotonCollection> phoHandle;
  iEvent.getByToken(tok_Photon_,phoHandle);
  if (!phoHandle.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get the product " << labelPhoton_;
    return true; // do not filter
  }
  const reco::PhotonCollection photons = *(phoHandle.product());

  if (h1SelFlag) h1SelFlag->Fill(int(_nPhoCollOk));

  edm::Handle<reco::PFJetCollection> pfjetHandle;
  iEvent.getByToken(tok_PFJet_,pfjetHandle);
  if (!pfjetHandle.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "AlCaGammaJetProducer: Error! can't get product " << labelPFJet_;
    return true; // do not filter
  }
  const reco::PFJetCollection pfjets = *(pfjetHandle.product());

  if (h1SelFlag) h1SelFlag->Fill(int(_nJetCollOk));

  // Check the conditions for a good event
  if (photons.size()<1) return false;
  if (h1SelFlag) h1SelFlag->Fill(int(_nOnePhoOk));
  if (pfjets.size()<1) return false;
  if (h1SelFlag) h1SelFlag->Fill(int(_nOneJetOk));

  // Find the leading photon (pt and idx)
  double maxPhoPt=0;
  int maxPhoPtIdx=-1;
  for (unsigned int ipho=0; ipho< photons.size(); ipho++) {
    if (photons.at(ipho).pt() > maxPhoPt) {
      maxPhoPt = photons.at(ipho).pt();
      maxPhoPtIdx= int(ipho);
    }
  }
  if (maxPhoPt < minPtPhoton_)  return false;
  if (maxPhoPtIdx==-1) {
    edm::LogWarning("AlCaGammaJet") << "maxPhoPtIdx=-1 after pT cut deemed ok";
    return false;
  }

  if (h1SelFlag) h1SelFlag->Fill(int(_nPhoPtOk));
  const edm::Ref<reco::PhotonCollection> leadPhoRef(phoHandle,maxPhoPtIdx);

  // Get vertices
  edm::Handle<reco::VertexCollection> h_vertices;
  iEvent.getByToken(tok_vertices_, h_vertices);
  if (!h_vertices.isValid()) {
    edm::LogWarning("AlCaGammaJet") << "failed to get vertices";
    return false;
  }
  if (h1SelFlag) h1SelFlag->Fill(int(_nVertexHandleOk));
  if (h_vertices->empty()) return false;

  if (h1SelFlag) h1SelFlag->Fill(int(_nVerticesOk));
  const reco::Vertex &pv = h_vertices->front();
  // photon direction wrt the good PV
  math::XYZVector leadPho_WrtPV( leadPhoRef->superCluster()->x() - pv.x(),
				 leadPhoRef->superCluster()->y() - pv.y(),
				 leadPhoRef->superCluster()->z() - pv.z() );

  // Find the leading jet (et and idx)
  // however, require that the jet is far from the photon
  double maxJetPt=0;
  int maxJetPtIdx=-1;
  const double C_PI=static_cast<double>(4)*std::atan(static_cast<double>(1));

  for (unsigned int ijet=0; ijet< pfjets.size(); ijet++) {
    const reco::PFJet* jet= & pfjets.at(ijet);
    if (jet->pt() > maxJetPt) {
      double dEta= leadPho_WrtPV.Eta() - jet->eta();
      double dPhi= leadPho_WrtPV.Phi() - jet->phi();
      if (dPhi>C_PI) dPhi = static_cast<double>(2)*C_PI - dPhi;
      if (sqrt(dEta*dEta + dPhi*dPhi) > 0.5) {
	maxJetPt = pfjets.at(ijet).pt();
	maxJetPtIdx= int(ijet);
      }
    }
  }
  if (maxJetPt < minPtJet_)  return false;
  if (maxJetPtIdx==-1) {
    edm::LogWarning("AlCaGammaJet") << "maxJetPtIdx=-1 after pT cut deemed ok";
    return false;
  }

  if (h1SelFlag) h1SelFlag->Fill(int(_nJetPtOk));
  const edm::Ref<reco::PFJetCollection> leadJetRef(pfjetHandle,maxJetPtIdx);

  // check the angle between the photon and the jet
  double dphi= std::fabs( leadPhoRef->phi() - leadJetRef->phi() );
  if (dphi>C_PI) dphi = std::fabs(static_cast<double>(2)*C_PI - dphi);
  if (dphi < minDPhiPhoJet_) return false;

  if (h1SelFlag) h1SelFlag->Fill(int(_nDPhiOk));

  // ------------------- Photon quality cuts
  if (applyPhoQualityCuts_) {

    const int index= (leadPhoRef->isEB()) ? 0 : 1;

    // check H/E
    if (leadPhoRef->hadTowOverEm() > phoMaxHE_[index]) return false;
    if (h1SelFlag) h1SelFlag->Fill(int(_nHoEOk));

    // check sigma_IEtaIEta
    edm::Handle<edm::ValueMap<float> > h_phoSieie;
    iEvent.getByToken(tok_phoSieieMap_, h_phoSieie);
    if (!h_phoSieie.isValid()) {
      edm::LogWarning("AlCaGammaJet") << "Failed to get phoSieie";
      return false;
    }
    if (h1SelFlag) h1SelFlag->Fill(int(_nSieieHandleOk));

    if ((*h_phoSieie)[leadPhoRef] > phoMaxSieie_[index]) return false;
    if (h1SelFlag) h1SelFlag->Fill(int(_nSieieOk));

    // get pile-up density rho
    edm::Handle<double> rhoHandle;
    iEvent.getByToken(tok_rho_,rhoHandle);
    if (!rhoHandle.isValid()) {
      edm::LogWarning("AlCaGammaJet") << "rhoHandle is not valid";
      return false;
    }
    const double rho= *(rhoHandle.product());
    if (h1SelFlag) h1SelFlag->Fill(int(_nRhoHandleOk));

    // get isolation values from the EG isolation map
    edm::Handle<edm::ValueMap<float> > h_egmPhoChIso, h_egmPhoNhIso, h_egmPhoPhIso;
    iEvent.getByToken(tok_phoChIsoMap_, h_egmPhoChIso);
    iEvent.getByToken(tok_phoNhIsoMap_, h_egmPhoNhIso);
    iEvent.getByToken(tok_phoPhIsoMap_, h_egmPhoPhIso);
    if (!h_egmPhoChIso.isValid() || !h_egmPhoNhIso.isValid() ||
	!h_egmPhoPhIso.isValid()) {
      edm::LogWarning("AlCaGammaJet") << "Failed to get egm isolation";
      return false;
    }
    if (h1SelFlag) h1SelFlag->Fill(int(_nEgmIsoHandleOk));

    const std::vector<double> *eaArr=
      (index==0) ? &phoEffAreas_barrelV_ : &phoEffAreas_endcapV_;

    // estimate the area-corrected isolation
    double phoChIsoEA = std::max(static_cast<double>(0),
		 (*h_egmPhoChIso)[leadPhoRef] - rho * eaArr->at(_idxCHIso));
    double phoNhIsoEA = std::max(static_cast<double>(0),
		 (*h_egmPhoNhIso)[leadPhoRef] - rho * eaArr->at(_idxNHIso));
    double phoPhIsoEA = std::max(static_cast<double>(0),
		 (*h_egmPhoPhIso)[leadPhoRef] - rho * eaArr->at(_idxPhIso));
    if ((phoChIsoEA > phoMaxChIso_[index]) ||
	(phoNhIsoEA > phoMaxNhIso_[index]) ||
	(phoPhIsoEA > phoMaxPhIso_[index])) {
      return false;
    }
    if (h1SelFlag) h1SelFlag->Fill(int(_nIsoOk));
  }

  if (h1SelFlag) h1SelFlag->Fill(int(_nSelectionOk));
  nSelected_++;
  return true;

}

// ------------ method called once each job just before starting event loop  ------------
void AlCaGammaJetSelector::beginJob()
{
  h1SelFlag = new TH1D("h1SelFlag","Selection counts",int(_nLastFlag),double(0.5),double(_nLastFlag+0.5));
}

// ------------ method called once each job just after ending the event loop  ------------
void AlCaGammaJetSelector::endJob() {
  edm::LogWarning("AlCaGammaJet")
    << "AlCaGammaJetSelector: nProcessed=" << nProcessed_
    << ", nSelected=" << nSelected_;

  if (h1SelFlag) {
    std::stringstream ss;
    ss << "Full account:\n";
    ss << " nProc      = " << h1SelFlag->GetBinContent(int(_nProc)) << "\n";
    ss << " nPhoCollOk = " << h1SelFlag->GetBinContent(int(_nPhoCollOk)) << "\n";
    ss << " nJetcollOk = " << h1SelFlag->GetBinContent(int(_nJetCollOk)) << "\n";
    ss << " nOnePhoOk  = " << h1SelFlag->GetBinContent(int(_nOnePhoOk)) << "\n";
    ss << " nOneJetOk  = " << h1SelFlag->GetBinContent(int(_nOneJetOk)) << "\n";
    ss << " nPhoPtOk   = " << h1SelFlag->GetBinContent(int(_nPhoPtOk)) << "\n";
    ss << " nVertexHandleOk = " << h1SelFlag->GetBinContent(int(_nVertexHandleOk)) << "\n";
    ss << " nVerticesOk= " << h1SelFlag->GetBinContent(int(_nVerticesOk)) << "\n";
    ss << " nJetPtOk   = " << h1SelFlag->GetBinContent(int(_nJetPtOk)) << "\n";
    ss << " nDPhiOk    = " << h1SelFlag->GetBinContent(int(_nDPhiOk)) << "\n";
    if (!applyPhoQualityCuts_) ss << " photonQualityCuts are not applied\n";
    else {
      ss << " nHoEOk     = " << h1SelFlag->GetBinContent(int(_nHoEOk)) << "\n";
      ss << " nSieieHandleOk = " << h1SelFlag->GetBinContent(int(_nSieieHandleOk)) << "\n";
      ss << " nSieieOk   = " << h1SelFlag->GetBinContent(int(_nSieieOk)) << "\n";
      ss << " nRhoHandleOk   = " << h1SelFlag->GetBinContent(int(_nRhoHandleOk)) << "\n";
      ss << " nEgmIsoHandleOk=" << h1SelFlag->GetBinContent(int(_nEgmIsoHandleOk)) << "\n";
      ss << " nIsoOk    =  " << h1SelFlag->GetBinContent(int(_nIsoOk)) << "\n";
    }
    ss << " nSelectionOk = " << h1SelFlag->GetBinContent(int(_nSelectionOk)) << "\n";
    edm::LogWarning("AlCaGammaJet") << ss.str();
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
AlCaGammaJetSelector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  //desc.add<edm::InputTag>("PhoInput", edm::InputTag("gedPhotons"));
  //desc.add<edm::InputTag>("PFjetInput", edm::InputTag("ak4PFJetsCHS"));
  //desc.add<double>("MinPtJet", 10.0);
  //desc.add<double>("MinPtPhoton", 10.0);
  desc.setAllowAnything();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(AlCaGammaJetSelector);
