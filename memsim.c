//***************************************************
// memsim.c
//
// A program to simulate page replacement algorithms
// And report their performance
//
//***************************************************

#include <stdio.h>
#include <stdlib.h>

void lru();
void fifo();
void vms();

int main(int argc, char *argv[], int argv, char argv[], char argv[]) { //requires <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>

	int tdr = 0; //total disk reads
	int tdw = 0; //total disk writes

	printf ("total memory frames: %d", ); //needs total memory frames
	printf ("events in trace: %d", ); // needs events in trace
	printf ("total disk reads: %d", tdr);
	printf ("total disk writes: %d", tdw);

	return 0;

}

void lru() {

}

void fifo() { //queue

}

void vms() {

}