#!/bin/sh

PRGNAME=$0

DIRNAME=`dirname $0`

DUMPIFS=$DIRNAME/dumpifs

prnUsageAndQuit()
{
	echo "Usage: $PRGNAME <ifs image> <destination directory>"
	exit
}

if [ "x$1" = "x" ];then
	prnUsageAndQuit
fi

if [ "x$2" = "x" ];then
	prnUsageAndQuit
fi

dirs=`$DUMPIFS $1 | awk '{print($3)}'|sort -u |xargs -n 1 dirname |sort -u`
for d in $dirs;do
theDir=$2/$d
echo mkdir -p $theDir
mkdir -p $theDir
done

cd $2
$DUMPIFS -x ../$1
cd ..
