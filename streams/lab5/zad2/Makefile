
all: test

test:
	# kompilacja zrodla
	gcc -Wall -std=gnu99  -o main main.c
	gcc -Wall -std=gnu99  -o master master.c
	gcc -Wall -std=gnu99  -o slave slave.c

	#execute tests
	./main potok 5 5
	#clean
	rm main
	rm slave
	rm master
