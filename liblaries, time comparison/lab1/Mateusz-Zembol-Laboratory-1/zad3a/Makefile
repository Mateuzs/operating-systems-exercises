
all: static shared dynamic

static:
	# kompilacja zrodla i uruchomienie programu testujacego z biblioteka statyczna
	gcc -Wall -std=gnu99 -o mainStatic main.c -L. tableOfBlocksLibrary.a
	./mainStatic

shared:

	# kompilacja zrodla i uruchomienie programu testujacego z biblioteka wspoldzielona
	gcc -Wall -std=gnu99 main.c -L. -Wl,  libtableofblocks.so.1.0 -o mainShared
	./mainShared

dynamic:
	gcc -Wall -std=gnu99  main2.c -ldl -o mainDynamic
	./mainDynamic

clean:
	rm -f *.o
