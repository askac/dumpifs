CC=clang
LIB=-lz -llzo2 -lucl

default: dumpifs uuu

uuu:
	${CC} uuu.c -o uuu ${LIB}

dumpifs: dumpifs.c
	${CC} dumpifs.c -o dumpifs ${LIB}
clean:
	rm dumpifs
	rm uuu
