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

void lru(int time[], int n);
void fifo(int *q, int x);
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
	char str[9] //8 letter string + null
	
	

	//DEBUG MODE
	FILE *file;
	file = fopen (filename, "r");
	if (file) {
		while (fscanf (file, "%s") != EOF) {
			fscanf (file, "%s % c", str, action); //reads string '0129abcf W', store in str
			
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

void lru(int time[], int n) {
	int i, minimum = time[0], pos = 0;
 
    for(i = 1; i < n; ++i){
        if(time[i] < minimum){
            minimum = time[i];
            pos = i;
        }
    }
    
    return pos;
}

void fifo(int *q, int x) { //queue
	int head = 0;
	int tail = 0;
	
	//printf("head=%d\ttail=%d\n", head, tail+1);
	//enqueue
	q[tail] = x;
	if (tail == N) {      
        	tail = 0;
	}
	else {
        	tail++;
	}
    	//printf("tail=%d\n", tail);
	
	//dequeue
	int x;
    	//if (head == tail) {
        //fprintf(stderr, "empty\n"); }
   	 x = q[head];
    	if (head == N) { 
        	head = 0;
	}
	else { 
        	head++;
	}
	return x;
	int qempty(void) {
    		return head == tail;
	}
	
}

void vms() {

}
