#! /usr/bin/env python

# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.FWLiteEnabler.enable()

#cms python data types
import FWCore.ParameterSet.Config as cms

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

from EgammaAnalysis.ElectronTools.egammaEnergyShifter import *
from EgammaAnalysis.ElectronTools.egammaEnergyShifter_cff import *
calibratedElectrons = electronEnergyShifter(configElectronEnergyShifter)

events = Events('step4.root')

for iev,event in enumerate(events):
    
    print iev
    calibratedElectrons.setEvent(event)    
    for i,el in enumerate(calibratedElectrons.electrons.product()):

        if el.pt() < 5: continue
        print el.energy()
        print calibratedElectrons.getSimpleShiftedObject(i, simplifiedUncertaintyType.ScaleUp).energy()
        print calibratedElectrons.getSimpleShiftedObject(i, simplifiedUncertaintyType.ScaleDown).energy()
        print calibratedElectrons.getSimpleShiftedObject(i, simplifiedUncertaintyType.ResolutionUp).energy()
        print calibratedElectrons.getSimpleShiftedObject(i, simplifiedUncertaintyType.ResolutionDown).energy()
