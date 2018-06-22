#!/bin/sh
ucltool=./uuu
lzotool=./zzz
compressUse=$ucltool

tempBody=__temp__B
tempBody2=__temp__B2

if [ $# -ne 3 ];then
cat << EOF
Usage: $0 <head offset(dec)> <decompressed.ifs> <destination ifs>
head offset could be found when dump a ifs file.
EOF
exit
fi

if [ $1 -lt 260 ];then
echo "Invalid offset. Atleast 260"
exit
fi

offsetH=$1
srcIfs=$2
dstIfs=$3

if [ -e $tempBody ];then
echo "Temperate file $tempBody exist!"
exit
fi

if [ -e $tempBody2 ];then
echo "Temperate file $tempBody2 exist!"
exit
fi

if [ -e $dstIfs ];then
echo "Destination file $dstIfs exist!"
exit
fi

cat << EOF
Compress method?
1. ucl
2. lzo
EOF

read packuse
if [ "$packuse" -lt 1 -o "$packuse" -gt 2 ];then
	echo "Invalid option $packuse"
	exit
fi

if [ $packuse -eq 1 ];then
	compressUse=$ucltool
fi

if [ $packuse -eq 2 ];then
	compressUse=$lzotool
fi

echo "Select $packuse . Use compress tool $compressUse"

dd if=$srcIfs of=$dstIfs bs=$offsetH count=1
dd if=$srcIfs of=$tempBody bs=$offsetH skip=1
$compressUse $tempBody $tempBody2

echo "Compress using $compressUse done."
echo "Packing $dstIfs"
dd of=$dstIfs if=$tempBody2 bs=1 seek=$offsetH

finalSize=`du -b $dstIfs | awk '{print($1)}'`
dd if=/dev/zero of=$dstIfs bs=1 count=9 seek=$finalSize
echo "Done"
rm $tempBody
rm $tempBody2
ls -l $dstIfs
