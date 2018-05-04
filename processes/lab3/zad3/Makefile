
all: test

test:
	# kompilacja zrodla
	gcc -Wall -std=gnu99  -o main main.c
	gcc -Wall -std=gnu99  -o main2 main2.c
	gcc -Wall -std=gnu99  -o main3 main3.c

	#execute test
	./main file.txt 2 100 || true
	./main file.txt 3 100
	./main file.txt 3 300
	./main file.txt 3 1200
