#!/bin/bash 
 
# script file for BASH 
# which bash
# save this file as r.sh
# chmod +x r.sh
# ./r.sh
# checked in https://www.shellcheck.net/



ySize=800 # size[1]
xSize=$((3*$ySize)) # size[0] = xSize = 3* ySize because 10* ySize shows to much exterior of the set with no interesting features   





X=-2.001 # x
Y=+0 # add sign for more readibility = y 
dX=0.001 


printf "run the compiled programs\n"
for i in {1..20}
do
	echo "$i"
	./pfract -x $X  -y $Y -size $xSize $ySize -iterations 1256 -rad -20 e$X$Y.raw > e$X$Y.txt
	./colorize e$X$Y.raw e$X$Y.ppm -spawn -400 -scale 1.6 -fadedepth 25
	X=$(echo "$X + $dX"|bc -l)  # new X value
done # for i in {1..9}


printf "convert all ppm files to png using Image Magic v 6 convert \n"
# for all ppm files in this directory
for file in *.ppm ; do
  # b is name of file without extension
  b=$(basename "$file" .ppm)
  # convert  using ImageMagic : -resize widthxheight || 
  convert "${b}".ppm  "${b}".png  # iWidth = iHeight* DisplayAspectRatio 
  echo "$file"
done # for file 





printf "delete all ppm and raw files \n"
rm ./*.ppm
rm ./*.raw

