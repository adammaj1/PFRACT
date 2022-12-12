#!/bin/bash 
 
# script file for BASH 
# which bash
# save this file as r.sh
# chmod +x r.sh
# ./r.sh
# checked in https://www.shellcheck.net/

# export LC_NUMERIC="en_US.UTF-8"
export LC_COLLATE=C
export LC_NUMERIC=C
export LANG=en_GB.utf8 # the problem with printf and locale settings

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


ySize=800
xSize=$((3*$ySize)) # xSize = 3* ySize




# 1.4 is periodic part
#  1.749700000000000
iMax=100 # number of images to draw
X=-1.7891690000000000 # translation = lower bound of X range 
Y=+0 # add sign for more readibility
dX=$(echo "0.0000000100000000/$iMax"|bc -l) # adding positive nuber to negative number = go toward 0


printf "run the compiled programs \n"
# printf "make %d images for X from %f to %f \n" "$iMax" "" ""
for i in $(seq 1 $iMax) # https://stackoverflow.com/questions/169511/how-do-i-iterate-over-a-range-of-numbers-defined-by-variables-in-bash
do
	printf "i = %d \t cx = %.16f\n" "$i" "$X"
	absX=$(echo "-1.0*$X"|bc -l) # number without sign https://stackoverflow.com/questions/3779338/how-to-print-a-positive-number-as-a-negative-number-via-printf
	FNAME="$(printf "%.16f" $absX)" # to keep the same number of zeros = format the file name
	./pfract -x $X  -y $Y -size $xSize $ySize -iterations 1256 -rad -20 $FNAME.raw # > $FNAME.txt
	./colorize $FNAME.raw $FNAME.ppm -spawn -400 -scale 1.6 -fadedepth 25
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


printf "info about software \n"
lsb_release -a  # operating system
bash --version # CLI
# utility programs
make -v 
gcc --version
convert -version
convert -list resource
# end


