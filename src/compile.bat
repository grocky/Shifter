@ECHO OFF

set MDir=~/Code/Shifter/bin
set MFile=Makefile

make -C %MDir% -f %MDir%/%MFile% %1
