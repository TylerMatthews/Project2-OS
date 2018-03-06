//***********************************************************************
// Authors: Tyler Matthews, Matthew Monnik
// Date: 03/03/2018
// Page replacement algorithm simulator
//
//
//***********************************************************************
#include <stdio.h>

int numFrames;
int numCleanPages=0;
char *mode;
double numHits = 0.00;
int numReads = 0;
int numWrites = 0;
int numEvents = 0;
int numDiskReads = 0;

typedef struct VMSFrame 
{
	struct VMSFrame *next;
	char isValid; //VALID BIT
	char isDirty; //DIRTY BIT
	unsigned int num2;       
} VMSFrame_t;

typedef struct queue
{
    struct queue *next;
} queue_t;  

struct LRUFrame
{
	unsigned int num1;  
	char isValid;  //VALID BIT
	char isDirty; //DIRTY BIT
	unsigned int num2;       
};

struct FIFOFrame
{
	unsigned int num1;
	char isValid;  //VALID BIT
	char isDirty; //DIRTY BIT
	unsigned int num2;
};

typedef struct queueStart
{
    struct queue *start;
    struct queue *end;
    int Count;
} queueStart_t;  


void PrintResults(void);
void SaveReference(unsigned int num2, char rw);
void SaveReferenceFIFO(unsigned int num2, char rw);
void initializeLRU(void);
void initializeVMS(void);
void initializeFIFO(void);
void LRU(unsigned int addr, char rw);
void VMS(unsigned int addr, char rw);
void FIFO(unsigned int addr, char rw);
int PutAtQHead(queueStart_t *QueueHead, queue_t *Qelement);
unsigned int GetPageNum(unsigned int addr);
