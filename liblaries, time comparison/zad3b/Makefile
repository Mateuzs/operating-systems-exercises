
all: static shared dynamic staticO0 staticO1 staticO2 staticO3 staticOS

static:
	# kompilacja zrodla i uruchomienie programu testujacego z biblioteka statyczna
	gcc -Wall -std=gnu99 -o mainStatic main.c -L. tableOfBlocksLibrary.a
	./mainStatic

shared:

	# kompilacja zrodla i uruchomienie programu testujacego z biblioteka wspoldzielona
	gcc -Wall -std=gnu99 main.c -L. -Wl,  libtableofblocks.so.1.0 -o mainShared
	./mainShared

dynamic:
	# kompilacja zrodla i uruchomienie programu testujacego z biblioteka dynamiczna
	gcc -Wall -std=gnu99  main2.c -ldl -o mainDynamic
	./mainDynamic

# optymalizacja O0
staticO0:
		gcc -Wall -std=gnu99 -o2 -o mainStaticO0 main.c -L. tableOfBlocksLibrary.a
		./mainStaticO0

# optymalizacja O1
staticO1:
	gcc -Wall -std=gnu99  -o1 -o mainStaticO1 main.c -L. tableOfBlocksLibrary.a
	./mainStaticO1

# optymalizacja O2
staticO2:
	gcc -Wall -std=gnu99 -o2 -o mainStaticO2 main.c -L. tableOfBlocksLibrary.a
	./mainStaticO2

# optymalizacja O3
staticO3:
		gcc -Wall -std=gnu99 -o2 -o mainStaticO3 main.c -L. tableOfBlocksLibrary.a
		./mainStaticO3

# optymalizacja OS
staticOS:
	gcc -Wall -std=gnu99 -o2 -o mainStaticOS main.c -L. tableOfBlocksLibrary.a
	./mainStaticOS


clean:
	rm -f *.o
