//***********************************************************************
// Authors: Tyler Matthews, Matthew Monnik
// Date: 03/03/2018
// Page replacement algorithm simulator
//
//
//***********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memsim.h"

//It as assumed all pages are 4096 Bytes


//First in First out
void fifo(int numFrames, char* traceFile, int debug)
{
	unsigned address1;	//Holds the address from the file
	char readwrite;		//Holds whether it is a read or write from the file

	static int pageTableSize = 1048567;	//LA/Offset = 2^20 table entries

	unsigned pageNum;	

	struct pageTableEntry pageTable[pageTableSize];		//Creates the page table
	struct pageTableEntry cache[numFrames];	//Creates the cache

	int foundInCache = -1;

	//Cache front initialized to -1
	int frontOfCache = -1;	//Front of the cache
	int nextOpenCache = 0; //Next Open Cache

	//Initializations
	initPageTable(pageTable, pageTableSize);
	initCache(cache, numFrames);
	
	FILE* file;
	file = fopen(traceFile, "r");
	if(file)
	{
		//Scans in whole file
		while(fscanf(file, "%x %c", &address1, &readwrite) != EOF)
		{
			pageNum = address1 / (16 * 16 * 16);	//Gets page num

			//Sets the valid bit
			if(pageTable[pageNum].valid == 0)
			{
				pageTable[pageNum].vpn = pageNum;
				pageTable[pageNum].valid = 1;
			}
			//is the frame loaded into cache?
			foundInCache = isCached(cache, numFrames, pageNum, debug);
			
			if(foundInCache >= 0)
			{
				if(readwrite == 'W')
				{
					cache[foundInCache].dirty = 1;
				}
			}
			
			else if(FrontOfCache == -1)
			{
				if(debug)
				{
					printf("Filled in first entry of cache.\n");
				}
				FrontOfCache = 0;
				nextOpenCache = 1;
				cache[FrontOfCache].vpn = pageNum;
				pageTable[pageNum].present = 1;
				numReads++;
				if(readwrite == 'W')
				{
					cache[FrontOfCache].dirty = 1;
				}			
			}
			//If Cache is full
			else if(FrontOfCache == nextOpenCache)
			{
				if(debug)
				{
					printf("Cache is full.\n");
				}
				if(cache[FrontOfCache].dirty == 1)
				{
					pageTable[cache[FrontOfCache].vpn].present = 0;
					if(debug)
					{
						printf("Write to disk.\n");
					}
					numWrites++;
				}
				if(debug)
				{
					printf("Read from disk.\n");
				}
				numReads++;
				pageTable[pageNum].present = 1;
				//Remove front of cache, Front of cache is moved right
				cache[FrontOfCache] = pageTable[pageNum];
				if(readwrite == 'W')
				{
					cache[FrontOfCache].dirty = 1;
				}
				else
				{
					cache[FrontOfCache].dirty = 0;
				}
				FrontOfCache = (FrontOfCache + 1) % numFrames;
				nextOpenCache = FrontOfCache;
			}
			//The cache has not yet been filled up
			else
			{
				if(debug)
				{
					printf("Slot %d in array is filled.\n", nextOpenCache);
				}
				cache[nextOpenCache].vpn = pageNum;
				if(debug)
				{
					printf("Read from disk.\n");
				}
				numReads++;
				pageTable[pageNum].present = 1;
				if(readwrite == 'W')
				{
					cache[nextOpenCache].dirty = 1;
				}
				nextOpenCache = (nextOpenCache + 1) % numFrames;
			}
			numTraces++;
			foundInCache = -1;
			if(debug)
			{
				cacheDisplay(cache, numFrames);
			}
		}
		fclose(file);
	}
	printf("Total disk reads: %d\n", numReads);
	printf("Total disk writes: %d\n", numWrites);
	printf("Hits percentage: %f%%\n", ((double)numHits / numTraces) * 100);
	printf("Total memory numFrames: %d\n", numFrames);
	printf("Events in trace: %d\n", numTraces);
	
}

//Least recently used replacement algorithm
void lru(int numFrames, char* traceFile, int debug)
{
	unsigned address1;	//Holds input address
	char readwrite;	    //read or write

	static int pageTableSize = 1048567;	//2^32 logical address / 2^12 offset = 2^20 = 1048576 page table entries

	int i;
	unsigned pageNum;	

	struct pageTableEntry pageTable[pageTableSize];		//Page table creation
	struct pageTableEntry cache[numFrames];	            //Cache creation

	int foundInCache = -1;
	int now = 0;	//used for page add time
	int leastRecentlyUsed;	
	int leastRecentlyUsedCache;	

	
	//initially empty
	int FrontOfCache = -1;	//Front spot in array
	int nextOpenCache = 0; //next available spot in array

	//Initializations
	initPageTable(pageTable, pageTableSize);
	initCache(cache, numFrames);
	
	FILE* file;
	file = fopen(traceFile, "r");
	if(file)
	{
		//Will scan in everything in the file
		while(fscanf(file, "%x %c", &address1, &readwrite) != EOF)
		{
			pageNum = address1 / (16 * 16 * 16);	//Gets the page number. Offset is 12 bits, hence the 3 16s

			//Sets the valid bit
			if(pageTable[pageNum].valid == 0)
			{
				pageTable[pageNum].vpn = pageNum;
				pageTable[pageNum].valid = 1;
			}
			//Checks to see if the frame has already been loaded in cache
			foundInCache = isCached(cache, numFrames, pageNum, debug);
			//Page is already in cache
			if(foundInCache >= 0)
			{
				cache[foundInCache].added = now;
				now++;
				if(readwrite == 'W')
				{
					cache[foundInCache].dirty = 1;
				}
			}
			//If the Cache is Empty
			else if(FrontOfCache == -1)
			{
				if(debug)
				{
					printf("Filled in first entry of cache.\n");
				}
				FrontOfCache = 0;
				nextOpenCache = 1;
				cache[FrontOfCache].vpn = pageNum;
				cache[FrontOfCache].added = now;
				now++;
				pageTable[pageNum].present = 1;
				numReads++;
				if(readwrite == 'W')
				{
					cache[FrontOfCache].dirty = 1;
				}			
			}
			//If the cache is full
			else if(FrontOfCache == nextOpenCache)
			{
				if(debug)
				{
					printf("Cache is full.\n");
				}
				leastRecentlyUsed = cache[0].added;
				leastRecentlyUsedCache = 0;
				//Increment through cache, find Least Recently Used
				//Least recent Used page is replaced
				for(i = 1; i < numFrames; i++)
				{
					if(cache[i].added < leastRecentlyUsed)
					{
						leastRecentlyUsed = cache[i].added;
						leastRecentlyUsedCache = i;
					}
				}
				if(cache[leastRecentlyUsedCache].dirty == 1)
				{
					pageTable[cache[leastRecentlyUsedCache].vpn].present = 0;
					if(debug)
					{
						printf("Write to disk.\n");
					}
					numWrites++;
				}
				if(debug)
				{
					printf("Read from disk.\n");
				}
				numReads++;
				pageTable[pageNum].present = 1;
				cache[leastRecentlyUsedCache] = pageTable[pageNum];
				cache[leastRecentlyUsedCache].added = now;
				now++;
				if(readwrite == 'W')
				{
					cache[leastRecentlyUsedCache].dirty = 1;
				}
				else
				{
					cache[leastRecentlyUsedCache].dirty = 0;
				}
			}
			//If the cache is not full yet
			else
			{
				if(debug)
				{
					printf("Slot %d in array is filled.\n", nextOpenCache);
				}
				cache[nextOpenCache].vpn = pageNum;
				cache[nextOpenCache].added = now;
				now++;
				if(debug)
				{
					printf("Read from disk.\n");
				}
				numReads++;
				pageTable[pageNum].present = 1;
				if(readwrite == 'W')
				{
					cache[nextOpenCache].dirty = 1;
				}
				nextOpenCache = (nextOpenCache + 1) % numFrames;
			}
			numTraces++;
			foundInCache = -1;
			if(debug)
			{
				cacheDisplay(cache, numFrames);
			}
		}
		fclose(file);
	}
	printf("The total number of disk reads is: %d\n", numReads);
	printf("The total number of disk writes is: %d\n", numWrites);	
	printf("The total number of memory numFrames is: %d\n", numFrames);
	printf("The hit percent is: %f%%\n", ((double)numHits / numTraces) * 100);
	printf("The number of events in the trace is: %d\n", numTraces);
}


//Main, takes in arguement in the form of "memsim <tracefile> <nframes> <lru|fifo|vms> <debug|quiet>"
int main(int argc, char *argv[])
{
	char *traceFile;
	int numFrames;
	char *algorithm;
	char *mode;
	int debug = 0;

	algorithm = argv[3];
	mode = argv[4];
	traceFile = argv[1];
	numFrames = atoi(argv[2]);

	if(strcmp(mode, "debug") == 0)
		debug = 1;
	
	if(strcmp(algorithm, "lru") == 0)
		lru(numFrames, traceFile, debug);
	else if(strcmp(algorithm, "fifo") == 0)
		fifo(numFrames, traceFile, debug);
	else if(strcmp(algorithm, "vms") == 0)
		vms(numFrames, traceFile, debug);
	
	return 0;
}