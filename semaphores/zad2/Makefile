
all: compile

compile:
	# kompilacja zrodla
	gcc -Wall -std=gnu99  -o barber barber.c -pthread -lrt
	gcc -Wall -std=gnu99  -o clients clients.c -pthread -lrt

clean:
	#clean
	rm barber clients
