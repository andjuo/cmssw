#ifndef SimpleElectron_STANDALONE
#include "EgammaAnalysis/ElectronTools/interface/SimpleElectron.h"
SimpleElectron::SimpleElectron(const reco::GsfElectron &in, unsigned int runNumber, bool isMC) :
        run_(runNumber),
        eClass_(int(in.classification())), 
        r9_(in.full5x5_r9()),
        scEnergy_(in.superCluster()->rawEnergy() + in.isEB() ? 0 : in.superCluster()->preshowerEnergy()), 
        scEnergyError_(-999.),  // FIXME???
        trackMomentum_(in.trackMomentumAtVtx().R()), 
        trackMomentumError_(in.trackMomentumError()), 
        regEnergy_(in.correctedEcalEnergy()), 
        regEnergyError_(in.correctedEcalEnergyError()), 
        eta_(in.superCluster()->eta()), 
        isEB_(in.isEB()), 
        isMC_(isMC), 
        isEcalDriven_(in.ecalDriven()), 
        isTrackerDriven_(in.trackerDrivenSeed()), 
        newEnergy_(regEnergy_), 
        newEnergyError_(regEnergyError_),
        combinedMomentum_(in.p4(reco::GsfElectron::P4_COMBINATION).P()), 
        combinedMomentumError_(in.p4Error(reco::GsfElectron::P4_COMBINATION)),
        scale_(1.0), smearing_(0.0)
{
}

std::ostream& operator<<(std::ostream& out, const math::XYZTLorentzVector &v)
{
  out << "(" << v.Pt() << ", " << v.Eta() << ", " << v.Phi() << " " << v.E() << ")";
  return out;
}


void SimpleElectron::writeTo(reco::GsfElectron & out, int debug) const 
{
    math::XYZTLorentzVector oldMomentum = out.p4();
    math::XYZTLorentzVector newMomentum = math::XYZTLorentzVector(oldMomentum.x()*getCombinedMomentum()/oldMomentum.t(),
                                                                  oldMomentum.y()*getCombinedMomentum()/oldMomentum.t(),
                                                                  oldMomentum.z()*getCombinedMomentum()/oldMomentum.t(),
                                                                  getCombinedMomentum());
    if (debug) std::cout << "set " << oldMomentum << " to " << newMomentum << " (scale=" << getCombinedMomentum()/oldMomentum.t() << ")\n";
    out.setCorrectedEcalEnergy(getNewEnergy());
    out.setCorrectedEcalEnergyError(getNewEnergyError());
    out.correctMomentum(newMomentum, getTrackerMomentumError(), getCombinedMomentumError());
}
#endif


std::ostream& operator<<(std::ostream &out, const SimpleElectron *e)
{
  out << e->run_ << " " << e->eClass_ << " " << e->r9_ << " "
      << e->scEnergy_ << " " << e->scEnergyError_ << " "
      << e->trackMomentum_ << " " << e->trackMomentumError_ << " "
      << e->regEnergy_ << " " << e->regEnergyError_ << " "
      << e->eta_ << " " << e->isEB_ << " " << e->isMC_ << " "
      << e->isEcalDriven_ << " " << e->isTrackerDriven_ << " "
      << e->newEnergy_ << " " << e->newEnergyError_ << " "
      << e->combinedMomentum_ << " " << e->combinedMomentumError_ << " "
      << e->scale_ << " " << e->smearing_ << " " << e->correction_;
  return out;
}

int SimpleElectron::read(std::istream &inp)
{
  inp >> run_ >> eClass_ >> r9_
      >> scEnergy_ >> scEnergyError_
      >> trackMomentum_ >> trackMomentumError_
      >> regEnergy_ >> regEnergyError_
      >> eta_ >> isEB_ >> isMC_
      >> isEcalDriven_ >> isTrackerDriven_
      >> newEnergy_ >> newEnergyError_
      >> combinedMomentum_ >> combinedMomentumError_
      >> scale_ >> smearing_ >> correction_;
  return 1;
}
