#!/bin/bash

runNo=$1
toRun=$2

if [ ${#toRun} -eq 0 ] ; then
    echo "dumpIVRunAvg.sh fromRun toRun"
    echo
    echo "  The script calls "
    echo "cmsRun readIVRunAvgDB_v2.py runNumber=\$runNo"
    exit 1
fi

while [ ${runNo} -le ${toRun} ] ; do
    cmsRun readIVRunAvgDB_v2.py runNumber=$runNo | tee log-dumpIVRunAvg-${runNo}.log
    outfbase="dumpGEMPVSSIVRunAvg"
    mv ${outfbase}.out ${outfbase}-${runNo}.out
    runNo=$(( runNo + 1 ))
done
