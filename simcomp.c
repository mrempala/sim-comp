//Header Files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define true 1
#define false 0

//Global

//We should probably place structs in files outside this one
//along with any data structures.

//Based on design from the presentation
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
    
    //Boot-Up Simulated Computer
    
    //Begin Simulation Loop
    while(true)
    {
        //
    }
    
    //Print output to screen, file, or both
    
    return 0;
}

//Function Implementations

