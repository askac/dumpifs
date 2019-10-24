#!/bin/sh


Usage()
{
	echo "$0 <src> <dst>"
}


fs=$1
ft=$2

if [ "x$#" != "x2" ]
then
	Usage
	exit
fi

if [ ! -e $fs ]
then
	Usage
	exit
fi

if [ ! -e $ft ]
then
	Usage
	exit
fi

cmp -l $fs $ft | gawk '{printf "%08X %02X %02X\n", $1-1, strtonum(0$2), strtonum(0$3)}'
