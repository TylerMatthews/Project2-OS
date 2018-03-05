//***********************************************************************
// Authors: Tyler Matthews, Matthew Monnik
// Date: 03/03/2018
// Page replacement algorithm simulator
//
//
//***********************************************************************
#include <stdio.h>

//Initial counts
int numTraces = 0;
int numReads = 0;
int numWrites = 0;
int numHits = 0;

struct PageTableEntry
{
	int isPresent;	//Is it in memory?	
    int isValid;	
    int virtualPN;	//Virtual page number
	int isAdded;	//lru, to see when added
	int isDirty;	
};

//Initalize a page table
void initPageTable(struct PageTableEntry pageTable[], int pageTableSize)
{
	int i;
	for(i = 0; i < pageTableSize; i++)
	{
		pageTable[i].virtualPN = -1;
		pageTable[i].isDirty = 0;
		pageTable[i].isValid = 0;
		pageTable[i].isPresent = -1;
	}
}

//Initializes the cache
void initCache(struct PageTableEntry cache[], int numFrames)
{
	int i;
	for(i = 0; i < numFrames; i++)
	{
		cache[i].virtualPN = -1;
		cache[i].reference = 1;
		cache[i].isAdded = -1;
		cache[i].distance = -1;
	}
}

//Returns the location in cache unless it is not in cache.
int isCached(struct PageTableEntry cache[], int numFrames, unsigned pageNumber, int debug)
{
	int i;
	for(i = 0; i < numFrames; i++)
	{
		if(cache[i].virtualPN == pageNumber)
		{
			hitCount++;
			if(debug)
			{
				printf("Found in cache.\n");
			}
			return i;	//page Location
		}
	}
	return -1;	//If page was not found
}

//Prints the Cache
void displayCached(struct PageTableEntry cache[], int numFrames)
{
	int i;
	printf("Cache: ");
	for(i = 0; i < numFrames; i++)
	{
		printf("%d, ", cache[i].virtualPN);
	}
	printf("\n");
}