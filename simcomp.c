//Header Files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#define true 1
#define false 0

//Global

struct processControlBlock
{
    //Identical to slide from design
    pid_t pid;
    pid_t parentPid;
    //Time of arrival
    //Time remaining
    int priority;
    //List of operations
};


//Function Declarations

//Main
int main()
{

    return 0;
}

//Function Implementations

