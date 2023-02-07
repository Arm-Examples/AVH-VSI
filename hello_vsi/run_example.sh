#!/usr/bin/env bash

# Find the most recent build target
if grep -qs Corstone_300 Objects/CMakeLists.txt ;
then 
    echo "Running Hello VSI application on Corstone 300 FVP"
    FVP="VHT_Corstone_SSE-300_Ethos-U55"
elif grep -qs Corstone_310 Objects/CMakeLists.txt ;
then
    echo "Running Hello VSI application on Corstone 310 FVP"
    FVP="VHT_Corstone_SSE-310"
else
    echo "Was not able to find suitable application. Please buid or rebuild the applciation."
    exit
fi

# Execute the application on the FVP 
$FVP -V "./source/VSI/data_sensor/python" -f fvp_config.txt -a Objects/hello_vsi.axf --stat --simlimit 24 $*

