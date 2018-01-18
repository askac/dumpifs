CC=clang
LIB=-lz -llzo2 -lucl

dumpifs: dumpifs.c
	${CC} dumpifs.c -o dumpifs ${LIB}
clean:
	rm dumpifs