CC=clang
QCC=qcc.exe -V4.4.2,gcc_ntoarmv7le
QLIB=-lz -lnbutil -lucl -llzo -DQNX -I includeQNX/
LIB=-lz -llzo2 -lucl -llz4

default: dumpifs fixdecifs fixencifs uuu zzz exMifsLzo exMifsLz4

q:	qnx/dumpifs qnx/uuu qnx/zzz qnx/fixdecifs qnx/fixencifs

qnx/zzz: zzz
	${QCC} zzz.c -o qnx/zzz ${QLIB}
zzz:
	${CC} zzz.c -o zzz ${LIB}
qnx/uuu: uuu
	${QCC} uuu.c -o qnx/uuu ${QLIB}
uuu:
	${CC} uuu.c -o uuu ${LIB}
#lz4dict:
#	${CC} lz4dict.c -o lz4dict ${LIB}
exMifsLzo:
	${CC} exMifsLzo.c -o  exMifsLzo ${LIB}
exMifsLz4:
	${CC} exMifsLz4.c -o  exMifsLz4 ${LIB}
qnx/fixdecifs: fixdecifs
	${QCC} fixdecifs.c -o qnx/fixdecifs ${QLIB}
fixdecifs: fixdecifs.c
	${CC} fixdecifs.c -o fixdecifs ${LIB}
qnx/fixencifs: fixencifs
	${QCC} fixencifs.c -o qnx/fixencifs ${QLIB}
fixencifs: fixencifs.c
	${CC} fixencifs.c -o fixencifs ${LIB}
qnx/dumpifs: dumpifs
	${QCC} dumpifs.c -o qnx/dumpifs ${QLIB}
dumpifs: dumpifs.c
	${CC} dumpifs.c -o dumpifs ${LIB}
clean:
#	rm lz4dict
	rm exMifsLzo
	rm exMifsLz4
	rm dumpifs
	rm uuu
	rm zzz
	rm fixdecifs
	rm qnx/*

