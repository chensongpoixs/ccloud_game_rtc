#!/bin/bash


ulimit -c unlimited


nohup ./rtc




#!/bin/bash

#bin_names=( cpgcommmserver cpggatewayserver cpgloginserver cpgcentralserver cpgmatchserver)
#
#
#for name in ${bin_names[@]}
#do
#	nohup ./$name > $name.log 2>&1 &
#	echo "$name start ok"
#done
#
#echo "start all ok"


#f_release

# new server start 
#nohup ./cpgcentralserver cs 127.0.0.1 7731 > centralserver.log 2>&1 &

#nohup ./cpglogserver server.cfg > logserver.log 2>&1 &
 
#nohup ./cpgloginserver ls 127.0.0.1 7621 > loginserver.log 2>&1 &

#nohup ./cpgmatchserver ms_mtt_cpg 127.0.0.1 7641 >  matchserver.log 2>&1 &

#nohup ./cpgcommmserver ms_comm 127.0.0.1 7651 > commserver.log 2>&1 &

#nohup ./cpggatewayserver gs 0.0.0.0 7236  > gatewaryserver.log 2>&1 &

#
#

