//***********************************************************************
// Authors: Tyler Matthews, Matthew Monnik
// Date: 03/03/2018
// Page replacement algorithm simulator
//
//
//***********************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memsim.h"


//for VMS
VMSFrame_t *vmsTables; //Pointer to array of structs
queueStart_t vmsCPQueueStart;
queueStart_t vmsDPQueueStart;

//for LRU
struct LRUFrame *LRUTable; //Pointer to array of structs
struct FIFOFrame *FIFOTable; //Pointer to array of structs

//prints results
void PrintResults(void)
{
    printf("total memory frames: %d\n", numFrames);
    printf("events in trace: %d\n", numEvents);
    printf("total disk reads: %d\n", numReads);
    printf("total disk writes: %d\n", numWrites);
    if(strcmp(mode,"debug") == 0)
        printf("hit rate: %.2f\n", ((1000000-numReads)*100/(double)1000000)); //Hit Ratio
}


//Finds the Least recently used page
struct LRUFrame *LRUPage(void)
{
    int i;
    struct LRUFrame *LRU_PageTableEntries;
    struct LRUFrame *LRU_LastPageTableEntry;

    LRU_PageTableEntries = LRU_LastPageTableEntry = LRUTable;
    i = 0;
    while(i < numFrames)
    {
        if(LRU_PageTableEntries->num1 > LRU_LastPageTableEntry->num1)
            LRU_LastPageTableEntry = LRU_PageTableEntries;
        LRU_PageTableEntries++;
        i++;
    }
    return(LRU_LastPageTableEntry);
}

struct FIFOFrame *FIFOPage(void)
{
    int i;
    struct FIFOFrame *FIFO_PageTableEntries;
    struct FIFOFrame *FIFO_FirstPageTableEntry;

    FIFO_PageTableEntries = FIFO_FirstPageTableEntry = FIFOTable;
    i = 0;
    while(i < numFrames)
    {
        if(FIFO_PageTableEntries->num1 < FIFO_FirstPageTableEntry->num1)
            FIFO_FirstPageTableEntry = FIFO_PageTableEntries;
        FIFO_PageTableEntries--;
        i++;
    }
    return(FIFO_FirstPageTableEntry);
}



void SaveReference(unsigned int num2, char rw)
{
    int i;
    struct LRUFrame *LRU_PageTableEntries;

    i = 0;
    LRU_PageTableEntries = LRUTable;
    while(i < numFrames)
    {
        if((LRU_PageTableEntries->num2 != num2) && (LRU_PageTableEntries->isValid))
            LRU_PageTableEntries->num1++;
        if((rw == 'W') && (LRU_PageTableEntries->num2 ==  num2))
            LRU_PageTableEntries->isDirty = 'W';
        LRU_PageTableEntries++;
        i++;
    }
}

void SaveReferenceFIFO(unsigned int num2, char rw)
{
    int i;
    struct FIFOFrame *FIFO_PageTableEntries;

    i = 0;
    FIFO_PageTableEntries = FIFOTable;
    while(i < numFrames)
    {
        if((FIFO_PageTableEntries->num2 != num2) && (FIFO_PageTableEntries->isValid))
            FIFO_PageTableEntries->num1++;
        if((rw == 'W') && (FIFO_PageTableEntries->num2 ==  num2))
            FIFO_PageTableEntries->isDirty = 'W';
        FIFO_PageTableEntries++;
        i++;
    }
}

void initializeLRU(void)
{
    int i;
    struct LRUFrame *LRU_PageTableEntries;
    i = 0;
    LRU_PageTableEntries = LRUTable;
    while(i < numFrames)
    {
        LRU_PageTableEntries->isDirty = 'R';
        LRU_PageTableEntries++;
        i++;
    }
}

void initializeFIFO(void)
{
    int i;
    struct FIFOFrame *FIFO_PageTableEntries;
    i = 0;
    FIFO_PageTableEntries = FIFOTable;
    while(i < numFrames)
    {
        FIFO_PageTableEntries->isDirty = 'R'; //All Initialized to read
        FIFO_PageTableEntries++;
        i++;
    }
}

//For VMS page replacement
void initializeVMS(void)
{
    int i;
    VMSFrame_t *pageEntryLink;

    numCleanPages=0;
    vmsCPQueueStart.Count = 0;
    vmsDPQueueStart.start = NULL;
    vmsDPQueueStart.end = NULL;
    vmsCPQueueStart.start = NULL;
    vmsCPQueueStart.end = NULL;
    vmsDPQueueStart.Count = 0;

    i = 0;
    pageEntryLink = vmsTables;
    while(i<numFrames)
    {
        pageEntryLink->isDirty = 'R';//All Initialized to Read
        pageEntryLink++;
        i++;
    }
}

VMSFrame_t *freePageLoc(void) //Find free page location
{
    int i;
    VMSFrame_t *pageEntryLink;
    i = 0;
    pageEntryLink = vmsTables;
    while(i < numFrames)
    {
        if(!pageEntryLink->isValid)
            return pageEntryLink;
        i++;
        pageEntryLink++;
    }
    return NULL;
}


unsigned int GetPageNum(unsigned int addr) //Get logical page number
{
    return ((addr>>12));
}


queue_t *removeFromQueue(queueStart_t *QueueHead, queue_t *Qelement)
{
    queue_t *qP;
    //queue_t *qNext;

    if(!QueueHead->Count)
        return NULL;
    if(QueueHead->Count == 1)   
    {
        if(QueueHead->start == Qelement)
        {
            QueueHead->start = NULL;
            QueueHead->end = NULL;
            QueueHead->Count = 0;
            return Qelement;
        }
        else
            return NULL;  //if no element is present.
    }
    qP = QueueHead->start;
    if(qP == Qelement)  //if first element
    {
        QueueHead->start = Qelement->next;
        QueueHead->Count--;
        return Qelement;
    }

    while(qP->next != Qelement)
    {
        qP = qP->next;
    }
    qP->next = Qelement->next;
    if(QueueHead->end == Qelement)
        QueueHead->end = qP;
    QueueHead->Count--;
    return Qelement;
}

int PutAtQHead(queueStart_t *QueueHead, queue_t *Qelement)
{
    if(!QueueHead->Count)
    {
        Qelement->next = NULL;
        QueueHead->start = QueueHead->end = Qelement;
        QueueHead->Count = 1;
    }
    else
    {
        Qelement->next = QueueHead->start;
        QueueHead->start = Qelement;
        QueueHead->Count++;
    }
    return 0;
}

queue_t *GetEnd(queueStart_t *QueueHead)
{
    queue_t *Qelement;
    queue_t *pTempEle;
    //queue_t *pTemp1Element;

    if(!QueueHead->Count)
        return NULL;
    else
    {
        Qelement = QueueHead->start;
        if(QueueHead->Count == 1)
        {
            QueueHead->start = QueueHead->end = NULL;
            QueueHead->Count--;
            return Qelement;
        }
        else if(QueueHead->Count == 2)
        {
            Qelement = QueueHead->end;
            QueueHead->end = QueueHead->start;
            QueueHead->start->next=NULL;
            QueueHead->Count--;
            return Qelement;
        }
        else
        {
            pTempEle = QueueHead->start->next;
            while((pTempEle->next != QueueHead->end) &&
                  (pTempEle->next->next != NULL))
            {
                pTempEle = pTempEle->next;
            }
            Qelement = QueueHead->end;
            QueueHead->end = pTempEle;
            pTempEle->next = NULL;
            QueueHead->Count--;
            return Qelement;
        }
    }
}

//VMS
void VMS(unsigned int addr, char rw)
{
    int i;
    int num2;
    int entryFound = 0;
    VMSFrame_t *pageEntryLink;
    VMSFrame_t *tempLink;

    num2 = GetPageNum(addr);

    
    if(numCleanPages <= (numFrames/2))
    {
        if(!(pageEntryLink = (VMSFrame_t *)vmsCPQueueStart.start)) //check if clean list is empty.
        {   
            pageEntryLink = vmsTables;
            pageEntryLink->num2 = num2;
            pageEntryLink->isDirty = rw;
            pageEntryLink->isValid = 1;
            PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink); // Adds to clean list
            numCleanPages++;
            numReads++;
            return;
        }
        pageEntryLink = (VMSFrame_t *)vmsCPQueueStart.start;
        while(pageEntryLink)
        {

            if((pageEntryLink->num2 == num2) && (pageEntryLink->isValid))
            {
                entryFound = 1;
                break;
            }
            pageEntryLink = pageEntryLink->next;
        }
        if(entryFound)
        {
            if(strcmp(mode,"debug") == 0)
                printf("A page was found in the clean list\n");
            if((pageEntryLink->isDirty == 'R') && (rw == 'W'))
                pageEntryLink->isDirty = 'W';   //Dirt bit is updated if needed.
            return;
        }
        else
        {
            i = 0;
            pageEntryLink = vmsTables;
            while(i <= (numFrames/2))
            {
                if(!pageEntryLink->isValid)
                {
                    pageEntryLink->num2 = num2;
                    pageEntryLink->isValid = 1;
                    pageEntryLink->isDirty = 'R';
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink); //Add to the clean list
                    numCleanPages++;
                    numReads++;
                    return;
                }
                i++;
                pageEntryLink++;
            }
        }
    }

    else
    {
        
        if(numCleanPages != numFrames)
        {
            pageEntryLink = (VMSFrame_t *)vmsCPQueueStart.start;
            while(pageEntryLink)
            {

                if((pageEntryLink->num2 == num2) && (pageEntryLink->isValid))
                {
                    entryFound = 1;
                    break;
                }
                pageEntryLink = pageEntryLink->next;
            }
            if(entryFound)
            {
                if(strcmp(mode,"debug")==0)
                    printf("Page found in Clean list\n");
                if((pageEntryLink->isDirty == 'R') && (rw == 'W'))
                    pageEntryLink->isDirty = 'W';   //update Dirty bit
                return;
            }
            else
            {
                if(!(pageEntryLink = (VMSFrame_t *)vmsDPQueueStart.start)) //Check if dirty list is empty
                {  
                    pageEntryLink = freePageLoc();
                    pageEntryLink->isValid = 1;
                    pageEntryLink->num2 = num2;
                    pageEntryLink->isDirty = 'R';
                    tempLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsCPQueueStart);
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink); 
                    PutAtQHead((queueStart_t *)&vmsDPQueueStart, (queue_t *)tempLink); 
                    numCleanPages++;
                    numReads++;
                    return;
                }
                i = 0;
                pageEntryLink = (VMSFrame_t *)vmsDPQueueStart.start;
                while(pageEntryLink)
                {
                    if((pageEntryLink->num2 == num2) && (pageEntryLink->isValid))
                    {
                        entryFound = 1;
                        break;
                    }
                    pageEntryLink = pageEntryLink->next;
                }
                if(entryFound)
                {
                    if(strcmp(mode,"debug")==0)
                        printf("Page found in Dirty list\n");
                    removeFromQueue((queueStart_t *)&vmsDPQueueStart, (queue_t *)pageEntryLink);
                    if((pageEntryLink->isDirty == 'R') && (rw == 'W'))
                        pageEntryLink->isDirty = 'W';
                    tempLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsCPQueueStart);
                    PutAtQHead((queueStart_t *)&vmsDPQueueStart, (queue_t *)tempLink);
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink);
                    return;
                }
                else
                {
                    pageEntryLink = freePageLoc();
                    pageEntryLink->isValid = 1;
                    pageEntryLink->num2 = num2;
                    pageEntryLink->isDirty = 'R';
                    tempLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsCPQueueStart);
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink);
                    PutAtQHead((queueStart_t *)&vmsDPQueueStart, (queue_t *)tempLink);
                    numCleanPages++;
                    numReads++;
                    return;
                }
            }
        }
        else
        {

            pageEntryLink = (VMSFrame_t *)vmsCPQueueStart.start;
            while(pageEntryLink)
            {
                if((pageEntryLink->num2 == num2) && (pageEntryLink->isValid))
                {
                    entryFound = 1;
                    break;
                }
                pageEntryLink = pageEntryLink->next;
            }
            if(entryFound)
            {
                if(strcmp(mode,"debug")==0)
                    printf("Page found in Clean list\n");
                if((pageEntryLink->isDirty == 'R') && (rw == 'W'))
                    pageEntryLink->isDirty = 'W';
                return;
            }
            else
            {
                //iterate through dirty list

                pageEntryLink = (VMSFrame_t *)vmsDPQueueStart.start;
                while(pageEntryLink)
                {
                    if((pageEntryLink->num2 == num2) && (pageEntryLink->isValid))
                    {
                        entryFound = 1;
                        break;
                    }
                    pageEntryLink = pageEntryLink->next;
                }
                if(entryFound)
                {
                    if(strcmp(mode,"debug") == 0)
                        printf("The page was not found in the dirty list\n");
                    removeFromQueue((queueStart_t *)&vmsDPQueueStart, (queue_t *)pageEntryLink);
                    if((pageEntryLink->isDirty == 'R') && (rw == 'W'))
                        pageEntryLink->isDirty = 'W';
                    tempLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsCPQueueStart);
                    PutAtQHead((queueStart_t *)&vmsDPQueueStart, (queue_t *)tempLink);
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink);
                    return;
                }
                else
                {
                    pageEntryLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsDPQueueStart); //the victim
                    if(pageEntryLink->isDirty == 'W')
                        numWrites++; //page is swapped to disk
                    numReads++;
                    
                    pageEntryLink->num2 = num2;
                    pageEntryLink->isDirty = 'R';
                    tempLink = (VMSFrame_t *)GetEnd((queueStart_t *)&vmsCPQueueStart);
                    PutAtQHead((queueStart_t *)&vmsCPQueueStart, (queue_t *)pageEntryLink);
                    PutAtQHead((queueStart_t *)&vmsDPQueueStart, (queue_t *)tempLink);
                    if(strcmp(mode,"debug")==0)
                        printf("The page was replaced via VMS\n");
                    return;
                }
            }
        }
    }
}


//Least recently Used implementation
void LRU(unsigned int addr, char rw)
{
    int i;
    int num2;
    int entryFound = 0;
    struct LRUFrame *LRU_PageTableEntries;

    num2 = GetPageNum(addr);
    i = 0;
    LRU_PageTableEntries = LRUTable;
    while(i < numFrames)
    {
        if((LRU_PageTableEntries->num2 == num2) && (LRU_PageTableEntries->isValid))
        {
            entryFound = 1;
        }
        LRU_PageTableEntries++;
        i++;
    }
    if(entryFound)
    {
        if(strcmp(mode,"debug") == 0)
            printf("The page was already in the page table\n");
        SaveReference(num2, rw);
        return;
    }

    //Look for a free page.
    entryFound = 0;
    i = 0;
    LRU_PageTableEntries = LRUTable;
    while(i < numFrames)
    {
        if(!LRU_PageTableEntries->isValid && !entryFound)
        {
            LRU_PageTableEntries->isValid = 1;
            LRU_PageTableEntries->num1 = 0; //found,
            LRU_PageTableEntries->num2 = num2;

            entryFound = 1;
        }
        LRU_PageTableEntries++;
        i++;
    }

    if(entryFound) //If a free page is found
    {
        if(strcmp(mode,"debug")==0)
            printf("Page loaded into page table\n");
        numReads++;
        SaveReference(num2, rw);
        return;
    }


    LRU_PageTableEntries = LRUPage();       //If the page is not free, use the LRU algorithm

    if(LRU_PageTableEntries->isDirty == 'W')
        numWrites++;
    numReads++;                             //update number of reads
    LRU_PageTableEntries->num2 = num2;
    LRU_PageTableEntries->num1 = 0;
    LRU_PageTableEntries->isDirty = 'R';    //Initialize new page
    if(strcmp(mode,"debug") == 0)
        printf("The page was replaced via LRU\n");

}

void FIFO(unsigned int addr, char rw)
{
    int i;
    int num2;
    int entryFound = 0;
    struct FIFOFrame *FIFO_PageTableEntries;

    num2 = GetPageNum(addr);
    i = 0;
    FIFO_PageTableEntries = FIFOTable;
    while(i < numFrames)
    {
        if((FIFO_PageTableEntries->num2 == num2) && (FIFO_PageTableEntries->isValid))
        {
            entryFound = 1;
        }
        FIFO_PageTableEntries++;
        i++;
    }
    if(entryFound)
    {
        if(strcmp(mode,"debug") == 0)
            printf("The page was already in the page table\n");
        SaveReferenceFIFO(num2, rw);
        return;
    }

    //Look for a free page.
    entryFound = 0;
    i = 0;
    FIFO_PageTableEntries = FIFOTable;
    while(i < numFrames)
    {
        if(!FIFO_PageTableEntries->isValid && !entryFound)
        {
            FIFO_PageTableEntries->isValid = 1;
            FIFO_PageTableEntries->num1 = 0; //found,
            FIFO_PageTableEntries->num2 = num2;

            entryFound = 1;
        }
        FIFO_PageTableEntries++;
        i++;
    }

    if(entryFound) //If a free page is found
    {
        if(strcmp(mode,"debug")==0)
            printf("Page loaded into page table\n");
        numReads++;
        SaveReferenceFIFO(num2, rw);
        return;
    }

    //If the page is not free, use the FIFO algorithm
    FIFO_PageTableEntries = FIFOPage();

    if(FIFO_PageTableEntries->isDirty == 'W')
        numWrites++;
    numReads++;                             //update number of reads
    FIFO_PageTableEntries->num2 = num2;
    FIFO_PageTableEntries->num1 = 0;
    FIFO_PageTableEntries->isDirty = 'R';    //Initialize new page
    if(strcmp(mode,"debug") == 0)
        printf("The page was replaced via FIFO\n");

}



int main(int argc, char *argv[])
{
    unsigned int addr;
    char rw;
    FILE *file;
    char  *algorithm;
    char *traceFile;
    traceFile= argv[1];
    algorithm = argv[3];
    mode= argv[4];
    numFrames = atoi(argv[2]);

    
    if(strcmp(algorithm,"vms") == 0)
    {
        //allocate array space
        vmsTables = (VMSFrame_t *)calloc((size_t)(sizeof(struct VMSFrame)*numFrames), (size_t)1);
        initializeVMS();  
        file = fopen(traceFile, "r");
        if(file == 0)
        {
            perror("fopen");
            exit(1);
        }
        while(fscanf(file,"%x %c",&addr,&rw) == 2)
        {
            numEvents++;
            VMS(addr, rw);
        }

    }
    else if(strcmp(algorithm,"lru") == 0)
    {
        LRUTable = (struct LRUFrame *) calloc((size_t) (sizeof(struct LRUFrame) * numFrames),
                                              (size_t) 1);// array space is allocated
        initializeLRU();  //table is initialized
        file = fopen(traceFile, "r"); //trace file is read
        if (file == 0) {
            perror("fopen");
            exit(1);
        }
        while (fscanf(file, "%x %c", &addr, &rw) == 2) {
            numEvents++;
            LRU(addr, rw);
        }
    }
    else if(strcmp(algorithm,"fifo") == 0)
    {
        FIFOTable = (struct FIFOFrame *)calloc((size_t)(sizeof(struct FIFOFrame)*numFrames), (size_t)1);// array space is allocated
        initializeFIFO();  //table is initialized
        file = fopen(traceFile, "r"); //trace file is read
        if(file == 0)
        {
            perror("fopen");
            exit(1);
        }
        while(fscanf(file,"%x %c",&addr,&rw) == 2)
        {
            numEvents++;
            FIFO(addr, rw);
        }
    }
    PrintResults();
    free(LRUTable);
    free(FIFOTable);
    fclose(file);
}