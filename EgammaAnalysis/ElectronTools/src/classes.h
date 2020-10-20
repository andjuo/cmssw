#include "EgammaAnalysis/ElectronTools/interface/EpCombinationToolSemi.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEnergyShifter.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEnergyCalibratorRun2.h"
#include "EgammaAnalysis/ElectronTools/interface/PhotonEnergyCalibratorRun2.h"

namespace {
  struct electrontools_dictionary {
    EpCombinationToolSemi epts;
    ElectronEnergyCalibratorRun2 eec2;
    PhotonEnergyCalibratorRun2 pec2;
    ElectronEnergyShifter ees;
  };
}
