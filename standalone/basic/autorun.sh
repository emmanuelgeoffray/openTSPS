#!/bin/sh
 
#PROJECTPATH='/home/mina/sources/of_v0.8.0_linux64_release/apps/soixante/nofish/Aquarium'
#LAUNCHCOMMAND='./bin/Aquarium -host kenzo.ramsey.io -port 80 &'
PROJECTPATH='/home/mina/sources/of_v0.8.0_linux64_release/apps/soixante/openTSPS/standalone/basic/bin/basic'

#cd $PROJECTPATH
while true; do
if ps aux | grep -v grep | grep $PROJECTPATH 
then
echo "found" 
else
echo "run" 
#$LAUNCHCOMMAND
$PROJECTPATH
 
fi
done
