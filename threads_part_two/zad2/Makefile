
all: test

test:
	# kompilacja zrodla
	gcc -Wall -std=gnu99 -pthread  -o main main.c
	#execute tests
	./main inputFile.txt
	#clean
	rm main
