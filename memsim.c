//***************************************************
// memsim.c
//
// A program to simulate page replacement algorithms
// And report their performance
//
//***************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lru();
void fifo();
void vms();

int main(int argc, char *argv[], int argv[]) { //requires <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>

	char tracefile = argv[0];
	int nframes = argv[1];
	char alg = argv[2]; //lru/fifo/vms
	char mode = argv[3]; //debug/quiet
	
	int TraceEvents = 0;
	int tdr = 0; //total disk reads
	int tdw = 0; //total disk writes
	char action; //action records either R for read or W for write in the trace

	//DEBUG MODE
	FILE *file;
	file = fopen (filename, "r");
	if (file) {
		while (fscanf (file, "%s") != EOF) {
			//does stuff
		}
		fclose (file)
	}
	
	
	//QUIET MODE
	printf ("total memory frames: %d", nframes); //needs total memory frames
	printf ("events in trace: %d", TraceEvents); // needs events in trace
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
