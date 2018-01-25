CC=clang
LIB=-lz -llzo2 -lucl

default: dumpifs uuu zzz

zzz:
	${CC} zzz.c -o zzz ${LIB}
uuu:
	${CC} uuu.c -o uuu ${LIB}

dumpifs: dumpifs.c
	${CC} dumpifs.c -o dumpifs ${LIB}
clean:
	rm dumpifs
	rm uuu
	rm zzz
