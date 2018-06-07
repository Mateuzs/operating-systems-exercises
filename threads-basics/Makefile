
all: test

test:
	# kompilacja zrodla
	gcc -Wall -o main main.c
	gcc -Wall -o filter filter_generator.c
	#execute tests
	rm Times.txt
	rm filter25.txt
	rm filter44.txt
	rm filter60.txt

	./filter 25 filter25.txt
	./filter 44 filter44.txt
	./filter 60 filter60.txt

	./main 1 apollonian_gasket.ascii.pgm filter25.txt apollonian2.ascii.pgm
	./main 2 apollonian_gasket.ascii.pgm filter25.txt apollonian2.ascii.pgm
	./main 4 apollonian_gasket.ascii.pgm filter25.txt apollonian2.ascii.pgm
	./main 8 apollonian_gasket.ascii.pgm filter25.txt apollonian2.ascii.pgm

	./main 1 apollonian_gasket.ascii.pgm filter44.txt apollonian3.ascii.pgm
	./main 2 apollonian_gasket.ascii.pgm filter44.txt apollonian3.ascii.pgm
	./main 4 apollonian_gasket.ascii.pgm filter44.txt apollonian3.ascii.pgm
	./main 8 apollonian_gasket.ascii.pgm filter44.txt apollonian3.ascii.pgm

	./main 1 apollonian_gasket.ascii.pgm filter60.txt apollonian4.ascii.pgm
	./main 2 apollonian_gasket.ascii.pgm filter60.txt apollonian4.ascii.pgm
	./main 4 apollonian_gasket.ascii.pgm filter60.txt apollonian4.ascii.pgm
	./main 8 apollonian_gasket.ascii.pgm filter60.txt apollonian4.ascii.pgm
	#clean
	rm main
