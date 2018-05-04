
all: static shared

static:
	# kompilacja zrodla
	gcc -Wall -std=gnu99  -c tableOfBlocksDynamicAloc.c -o tableOfBlocksDynamicAloc.o
	gcc -Wall -std=gnu99  -c tableOfBlocksStaticAloc.c -o tableOfBlocksStaticAloc.o
	gcc -Wall -std=gnu99  -c dataBase.c -o dataBase.o


	# zbuduj biblioteke statyczna .a
	ar rcs tableOfBlocksLibrary.a tableOfBlocksDynamicAloc.o tableOfBlocksStaticAloc.o dataBase.o

	# czyscimy
	make clean

shared:

	# kompilacja źródła
	gcc -Wall -std=gnu99  -fPIC -c tableOfBlocksDynamicAloc.c -o tableOfBlocksDynamicAloc.o
	gcc -Wall -std=gnu99  -fPIC -c tableOfBlocksStaticAloc.c -o tableOfBlocksStaticAloc.o
	gcc -Wall -std=gnu99  -fPIC -c dataBase.c -o dataBase.o


	# zbuduj biblioteke wspoldzielona .so
	gcc -Wall -std=99 -fPIC -shared -o libtableofblocks.so.1.0 tableOfBlocksDynamicAloc.o tableOfBlocksStaticAloc.o dataBase.o

	# czyscimy
	make clean

clean:
	rm -f *.o
