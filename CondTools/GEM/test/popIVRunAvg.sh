#!/bin/bash

runNo=$1
toRun=$2

if [ ${#toRun} -eq 0 ] ; then
    echo "popIVRunAvg.sh fromRun toRun"
    echo
    echo "  The script calls "
    echo "cmsRun gemPVSS_IVRunAvgPopConAnalyzer.py runNumber=\$runNo"
    exit 1
fi

while [ ${runNo} -le ${toRun} ] ; do
    cmsRun gemPVSS_IVRunAvgPopConAnalyzer.py runNumber=$runNo | tee log-popIVRunAvg-${runNo}.log
    runNo=$(( runNo + 1 ))
done
