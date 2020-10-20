# EGM energy corrections and uncertainties

## EGM regression

The EGM regression applies MC-based corrections to the electron and photon energy scale and resolution. In order to apply the EGM regression use the following code in your python configuration:

```python
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
```

and add `process.regressionApplication` to your path. The EGM regression creates a clone of the `slimmedElectrons` and `slimmedPhotons` collections.

## EGM smearer

The EGM smearer applies data-based corrections to the electron and photon energy scale and resolution. These corrections are derived with electrons from Z-boson decays will work better in the corresponding phase-space. In order to apply the EGM smearer use the following code in your python configuration:

```python
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
                  calibratedPatElectrons  = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      ),
                  calibratedPatPhotons    = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      )
                                                   )
process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')
process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')
```

and add `process.calibratedPatElectrons` and `process.calibratedPatPhotons` to your path. The EGM smearer creates collections `calibratedPatElectrons` and `calibratedPatPhotons`. It also creates a series with ValueMaps with the associated uncertainties in the EGM energy scale and resolution. These uncertainties are also derived with electrons from Z-boson decays.

#### Example

There is an example file `applySmearer.py` with an working example on how to run the EGM regression and smearer. You can use to run over your miniAOD files and produce calibrated collections.

## Accessing uncertainties

Uncertainties can be accessed using the classes defined in `EgammaAnalysis/ElectronTools/interface/ElectronEnergyShifter.h` and `EgammaAnalysis/ElectronTools/interface/PhotonEnergyShifter.h`. The test file `EgammaAnalysis/ElectronTools/test/ElectronTestUncertainty.cc` has an example how to retrieve the shifted objects. The following two functions can be used (and corresponding ones to photons):

```c++
getShiftedObject(edm::RefToBase<reco::GsfElectron>, EGMSmearer::UncertaintyType);
getSimpleShiftedObject(edm::RefToBase<reco::GsfElectron>, EGMSmearer::SimplifiedUncertaintyType);
```

The first method shifts the electron with each source of systematic uncertainties:

Uncertainty | EGMSmearer::UncertaintyType | To be applied on
------------|-----------------------------|------------
Scale statistics | `EGMSmearer::ScaleStatUp`, `EGMSmearer::ScaleStatDown` | data
Scale systematics | `EGMSmearer::ScaleSystUp`, `EGMSmearer::ScaleSystDown` | data
Scale gain | `EGMSmearer::ScaleGainUp`, `EGMSmearer::ScaleGainDown` | data
Resolution rho | `EGMSmearer::ResolutionRhoUp`, `EGMSmearer::ResolutionRhoDown` | MC
Resolution phi | `EGMSmearer::ResolutionPhiUp`, `EGMSmearer::ResolutionPhiDown` | MC

The second method has a simplified version of the systematic uncertainties

Uncertainty | EGMSmearer::UncertaintyType | To be applied on
------------|-----------------------------|------------
Scale | `EGMSmearer::ScaleUp`, `EGMSmearer::ScaleDown` | data
Resolution | `EGMSmearer::ResolutionUp`, `EGMSmearer::ResolutionDown` | MC

The example can be run with `UncertaintyCMSSW.py`. There is also an example on how to access systematics in FWLite in `UncertaintyFWLite.py`

## Running the EGM regression and smearer in FWLite

We do not support running the regression and smearer on FWLite since it depends on the condition database.



