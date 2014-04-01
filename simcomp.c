//Header Files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define true 1
#define false 0

//Global

//We should probably place structs in files outside this one
//along with any data structures.

typedef struct processControlBlock
{
    pid_t pid;
    pid_t parentPid;
    time_t arrivalTime;
    time_t timeRemaining;
    int priority;
    //List of Actions/Operations (Busy waiting or I/O)
    int busyWaitFlag;
    struct processControlBlock *nextPCB;
}processControlBlock;

typedef struct taskInfoBlock
{
    //Operation Type/Name
    int totalCycles;
    int cyclesRemaining;
}taskInfoBlock;

//Function Declarations

//Main
int main()
{
    //Initialize variables
    taskInfoBlock test;
    processControlBlock test2;
    
    //Example of dynamic allocation using c (new is c++)
    test2.nextPCB = malloc(sizeof(processControlBlock));
    free(test2.nextPCB);
    
    //Initalize Simulated Computer
    
        //Read config file
        
        //Set config
        
        //Read program file
        
        //Populate Job Queue
        
    
    //Begin Simulation Loop
    while(false) //Jobs remaining?
    {
        while(false) //Is there an I/O Interrupt?
        {
            //Remove waiting process from I/O Queue
            
            //Place Job in back of Queue
        }
        
        //Perform Context Switch if Necessary
        
        //Process Task
        
        //Count Time Quantum as Passed
        
        //Log Output Data
    }
    
    //Print output to screen, file, or both
    
    return 0;
}

//Function Implementations

