#!/bin/bash

# based on ideas from
# https://cmssdt.cern.ch/SDT/doxygen/CMSSW_7_4_0_pre7/doc/html/de/dd7/classaddOnTests_1_1StandardTester.html

#
# The SinglePhoton Run2012D runNo=208307 is available at T2_CH_CERN
# Create information files:
#   (a) lumiRanges (this is a dummy setting as it is now)
#   (b) and the list of files
#

echo '{
"208307":[[1,268435455]]
}' > step1_lumiRanges.log 2>&1

 (das_client.py --limit 0 --query 'file dataset=/SinglePhoton/Run2012D-v1/RAW run=208307 site=T2_CH_CERN') | sort -u > step1_dasquery.log  2>&1

# run HLT path on data

cmsDriver.py RelVal -s L1REPACK:GCTGT --data --scenario=pp -n 100 --conditions auto:run2_hlt_FULL  --datatier "RAW" --eventcontent RAW --customise=HLTrigger/Configuration/CustomConfigs.L1T --customise SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --magField 38T_PostLS1 --fileout file:RelVal_Raw_FULL_DATA.root --filein filelist:step1_dasquery.log --lumiToProcess step1_lumiRanges.log  > RelVal_reHLT.log  2>&1

# re-reconstruct data

cmsDriver.py RelVal -s HLT:FULL,RAW2DIGI,L1Reco,RECO,EI --data --scenario=pp -n 100 --conditions auto:run2_data_FULL  --datatier "RAW-HLT-RECO" --eventcontent FEVTDEBUGHLT --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --magField 38T_PostLS1 --processName=RECO --filein file:RelVal_Raw_FULL_DATA.root --fileout file:RelVal_RECO.root | tee RelVal_reco.log 2>&1

# run HcalCal AlCa producer
# This produces HcalCalGammaJet.root file in addition to RelVal_ALCA.root

cmsDriver.py RelVal -s ALCA:HcalCalGammaJet --data --scenario=pp -n 100 --conditions auto:run2_data_FULL --datatier USER --customise=HLTrigger/Configuration/CustomConfigs.L1THLT --customise=SLHCUpgradeSimulations/Configuration/postLS1Customs.customisePostLS1 --magField 38T_PostLS1 --processName=USER --filein file:RelVal_RECO.root --fileout file:RelVal_ALCA.root | tee RelVal_alca.log 2>&1
