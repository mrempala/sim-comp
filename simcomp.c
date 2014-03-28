//Header Files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define true 1
#define false 0

//Global

//We should probably place structs in files outside this one
//along with any data structures.

struct processControlBlock
{
    pid_t pid;
    pid_t parentPid;
    //Time of Arrival
    //Time Remaining
    int priority;
    //List of Actions/Operations
};

struct taskInfoBlock
{
    //Operation Type/Name
    int totalCycles;
    int cyclesRemaining;
};

//Function Declarations

//Main
int main()
{
    //Initialize variables
    
    
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
        
        //Process Task
        
        //Count Time Quantum as Passed
        
        //Log Output Data
    }
    
    //Print output to screen, file, or both
    
    return 0;
}

//Function Implementations

