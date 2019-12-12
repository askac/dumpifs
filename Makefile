CC=clang
LIB=-lz -llzo2 -lucl -llz4

default: dumpifs fixdecifs fixencifs uuu zzz exMifsLzo exMifsLz4

zzz:
	${CC} zzz.c -o zzz ${LIB}
uuu:
	${CC} uuu.c -o uuu ${LIB}
#lz4dict:
#	${CC} lz4dict.c -o lz4dict ${LIB}
exMifsLzo:
	${CC} exMifsLzo.c -o  exMifsLzo ${LIB}
exMifsLz4:
	${CC} exMifsLz4.c -o  exMifsLz4 ${LIB}
fixdecifs: fixdecifs.c
	${CC} fixdecifs.c -o fixdecifs ${LIB}
fixencifs: fixencifs.c
	${CC} fixencifs.c -o fixencifs ${LIB}
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
