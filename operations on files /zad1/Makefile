
all: test

test:
	# kompilacja zrodla
	gcc -Wall -std=gnu99  -o test test.c generator.c sort.c copy.c myTime.c
	#execute tests
	./test
	#clean
	rm file*
	rm test
