#!/bin/bash 
 
# script file for BASH 
# which bash
# save this file as d.sh
# chmod +x d.sh
# ./d.sh
# checked in https://www.shellcheck.net/




printf "compile the programs \n"
gcc pfract.c -lm -Wall -Wextra -o pfract


if [ $? -ne 0 ]
then
    echo ERROR: pfract compilation failed !!!!!!
    exit 1
fi

gcc colorize.c -lm -Wall -Wextra -o colorize

if [ $? -ne 0 ]
then
    echo ERROR: colorize compilation failed !!!!!!
    exit 1
fi


xSize=8000
ySize=800


printf "run the compiled programs, example 1\n"
./pfract -julia -cx -1.115313 -cy -0.304872 -x -0.147394 -y -0.234163 -size $xSize $ySize -iterations 164 -rad -8 example1.raw
./colorize example1.raw example1.ppm -fadedepth 20 -scale 1.4


./pfract -julia -cx 0.362927 -cy -0.077798 -x -0.274848 -y -0.908705 -size $xSize $ySize -iterations 164 -rad -5.6 example2.raw
./colorize example2.raw example2.ppm -start 270 -spawn -90 -fadedepth 60

	
./pfract -julia -cx 0.362927 -cy -0.077798 -x -0.282406 -y -0.912345 -size $xSize $ySize -iterations 256 -rad -9 example3.raw
./colorize example3.raw example3.ppm -start 200 -spawn -300 -scale 1.4 -fadedepth 50
	
./pfract -julia -cx -1.769705 -cy -0.003474 -x -0.015064 -y -0.036825 -size $xSize $ySize -iterations 256 -rad -8.3 example4.raw
./colorize example4.raw example4.ppm -spawn -360 
	
./pfract -x 0.360211 -y -0.684246 -size $xSize $ySize -iterations 256 -rad -10 example5.raw
./colorize example5.raw example5.ppm -spawn -400 -scale 1.6 -fadedepth 25
	
./pfract -julia -cx -0.771893 -cy -0.111656 -x -0.101176 -y -0.192846 -size $xSize $ySize -iterations 512 -rad -7.2 example6.raw
./colorize example6.raw example6.ppm -spawn -360 -start 45 -fadedepth 100 -scale 1.5


printf "convert all ppm files to png using Image Magic v 6 convert \n"
# for all ppm files in this directory
for file in *.ppm ; do
  # b is name of file without extension
  b=$(basename "$file" .ppm)
  # convert  using ImageMagic : -resize widthxheight || 
  convert "${b}".ppm -resize 50% "${b}".png  # iWidth = iHeight* DisplayAspectRatio 
  echo "$file"
done


printf "delete all ppm and raw files \n"
rm ./*.ppm
rm ./*.raw

